// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "Logic_ComboAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;

/**
 * 콤보 공격 Logic — 몽타주 + 콤보 상태 통합.
 *
 * Logic_PlayMontage + Logic_ComboAdvance를 하나로 합침.
 * 섹션 전환은 AnimInstance::Montage_JumpToSection으로 직접 처리.
 *
 * LogicList:
 *   [0] Logic_ComboAttack  ← 이 클래스
 *   [1] Logic_SphereTrace  (DoAttackTrace 이벤트 구독)
 *
 * 입력 흐름 (새 설계):
 *   첫 번째 좌클릭 → TryActivateAbility
 *   두 번째 좌클릭 → GASCharacterBase가 InputPressedTag 이벤트 발송
 *                    → OnGameplayEvent → QueueInput()
 */
UCLASS(DisplayName = "Logic: Combo Attack")
class PRACTICE_API ULogic_ComboAttack : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/** 콤보 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Animation")
	TObjectPtr<UAnimMontage> ComboMontage;

	/** 각 콤보 히트의 섹션 이름 목록 (순서대로) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Animation")
	TArray<FName> ComboSections;

	/** 재생 배속 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Animation", meta = (ClampMin = 0.1f))
	float PlayRate = 1.0f;

	/**
	 * 활성 중 재입력 시 발송되는 InputTag.
	 * GASCharacterBase가 SendGameplayEventToActor로 이 태그를 발송한다.
	 * 예) "Input.PrimaryAttack"
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo|Input")
	FGameplayTag InputPressedTag;

	// ── UAbilityLogicBase ─────────────────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

private:

	int32 CurrentIndex = 0;
	bool  bInputQueued = false;
	bool  bWindowOpen  = false;

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	/** 현재 섹션으로 점프 (이미 몽타주가 재생 중이면 JumpToSection, 처음이면 태스크 생성) */
	void PlayOrJumpToSection();

	/** AnimInstance를 통해 직접 섹션 전환 */
	void JumpToSection(FName SectionName);

	void AdvanceCombo();

	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
};
