// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilityLogicBase.h"
#include "AbilityFragment.h"
#include "WeaponTypes.h"
#include "WeaponAbilityBase.generated.h"

class AGASCharacterBase;
class UWeaponInstance;
class UWeaponBaseData;
class UAbilityDataRegistry;

/**
 * B방식 무기 어빌리티 베이스 — 로직 주입 Shell.
 *
 * 이 클래스 자체는 어떤 몽타주를 재생할지, 어떻게 판정할지 전혀 모른다.
 * 모든 행동은 LogicList에 주입된 UAbilityLogicBase 서브클래스에서 온다.
 *
 *   ActivateAbility → LogicList 순회 → 각 Logic::OnExecute()
 *   EndAbility      → LogicList 순회 → 각 Logic::OnAbilityEnd()
 *   (재입력은 GASCharacterBase가 InputTag GameplayEvent로 발송 → Logic이 구독해서 처리)
 *
 * 에디터 세팅 예시:
 *
 *   [콤보 공격 어빌리티 BP]
 *     LogicList[0] = ULogic_ComboAttack  { ComboMontage, Sections, InputPressedTag }
 *     LogicList[1] = ULogic_SphereTrace  { Dist=75, Radius=75, DamageGE=GE_Damage }
 *
 *   [차지 공격 어빌리티 BP]
 *     LogicList[0] = ULogic_ChargeAttack { ChargeMontage, AttackMontage, TriggerEventTag }
 *     LogicList[1] = ULogic_SphereTrace  { Dist=120, DamageGE=GE_Damage }
 *
 *   [패링 어빌리티 BP]
 *     LogicList[0] = ULogic_SphereTrace  { Dist=100, DamageGE=GE_ParryDamage }
 */
UCLASS()
class PRACTICE_API UWeaponAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UWeaponAbilityBase();

	// ── AbilityData / Fragment ────────────────────────────────

	/**
	 * AbilityTag → Fragment 클래스 스키마 DataAsset.
	 * PostEditChangeProperty가 이 레지스트리를 참조해 Fragments를 자동 구성한다.
	 * 부모 BP에 한 번만 설정하면 자식 어빌리티가 상속한다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityData")
	TObjectPtr<UAbilityDataRegistry> DataRegistry;

	/**
	 * 이 어빌리티가 보유하는 Fragment 인스턴스 목록.
	 * AbilityTags 변경 시 PostEditChangeProperty가 자동으로 동기화한다.
	 * 각 Fragment의 수치를 에디터에서 직접 수정 가능.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "AbilityData")
	TArray<TObjectPtr<UAbilityFragment>> Fragments;

	/** 런타임 시 Fragment를 타입으로 탐색 */
	template<typename T>
	T* GetFragment() const
	{
		for (const auto& Pair : RuntimeFragments)
		{
			if (T* Typed = Cast<T>(Pair.Value.Get())) return Typed;
		}
		return nullptr;
	}

	// ── LogicList ─────────────────────────────────────────────

	/**
	 * 이 어빌리티의 모든 행동을 정의하는 로직 목록.
	 * 에디터에서 인스턴스로 추가하며, 각 항목은 독립적인 레이어를 담당한다.
	 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Logic")
	TArray<TObjectPtr<UAbilityLogicBase>> LogicList;

	// ── UGameplayAbility interface ────────────────────────────

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	// ── Logic 탐색 유틸 ───────────────────────────────────────

	/**
	 * LogicList + InjectedLogics에서 타입 T인 첫 번째 항목 반환.
	 * 주입된 Logic도 포함해 탐색한다.
	 */
	template<typename T>
	T* GetLogic() const
	{
		for (UAbilityLogicBase* Logic : LogicList)
		{
			if (T* Typed = Cast<T>(Logic)) return Typed;
		}
		for (UAbilityLogicBase* Logic : InjectedLogics)
		{
			if (T* Typed = Cast<T>(Logic)) return Typed;
		}
		return nullptr;
	}

	// ── 무기 접근 헬퍼 (Logic에서 사용) ──────────────────────

	UFUNCTION(BlueprintPure, Category = "WeaponAbility")
	AGASCharacterBase* GetGASCharacter() const;

	UFUNCTION(BlueprintPure, Category = "WeaponAbility")
	UWeaponInstance* GetCurrentWeapon() const;

	/** 레벨·AttackPower 반영된 최종 데미지 */
	UFUNCTION(BlueprintPure, Category = "WeaponAbility")
	float GetScaledDamage() const;

	/** 레벨 반영된 최종 경직 데미지 */
	UFUNCTION(BlueprintPure, Category = "WeaponAbility")
	float GetScaledStaggerDamage() const;

	UFUNCTION(BlueprintPure, Category = "WeaponAbility")
	EWeaponType GetWeaponType() const;

	/**
	 * Logic 클래스에서 어빌리티 종료를 요청할 때 사용.
	 * K2_EndAbility는 protected라 Logic(UObject)에서 직접 호출 불가.
	 */
	void RequestEnd(bool bWasCancelled = false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
	}

protected:

	/**
	 * 모든 Logic의 GetSubscribedEventTags()를 수집해
	 * 태그별 WaitGameplayEvent 태스크를 생성한다.
	 * ActivateAbility에서 OnExecute 이후에 호출됨.
	 */
	void SetupEventListeners();

	/** WaitGameplayEvent 태스크에서 이벤트 수신 시 호출 — 해당 태그 구독 Logic에 dispatch */
	UFUNCTION()
	void OnGameplayEventDispatched(FGameplayEventData Payload);

	/** 이벤트 리스너 태스크 목록 (GC 방지용 strong ref) */
	UPROPERTY()
	TArray<TObjectPtr<UAbilityTask_WaitGameplayEvent>> EventListenerTasks;

	/** GE 주입으로 추가된 임시 Logic 목록. EndAbility 시 정리. */
	UPROPERTY()
	TArray<TObjectPtr<UAbilityLogicBase>> InjectedLogics;

	/**
	 * Fragments를 복제 후 RuntimeFragments 구성,
	 * GE AbilityModifierComponent를 리플렉션으로 적용,
	 * ULogicInjectorComponent를 InjectedLogics에 추가.
	 */
	void ScanAndApplyGEModifiers();

	/**
	 * Fragments → RuntimeFragments 복제.
	 * Assert: 중복 FragmentTag.
	 */
	void BuildRuntimeFragments();

	/**
	 * 모든 Logic의 GetRequiredFragmentTags() 검사.
	 * 필요한 Fragment가 RuntimeFragments에 없으면 ensure 실패.
	 */
	void ValidateFragments() const;

	/** FragmentTag → 런타임 Fragment (GE Modifier가 수정) */
	TMap<FGameplayTag, TObjectPtr<UAbilityFragment>> RuntimeFragments;

#if WITH_EDITOR
	/**
	 * AbilityTags 변경 시 Fragments 배열을 레지스트리 스키마에 맞게 동기화.
	 * Assert: 같은 FragmentTag 중복 / 스키마에 없는 Fragment 수동 추가.
	 */
	void SyncFragmentsToTags();

public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
#endif
};

// ── GetSiblingLogic 템플릿 구현 ───────────────────────────────────────────────
// AbilityLogicBase.h에 선언된 템플릿을 WeaponAbilityBase 정의 이후에 구현

template<typename T>
T* UAbilityLogicBase::GetSiblingLogic(UWeaponAbilityBase* Ability) const
{
	return Ability ? Ability->GetLogic<T>() : nullptr;
}
