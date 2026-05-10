// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_ParryWindow.h"
#include "WeaponAbilityBase.h"
#include "PracticeGameplayTags.h"
#include "AbilitySystemComponent.h"

void ULogic_ParryWindow::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
	bActive      = false;
	// 윈도우 열기는 Event.Anim.ParryWindowBegin 이벤트를 기다림
	// (FallbackDuration 방식을 쓰면 즉시 열고 싶다면 여기서 OpenWindow() 호출)
}

void ULogic_ParryWindow::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	CloseWindow();
	OwnerAbility = nullptr;
}

TArray<FGameplayTag> ULogic_ParryWindow::GetSubscribedEventTags() const
{
	return
	{
		PracticeGameplayTags::Event_Anim_ParryWindowBegin,
		PracticeGameplayTags::Event_Anim_ParryWindowEnd,
	};
}

void ULogic_ParryWindow::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	if (EventTag == PracticeGameplayTags::Event_Anim_ParryWindowBegin)
	{
		OpenWindow();
	}
	else if (EventTag == PracticeGameplayTags::Event_Anim_ParryWindowEnd)
	{
		CloseWindow();
	}
}

void ULogic_ParryWindow::OpenWindow()
{
	if (bActive) return;

	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability) return;

	if (UAbilitySystemComponent* ASC = Ability->GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(PracticeGameplayTags::State_InParryWindow);
		bActive = true;
	}

	// Fallback 자동 닫기
	if (FallbackDuration > 0.f)
	{
		Ability->GetWorld()->GetTimerManager().SetTimer(
			FallbackTimer,
			[this]() { CloseWindow(); },
			FallbackDuration,
			false
		);
	}
}

void ULogic_ParryWindow::CloseWindow()
{
	if (!bActive) return;

	if (UWeaponAbilityBase* Ability = OwnerAbility.Get())
	{
		Ability->GetWorld()->GetTimerManager().ClearTimer(FallbackTimer);

		if (UAbilitySystemComponent* ASC = Ability->GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(PracticeGameplayTags::State_InParryWindow);
		}
	}

	bActive = false;
}
