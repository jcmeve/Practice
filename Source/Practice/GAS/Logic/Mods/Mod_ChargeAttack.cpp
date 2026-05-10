// Fill out your copyright notice in the Description page of Project Settings.

#include "Mod_ChargeAttack.h"
#include "Logic_ChargeAttack.h"

// ── UMod_ChargeAttack_MaxChargeTime ──────────────────────────────────────────

void UMod_ChargeAttack_MaxChargeTime::Apply(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MaxChargeTime += Bonus;
}

void UMod_ChargeAttack_MaxChargeTime::Reset(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MaxChargeTime = GetDefault<ULogic_ChargeAttack>(CA->GetClass())->MaxChargeTime;
}

// ── UMod_ChargeAttack_MinChargeTime ──────────────────────────────────────────

void UMod_ChargeAttack_MinChargeTime::Apply(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MinChargeTime += Bonus;
}

void UMod_ChargeAttack_MinChargeTime::Reset(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MinChargeTime = GetDefault<ULogic_ChargeAttack>(CA->GetClass())->MinChargeTime;
}

// ── UMod_ChargeAttack_DamageMultiplier ───────────────────────────────────────

void UMod_ChargeAttack_DamageMultiplier::Apply(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MaxChargeDamageMultiplier += Bonus;
}

void UMod_ChargeAttack_DamageMultiplier::Reset(UAbilityLogicBase* Logic)
{
	if (ULogic_ChargeAttack* CA = Cast<ULogic_ChargeAttack>(Logic))
		CA->MaxChargeDamageMultiplier = GetDefault<ULogic_ChargeAttack>(CA->GetClass())->MaxChargeDamageMultiplier;
}
