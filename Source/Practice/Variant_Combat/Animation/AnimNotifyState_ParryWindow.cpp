// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_ParryWindow.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PracticeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_ParryWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner) return;

	FGameplayEventData Payload;
	Payload.Instigator     = Owner;
	Payload.EventMagnitude = TotalDuration;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Owner,
		PracticeGameplayTags::Event_Anim_ParryWindowBegin,
		Payload);
}

void UAnimNotifyState_ParryWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner) return;

	FGameplayEventData Payload;
	Payload.Instigator = Owner;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Owner,
		PracticeGameplayTags::Event_Anim_ParryWindowEnd,
		Payload);
}

FString UAnimNotifyState_ParryWindow::GetNotifyName_Implementation() const
{
	return FString("Parry Window");
}
