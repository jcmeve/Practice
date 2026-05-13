// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "Logic_FireOnRelease.generated.h"

/**
 * 릴리즈 InputTag를 받아 Fire 태그를 발행하는 Logic.
 *
 * GASCharacterBase가 우클릭 해제 시 "Input.SecondaryAttack"을 발송한다.
 * 이 Logic이 구독 후 수신하면 ChargeAbility가 리스닝하는 FireEventTag를 발송한다.
 *
 * ChargeAbility.TriggerEventTag = "Event.Weapon.ChargeReleased"
 *
 * LogicList 예시 (BP_Ability_SwordCharge):
 *   [0] Logic_FireOnRelease
 *         TriggerEventTag = "Input.SecondaryAttack"
 *         FireEventTag    = "Event.Weapon.ChargeReleased"
 *   [1] Logic_SphereTrace
 *
 * 흐름:
 *   우클릭 해제
 *     → GASCharacterBase: SendGameplayEvent("Input.SecondaryAttack")
 *     → Logic_FireOnRelease 수신
 *     → SendGameplayEvent("Event.Weapon.ChargeReleased")
 *     → ChargeAbility::OnReleaseEventReceived → Fire()
 */
UCLASS(DisplayName = "Logic: Fire On Release")
class PRACTICE_API ULogic_FireOnRelease : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/** 구독할 릴리즈 InputTag (GASCharacterBase가 해제 시 발송) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireOnRelease")
	FGameplayTag TriggerEventTag;

	/** 수신 시 발송할 Fire 태그 (ChargeAbility::TriggerEventTag와 일치해야 함) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireOnRelease")
	FGameplayTag FireEventTag;

	// ── UAbilityLogicBase ─────────────────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

private:

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;
};
