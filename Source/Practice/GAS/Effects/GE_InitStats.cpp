// Fill out your copyright notice in the Description page of Project Settings.

#include "GE_InitStats.h"
#include "PlayerAttributeSet.h"

UGE_InitStats::UGE_InitStats()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// 초기값을 Override로 설정 (기존 값 대체)
	auto AddOverride = [this](FGameplayAttribute Attr, float Value)
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute  = Attr;
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Value));
		Modifiers.Add(Mod);
	};

	AddOverride(UPlayerAttributeSet::GetMaxHealthAttribute(),   100.f);
	AddOverride(UPlayerAttributeSet::GetHealthAttribute(),      100.f);
	AddOverride(UPlayerAttributeSet::GetMaxStaminaAttribute(),  100.f);
	AddOverride(UPlayerAttributeSet::GetStaminaAttribute(),     100.f);
	AddOverride(UPlayerAttributeSet::GetMaxStaggerAttribute(),  100.f);
	AddOverride(UPlayerAttributeSet::GetStaggerAttribute(),       0.f);
	AddOverride(UPlayerAttributeSet::GetAttackPowerAttribute(),  10.f);
	AddOverride(UPlayerAttributeSet::GetDefenseAttribute(),       5.f);
	AddOverride(UPlayerAttributeSet::GetMoveSpeedAttribute(),   600.f);
}
