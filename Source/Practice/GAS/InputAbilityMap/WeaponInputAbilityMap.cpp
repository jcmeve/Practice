// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponInputAbilityMap.h"

TSubclassOf<UGameplayAbility> UWeaponInputAbilityMap::FindAbilityForInput(const FGameplayTag& InputTag) const
{
	for (const FInputAbilityEntry& Entry : Entries)
	{
		if (Entry.InputTag == InputTag)
		{
			return Entry.AbilityClass;
		}
	}
	return nullptr;
}

TArray<FInputAbilityEntry> UWeaponInputAbilityMap::GetEntriesByEvent(TEnumAsByte<EInputEvent> Event) const
{
	TArray<FInputAbilityEntry> Result;
	for (const FInputAbilityEntry& Entry : Entries)
	{
		if (Entry.TriggerEvent == Event)
		{
			Result.Add(Entry);
		}
	}
	return Result;
}

TArray<FInputAbilityEntry> UWeaponInputAbilityMap::GetEntriesByTag(const FGameplayTag& InputTag) const
{
	TArray<FInputAbilityEntry> Result;
	for (const FInputAbilityEntry& Entry : Entries)
	{
		if (Entry.InputTag == InputTag)
		{
			Result.Add(Entry);
		}
	}
	return Result;
}
