// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LogicModifierBase.h"
#include "Mod_ChargeAttack.generated.h"

class ULogic_ChargeAttack;

/** MaxChargeTime 증가 */
UCLASS(DisplayName = "Mod: ChargeAttack MaxChargeTime")
class PRACTICE_API UMod_ChargeAttack_MaxChargeTime : public ULogicModifierBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Bonus = 1.2f;

	virtual void Apply(UAbilityLogicBase* Logic) override;
	virtual void Reset(UAbilityLogicBase* Logic) override;
};

/** MinChargeTime 수정 */
UCLASS(DisplayName = "Mod: ChargeAttack MinChargeTime")
class PRACTICE_API UMod_ChargeAttack_MinChargeTime : public ULogicModifierBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Bonus = 0.f;

	virtual void Apply(UAbilityLogicBase* Logic) override;
	virtual void Reset(UAbilityLogicBase* Logic) override;
};

/** MaxChargeDamageMultiplier 수정 */
UCLASS(DisplayName = "Mod: ChargeAttack DamageMultiplier")
class PRACTICE_API UMod_ChargeAttack_DamageMultiplier : public ULogicModifierBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Bonus = 0.5f;

	virtual void Apply(UAbilityLogicBase* Logic) override;
	virtual void Reset(UAbilityLogicBase* Logic) override;
};
