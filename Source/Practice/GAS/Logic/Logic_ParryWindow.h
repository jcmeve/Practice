// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "Logic_ParryWindow.generated.h"

/**
 * 패링 윈도우 관리 레이어.
 *
 * AnimNotifyState 기반 이벤트로 동작:
 *   AnimNotifyState Begin → "Event.Anim.ParryWindowBegin" 발송 → OpenWindow()
 *   AnimNotifyState End   → "Event.Anim.ParryWindowEnd"   발송 → CloseWindow()
 *
 * FallbackDuration > 0 이면:
 *   Begin 이벤트를 받은 후 FallbackDuration초 뒤에 End 이벤트 없이도 자동 닫힘.
 *   AnimNotifyState를 쓰지 않는 간단한 경우에 사용.
 *   FallbackDuration = 0 이면 End 이벤트만으로 닫힘.
 *
 * GA_Guard, GA_ChargeAttack 양쪽에서 동일하게 재사용:
 *   → 몽타주에 ANS를 배치하는 것만으로 패링 윈도우 타이밍 조정 가능
 */
UCLASS(DisplayName = "Logic: Parry Window")
class PRACTICE_API ULogic_ParryWindow : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/**
	 * AnimNotifyState End 이벤트 없이 자동으로 닫힐 때까지의 시간 (초).
	 * 0 = End 이벤트가 올 때까지 유지.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParryWindow", meta = (ClampMin = 0.f, Units = "s"))
	float FallbackDuration = 0.f;

	// ── UAbilityLogicBase interface ───────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

	// ── Public API ────────────────────────────────────────────

	void CloseWindow();
	bool IsWindowOpen() const { return bActive; }

private:

	bool bActive = false;
	FTimerHandle FallbackTimer;
	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	void OpenWindow();
};
