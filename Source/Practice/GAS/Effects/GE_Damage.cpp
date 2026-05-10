// Fill out your copyright notice in the Description page of Project Settings.

#include "GE_Damage.h"
#include "PlayerAttributeSet.h"
#include "PracticeGameplayTags.h"

UGE_Damage::UGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute  = UPlayerAttributeSet::GetIncomingDamageAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	// SetByCaller 생성자로 Magnitude 설정 (MagnitudeCalculationType은 protected)
	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag        = PracticeGameplayTags::Data_Damage;
	Mod.ModifierMagnitude      = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(Mod);
}
