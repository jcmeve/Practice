// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_ComboAdvance.h"
#include "WeaponAbilityBase.h"
#include "Logic_PlayMontage.h"
#include "PracticeGameplayTags.h"

void ULogic_ComboAdvance::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
	CurrentIndex = 0;
	bInputQueued = false;
	bWindowOpen  = false;
	// 이벤트 구독은 WeaponAbilityBase::SetupEventListeners가 처리
}

void ULogic_ComboAdvance::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	CurrentIndex = 0;
	bInputQueued = false;
	bWindowOpen  = false;
	OwnerAbility = nullptr;
}

TArray<FGameplayTag> ULogic_ComboAdvance::GetSubscribedEventTags() const
{
	return { PracticeGameplayTags::Event_Anim_CheckCombo };
}

void ULogic_ComboAdvance::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	bWindowOpen = true;

	if (bInputQueued && ComboSections.IsValidIndex(CurrentIndex + 1))
	{
		AdvanceCombo();
	}
}

void ULogic_ComboAdvance::QueueInput()
{
	if (bWindowOpen && ComboSections.IsValidIndex(CurrentIndex + 1))
	{
		AdvanceCombo();
	}
	else
	{
		bInputQueued = true;
	}
}

void ULogic_ComboAdvance::AdvanceCombo()
{
	if (!OwnerAbility.IsValid()) return;

	const int32 NextIndex = CurrentIndex + 1;
	if (!ComboSections.IsValidIndex(NextIndex))
	{
		OwnerAbility->RequestEnd();
		return;
	}

	CurrentIndex = NextIndex;
	bInputQueued = false;
	bWindowOpen  = false;

	if (ULogic_PlayMontage* MontageLogic = GetSiblingLogic<ULogic_PlayMontage>(OwnerAbility.Get()))
	{
		MontageLogic->JumpToSection(ComboSections[CurrentIndex]);
	}
}
