// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityDataRegistry.h"

TArray<TSubclassOf<UAbilityFragment>> UAbilityDataRegistry::GetRequiredFragmentClasses(FGameplayTag AbilityTag) const
{
	if (const FAbilityFragmentRequirements* Req = AbilitySchema.Find(AbilityTag))
		return Req->RequiredFragmentClasses;
	return {};
}
