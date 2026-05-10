// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_PlayMontage.h"
#include "WeaponAbilityBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimInstance.h"

void ULogic_PlayMontage::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;

	if (!Montage)
	{
		return;
	}

	ActiveTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		Ability, NAME_None, Montage, PlayRate, StartSection, bStopWhenAbilityEnds);

	ActiveTask->OnCompleted.AddDynamic(this,   &ULogic_PlayMontage::OnMontageCompleted);
	ActiveTask->OnInterrupted.AddDynamic(this, &ULogic_PlayMontage::OnMontageInterrupted);
	ActiveTask->OnBlendOut.AddDynamic(this,    &ULogic_PlayMontage::OnMontageBlendOut);
	ActiveTask->ReadyForActivation();
}

void ULogic_PlayMontage::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	ActiveTask = nullptr;
	OwnerAbility = nullptr;
}

void ULogic_PlayMontage::JumpToSection(FName SectionName)
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !Montage)
	{
		return;
	}

	// 몽타주가 재생 중이면 해당 섹션으로 점프
	if (UAnimInstance* AnimInstance = Ability->GetCurrentActorInfo()
		? Ability->GetCurrentActorInfo()->GetAnimInstance() : nullptr)
	{
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ULogic_PlayMontage::OnMontageCompleted()
{
	if (bEndAbilityOnMontageEnd && OwnerAbility.IsValid())
	{
		OwnerAbility->RequestEnd();
	}
}

void ULogic_PlayMontage::OnMontageInterrupted()
{
	if (OwnerAbility.IsValid())
	{
		OwnerAbility->RequestEnd();
	}
}

void ULogic_PlayMontage::OnMontageBlendOut()
{
	// BlendOut은 Completed 직전에 발생 — Completed에서 처리
}

void ULogic_PlayMontage::Detach()
{
	// 델리게이트 연결 해제 — 이후 Interrupted 발생해도 어빌리티 종료 안 함
	if (ActiveTask)
	{
		ActiveTask->OnCompleted.RemoveDynamic(this,   &ULogic_PlayMontage::OnMontageCompleted);
		ActiveTask->OnInterrupted.RemoveDynamic(this, &ULogic_PlayMontage::OnMontageInterrupted);
		ActiveTask->OnBlendOut.RemoveDynamic(this,    &ULogic_PlayMontage::OnMontageBlendOut);
		ActiveTask = nullptr;
	}
	OwnerAbility = nullptr;
}
