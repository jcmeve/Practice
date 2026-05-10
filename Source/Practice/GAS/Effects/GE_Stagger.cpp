// Fill out your copyright notice in the Description page of Project Settings.

#include "GE_Stagger.h"
#include "PlayerAttributeSet.h"
#include "PracticeGameplayTags.h"

UGE_Stagger::UGE_Stagger()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute  = UPlayerAttributeSet::GetIncomingStaggerDamageAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag   = PracticeGameplayTags::Data_StaggerDamage;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(Mod);
}
