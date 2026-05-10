// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilityLogicBase.h"
#include "WeaponTypes.h"
#include "WeaponAbilityBase.generated.h"

class AGASCharacterBase;
class UWeaponInstance;
class UWeaponBaseData;

/**
 * B방식 무기 어빌리티 베이스 — 로직 주입 Shell.
 *
 * 이 클래스 자체는 어떤 몽타주를 재생할지, 어떻게 판정할지 전혀 모른다.
 * 모든 행동은 LogicList에 주입된 UAbilityLogicBase 서브클래스에서 온다.
 *
 *   ActivateAbility → LogicList 순회 → 각 Logic::OnExecute()
 *   EndAbility      → LogicList 순회 → 각 Logic::OnAbilityEnd()
 *   InputPressed    → ULogic_ComboAdvance::QueueInput() (있으면)
 *
 * 에디터 세팅 예시:
 *
 *   [콤보 공격 어빌리티 BP]
 *     LogicList[0] = ULogic_PlayMontage   { Montage=SM_Combo, Sections=["Hit1",...] }
 *     LogicList[1] = ULogic_ComboAdvance  { ComboSections=["Hit1","Hit2","Hit3"] }
 *     LogicList[2] = ULogic_SphereTrace   { Dist=75, Radius=75, DamageGE=GE_Damage }
 *
 *   [방패 가드 어빌리티 BP]
 *     LogicList[0] = ULogic_PlayMontage       { Montage=SM_GuardIdle, EndAbilityOnEnd=false }
 *     LogicList[1] = ULogic_ParryWindow        { Duration=0.35 }
 *     LogicList[2] = ULogic_WaitInputRelease   { ReleaseEventTag="Event.Guard.Released" }
 *
 *   [패링 어빌리티 BP]
 *     LogicList[0] = ULogic_PlayMontage   { Montage=SM_ParryCounter }
 *     LogicList[1] = ULogic_SphereTrace   { Dist=100, DamageGE=GE_ParryDamage }
 */
UCLASS()
class PRACTICE_API UWeaponAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UWeaponAbilityBase();

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

	/** 동일 입력이 재입력될 때 콤보 큐잉으로 전달 */
	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

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

private:

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
	 * 발동 시 ASC의 활성 GE를 스캔:
	 *   ULogicModifierComponent → 매칭 Logic에 ApplyGEModifier
	 *   ULogicInjectorComponent → Logic 복제 후 InjectedLogics에 추가
	 * OnExecute 이전에 호출되어야 한다.
	 */
	void ScanAndApplyGEModifiers();
};

// ── GetSiblingLogic 템플릿 구현 ───────────────────────────────────────────────
// AbilityLogicBase.h에 선언된 템플릿을 WeaponAbilityBase 정의 이후에 구현

template<typename T>
T* UAbilityLogicBase::GetSiblingLogic(UWeaponAbilityBase* Ability) const
{
	return Ability ? Ability->GetLogic<T>() : nullptr;
}
