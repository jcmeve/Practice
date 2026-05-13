// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAbilityBase.h"
#include "ComboAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

/**
 * 콤보 공격 어빌리티 — 핵심 콤보 메커니즘 직접 보유.
 *
 * 코어 (이 클래스가 직접 처리):
 *   - 콤보 몽타주 재생 / 섹션 전환
 *   - 콤보 상태 (CurrentIndex, InputQueued, WindowOpen)
 *   - Event.Anim.CheckCombo 구독 → 윈도우 관리
 *   - InputPressedTag 구독 → 콤보 큐잉
 *
 * 가변 (Logic이 담당):
 *   - Logic_SphereTrace : 피격 판정
 *   → 이벤트 기반, 이 클래스와 결합 없음
 */
UCLASS(BlueprintType, Blueprintable)
class PRACTICE_API UComboAbility : public UWeaponAbilityBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo|Animation")
	TObjectPtr<UAnimMontage> ComboMontage;

	/** 콤보 섹션 이름 목록 (순서대로) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo|Animation")
	TArray<FName> ComboSections;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo|Animation", meta = (ClampMin = 0.1f))
	float PlayRate = 1.0f;

	/**
	 * 활성 중 재입력 시 발송되는 InputTag.
	 * GASCharacterBase가 활성 어빌리티에 이 태그를 발송한다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo|Input")
	FGameplayTag InputPressedTag;

	// ── UGameplayAbility interface ────────────────────────────

	UComboAbility();

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

private:

	int32 CurrentIndex = 0;
	bool  bInputQueued = false;
	bool  bWindowOpen  = false;

	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY() TObjectPtr<UAbilityTask_WaitGameplayEvent>  CheckComboTask;
	UPROPERTY() TObjectPtr<UAbilityTask_WaitGameplayEvent>  InputPressedTask;

	void PlayCurrentSection();
	void JumpToSection(FName SectionName);
	void AdvanceCombo();

	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
	UFUNCTION() void OnCheckComboEvent(FGameplayEventData Payload);
	UFUNCTION() void OnInputPressedEvent(FGameplayEventData Payload);
};
