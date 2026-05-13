// Fill out your copyright notice in the Description page of Project Settings.

#include "ComboAbility.h"
#include "PracticeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"

UComboAbility::UComboAbility()
{
	FGameplayTagContainer Tags;
	Tags.AddTag(PracticeGameplayTags::Ability_Attack_Combo);
	SetAssetTags(Tags);

	ActivationBlockedTags.AddTag(PracticeGameplayTags::State_Dead);
	ActivationBlockedTags.AddTag(PracticeGameplayTags::State_Staggered);
	ActivationOwnedTags.AddTag(PracticeGameplayTags::State_Attacking);
}

// ─────────────────────────────────────────────────────────────────────────────
//  ActivateAbility
// ─────────────────────────────────────────────────────────────────────────────

void UComboAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ScanAndApplyGEModifiers();

	CurrentIndex = 0;
	bInputQueued = false;
	bWindowOpen  = false;

	// 코어: 첫 번째 콤보 섹션 재생
	PlayCurrentSection();

	// CheckCombo 이벤트 구독
	CheckComboTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, PracticeGameplayTags::Event_Anim_CheckCombo, nullptr, false, true);
	CheckComboTask->EventReceived.AddDynamic(this, &UComboAbility::OnCheckComboEvent);
	CheckComboTask->ReadyForActivation();

	// 재입력 구독
	if (InputPressedTag.IsValid())
	{
		InputPressedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, InputPressedTag, nullptr, false, true);
		InputPressedTask->EventReceived.AddDynamic(this, &UComboAbility::OnInputPressedEvent);
		InputPressedTask->ReadyForActivation();
	}

	// Logic 실행 (SphereTrace 등)
	for (UAbilityLogicBase* Logic : LogicList)
	{
		if (Logic) Logic->OnExecute(this);
	}
	for (UAbilityLogicBase* Logic : InjectedLogics)
	{
		if (Logic) Logic->OnExecute(this);
	}

	SetupEventListeners();
}

// ─────────────────────────────────────────────────────────────────────────────
//  EndAbility
// ─────────────────────────────────────────────────────────────────────────────

void UComboAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	MontageTask      = nullptr;
	CheckComboTask   = nullptr;
	InputPressedTask = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  몽타주 관리
// ─────────────────────────────────────────────────────────────────────────────

void UComboAbility::PlayCurrentSection()
{
	if (!ComboMontage || !ComboSections.IsValidIndex(CurrentIndex)) return;

	if (!MontageTask)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ComboMontage, PlayRate, ComboSections[CurrentIndex], true);
		MontageTask->OnCompleted.AddDynamic(this,   &UComboAbility::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UComboAbility::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		JumpToSection(ComboSections[CurrentIndex]);
	}
}

void UComboAbility::JumpToSection(FName SectionName)
{
	if (!ComboMontage) return;
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	if (!Info) return;

	if (UAnimInstance* AnimInst = Info->GetAnimInstance())
	{
		AnimInst->Montage_JumpToSection(SectionName, ComboMontage);
	}
}

void UComboAbility::AdvanceCombo()
{
	++CurrentIndex;
	bInputQueued = false;
	bWindowOpen  = false;
	PlayCurrentSection();
}

// ─────────────────────────────────────────────────────────────────────────────
//  이벤트 처리
// ─────────────────────────────────────────────────────────────────────────────

void UComboAbility::OnCheckComboEvent(FGameplayEventData Payload)
{
	bWindowOpen = true;
	if (bInputQueued && ComboSections.IsValidIndex(CurrentIndex + 1))
	{
		AdvanceCombo();
	}
}

void UComboAbility::OnInputPressedEvent(FGameplayEventData Payload)
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

// ─────────────────────────────────────────────────────────────────────────────
//  Delegates
// ─────────────────────────────────────────────────────────────────────────────

void UComboAbility::OnMontageCompleted()   { RequestEnd(false); }
void UComboAbility::OnMontageInterrupted() { RequestEnd(true);  }
