// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponTypes.generated.h"

// ─────────────────────────────────────────────────────────────
//  Enums
// ─────────────────────────────────────────────────────────────

/** Broad category of the weapon. Drives animation set + input-ability map selection. */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None        UMETA(DisplayName = "None"),
	Sword       UMETA(DisplayName = "Sword"),
	Shield      UMETA(DisplayName = "Shield"),
	Spear       UMETA(DisplayName = "Spear"),
	Bow         UMETA(DisplayName = "Bow"),
	Staff       UMETA(DisplayName = "Staff"),
	Thrown      UMETA(DisplayName = "Thrown"),
};

/** Drop rarity / item grade. Controls random stat roll range (see 6-2-3). */
UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Uncommon    UMETA(DisplayName = "Uncommon"),
	Rare        UMETA(DisplayName = "Rare"),
	Epic        UMETA(DisplayName = "Epic"),
	Legendary   UMETA(DisplayName = "Legendary"),
};

/** How a stat modifier is applied in the final stat pipeline:
 *  Flat → PercentAdd → PercentMult → Override  (applied in that order) */
UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	Flat        UMETA(DisplayName = "Flat (+value)"),
	PercentAdd  UMETA(DisplayName = "PercentAdd (+% additive)"),
	PercentMult UMETA(DisplayName = "PercentMult (*% multiplicative)"),
	Override    UMETA(DisplayName = "Override (set value)"),
};

// ─────────────────────────────────────────────────────────────
//  Structs
// ─────────────────────────────────────────────────────────────

/** A single stat modifier — value + how to apply it. */
USTRUCT(BlueprintType)
struct PRACTICE_API FStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatModifier")
	EStatModifierType ModifierType = EStatModifierType::Flat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatModifier")
	float Value = 0.0f;
};

/** Associates a stat (identified by GameplayTag) with a modifier.
 *  Used for base weapon bonuses, random rolls, and enchantment effects.
 *  The tag maps to a GAS Attribute (e.g. "Stat.AttackPower" → AttackPower attribute). */
USTRUCT(BlueprintType)
struct PRACTICE_API FWeaponStatBonus
{
	GENERATED_BODY()

	/** Tag naming the target attribute (e.g. "Stat.AttackPower", "Stat.MoveSpeed") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStatBonus")
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStatBonus")
	FStatModifier Modifier;
};

/** One enchantment slot on a weapon instance. */
USTRUCT(BlueprintType)
struct PRACTICE_API FEnchantmentEntry
{
	GENERATED_BODY()

	/** Tag identifying the enchantment type (e.g. "Enchant.Flame", "Enchant.Lifesteal") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enchantment")
	FGameplayTag EnchantmentTag;

	/** Number of times this enchantment has been stacked / upgraded */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enchantment", meta = (ClampMin = 1))
	int32 Stack = 1;
};
