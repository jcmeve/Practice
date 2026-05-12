// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_ComboAttack.h"
#include "WeaponAbilityBase.h"
#include "PracticeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimInstance.h"

// ─────────────────────────────────────────────────────────────────────────────
//  OnExecute
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ComboAttack::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility   = Ability;
	CurrentIndex   = 0;
	bInputQueued   = false;
	bWindowOpen    = false;

	PlayOrJumpToSection();
}

// ─────────────────────────────────────────────────────────────────────────────
//  OnAbilityEnd
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ComboAttack::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	MontageTask  = nullptr;
	OwnerAbility = nullptr;
	CurrentIndex = 0;
	bInputQueued = false;
	bWindowOpen  = false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  이벤트 구독
// ─────────────────────────────────────────────────────────────────────────────

TArray<FGameplayTag> ULogic_ComboAttack::GetSubscribedEventTags() const
{
	TArray<FGameplayTag> Tags;
	Tags.Add(PracticeGameplayTags::Event_Anim_CheckCombo);
	if (InputPressedTag.IsValid())
	{
		Tags.Add(InputPressedTag);
	}
	return Tags;
}

void ULogic_ComboAttack::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	if (EventTag == PracticeGameplayTags::Event_Anim_CheckCombo)
	{
		bWindowOpen = true;
		if (bInputQueued && ComboSections.IsValidIndex(CurrentIndex + 1))
		{
			AdvanceCombo();
		}
	}
	else if (InputPressedTag.IsValid() && EventTag == InputPressedTag)
	{
		// 활성 중 재입력 — QueueInput
		if (bWindowOpen && ComboSections.IsValidIndex(CurrentIndex + 1))
		{
			AdvanceCombo();
		}
		else
		{
			bInputQueued = true;
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  몽타주 관리
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ComboAttack::PlayOrJumpToSection()
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !ComboMontage || !ComboSections.IsValidIndex(CurrentIndex)) return;

	if (!MontageTask)
	{
		// 첫 번째 히트: 몽타주 태스크 생성
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			Ability, NAME_None, ComboMontage, PlayRate,
			ComboSections[CurrentIndex], true);

		MontageTask->OnCompleted.AddDynamic(this,   &ULogic_ComboAttack::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ULogic_ComboAttack::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		// 콤보 연계: 섹션만 전환
		JumpToSection(ComboSections[CurrentIndex]);
	}
}

void ULogic_ComboAttack::JumpToSection(FName SectionName)
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !ComboMontage) return;

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo) return;

	UAnimInstance* AnimInst = ActorInfo->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_JumpToSection(SectionName, ComboMontage);
	}
}

void ULogic_ComboAttack::AdvanceCombo()
{
	++CurrentIndex;
	bInputQueued = false;
	bWindowOpen  = false;
	PlayOrJumpToSection();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Delegates
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ComboAttack::OnMontageCompleted()
{
	if (OwnerAbility.IsValid())
		OwnerAbility->RequestEnd(false);
}

void ULogic_ComboAttack::OnMontageInterrupted()
{
	if (OwnerAbility.IsValid())
		OwnerAbility->RequestEnd(true);
}
