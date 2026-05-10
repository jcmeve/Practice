// Fill out your copyright notice in the Description page of Project Settings.

#include "GE_Skill_ChargeDelay.h"
#include "LogicModifierComponent.h"
#include "Mod_ChargeAttack.h"
#include "PracticeGameplayTags.h"

UGE_Skill_ChargeDelay::UGE_Skill_ChargeDelay()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	ULogicModifierComponent* ModComp =
		CreateDefaultSubobject<ULogicModifierComponent>(TEXT("ChargeAttackMod"));

	ModComp->TargetLogicTag = PracticeGameplayTags::Logic_ChargeTimer;

	// 모디파이어가 Logic_ChargeAttack을 Cast해서 직접 수정
	// Logic_ChargeAttack 코드는 전혀 건드리지 않음
	UMod_ChargeAttack_MaxChargeTime* TimeMod =
		CreateDefaultSubobject<UMod_ChargeAttack_MaxChargeTime>(TEXT("MaxChargeTimeMod"));
	TimeMod->Bonus = 1.2f;

	ModComp->Modifiers.Add(TimeMod);
	GEComponents.Add(ModComp);
}
