// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "GameplayTagContainer.h"
#include "Logic_WaitInputRelease.generated.h"

/**
 * 입력 해제 감지 로직 — WaitGameplayEvent 방식.
 *
 * UAbilityTask_WaitInputRelease는 GAS 전통 InputID 시스템에 의존하므로
 * 커스텀 태그 기반 입력 시스템에서는 동작하지 않는다.
 * 대신 GASCharacterBase가 릴리즈 시 InputTag를 GameplayEvent로 발송하고,
 * 이 Logic이 해당 이벤트를 구독해서 받는 방식을 사용한다.
 *
 * 설정:
 *   TriggerEventTag = 어빌리티를 활성화하는 InputTag와 동일한 태그
 *                     예) "Input.SecondaryAttack"
 *   ReleaseEventTag = 릴리즈 확정 시 하위 Logic에 발송할 태그
 *                     예) "Event.Weapon.ChargeReleased"
 *
 * 흐름:
 *   우클릭 해제 → GASCharacterBase가 "Input.SecondaryAttack" 이벤트 발송
 *   → Logic_WaitInputRelease 수신
 *   → TimeHeld 계산 (OnExecute 시점 기록)
 *   → ReleaseEventTag 이벤트 발송 (Payload.EventMagnitude = TimeHeld)
 *   → Logic_ChargeAttack 수신
 */
UCLASS(DisplayName = "Logic: Wait Input Release")
class PRACTICE_API ULogic_WaitInputRelease : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/**
	 * 릴리즈를 감지할 InputTag.
	 * GASCharacterBase가 릴리즈 시 이 태그로 GameplayEvent를 발송한다.
	 * 어빌리티를 활성화하는 InputTag와 동일하게 세팅해야 한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputRelease")
	FGameplayTag TriggerEventTag;

	/**
	 * 릴리즈 확정 시 발송할 GameplayEvent 태그.
	 * Logic_ChargeAttack 등의 ReleaseEventTag와 일치해야 한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputRelease")
	FGameplayTag ReleaseEventTag;

	// ── UAbilityLogicBase ─────────────────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

private:

	float StartTime = 0.f;
	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;
};
