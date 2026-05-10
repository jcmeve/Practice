// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotify_DoAttackTrace.h"
#include "CombatAttacker.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PracticeGameplayTags.h"

void UAnimNotify_DoAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner) return;

	// 기존 CombatCharacter 인터페이스 호환 유지
	if (ICombatAttacker* AttackerInterface = Cast<ICombatAttacker>(Owner))
	{
		AttackerInterface->DoAttackTrace(AttackBoneName);
	}

	// GAS 시스템: Logic_SphereTrace가 구독하는 이벤트 발송
	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Owner,
		PracticeGameplayTags::Event_Anim_DoAttackTrace,
		Payload);
}

FString UAnimNotify_DoAttackTrace::GetNotifyName_Implementation() const
{
	return FString("Do Attack Trace");
}