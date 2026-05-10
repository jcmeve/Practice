// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "Logic_ComboAdvance.generated.h"

class UAbilityTask_WaitGameplayEvent;
class ULogic_PlayMontage;

/**
 * 콤보 상태 기계 레이어.
 *
 * Event.Anim.CheckCombo 이벤트(AnimNotify에서 발생)를 구독하고
 * 입력이 큐잉돼 있으면 다음 섹션으로 전진한다.
 * 섹션 전환은 ULogic_PlayMontage::JumpToSection()을 통해 이루어져
 * 애니메이션 레이어와 로직 레이어가 분리된 채 협력한다.
 *
 * 입력 큐잉:
 *   WeaponAbilityBase::InputPressed() → QueueInput() 호출
 *   콤보 윈도우(CheckCombo 이후)가 열려있으면 즉시 어드밴스
 */
UCLASS(DisplayName = "Logic: Combo Advance")
class PRACTICE_API ULogic_ComboAdvance : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/**
	 * 몽타주 섹션 이름 목록 (콤보 순서대로).
	 * ULogic_PlayMontage의 StartSection이 이 배열의 [0]에 해당해야 한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<FName> ComboSections;

	// ── UAbilityLogicBase interface ───────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

	// ── Public API (WeaponAbilityBase::InputPressed에서 호출) ─

	/**
	 * 새 공격 입력이 들어왔을 때 호출.
	 * 윈도우가 열려있으면 즉시 어드밴스, 아니면 큐잉.
	 */
	void QueueInput();

private:

	int32 CurrentIndex  = 0;
	bool bInputQueued   = false;
	bool bWindowOpen    = false;

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	void AdvanceCombo();
};
