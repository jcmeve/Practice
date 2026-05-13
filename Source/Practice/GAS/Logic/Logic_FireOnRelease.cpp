// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_FireOnRelease.h"
#include "WeaponAbilityBase.h"
#include "AbilitySystemBlueprintLibrary.h"

void ULogic_FireOnRelease::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
}

void ULogic_FireOnRelease::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	OwnerAbility = nullptr;
}

TArray<FGameplayTag> ULogic_FireOnRelease::GetSubscribedEventTags() const
{
	if (TriggerEventTag.IsValid())
		return { TriggerEventTag };
	return {};
}

void ULogic_FireOnRelease::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !FireEventTag.IsValid()) return;

	AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// 릴리즈 이벤트를 Fire 태그로 전달
	// Payload를 그대로 넘겨 ChargeAbility가 타임스탬프 등을 활용할 수 있게 함
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Avatar, FireEventTag, Payload);
}
