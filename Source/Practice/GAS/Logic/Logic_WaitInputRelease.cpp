// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_WaitInputRelease.h"
#include "WeaponAbilityBase.h"
#include "AbilitySystemBlueprintLibrary.h"

void ULogic_WaitInputRelease::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
	StartTime    = Ability->GetWorld()->GetTimeSeconds();
}

void ULogic_WaitInputRelease::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	OwnerAbility = nullptr;
}

TArray<FGameplayTag> ULogic_WaitInputRelease::GetSubscribedEventTags() const
{
	if (TriggerEventTag.IsValid())
	{
		return { TriggerEventTag };
	}
	return {};
}

void ULogic_WaitInputRelease::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !ReleaseEventTag.IsValid()) return;

	AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	const float TimeHeld = Ability->GetWorld()->GetTimeSeconds() - StartTime;

	FGameplayEventData ReleasePayload;
	ReleasePayload.EventMagnitude = TimeHeld;
	ReleasePayload.Instigator     = Avatar;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Avatar, ReleaseEventTag, ReleasePayload);
}
