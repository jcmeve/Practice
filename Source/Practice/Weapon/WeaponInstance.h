// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeaponTypes.h"
#include "WeaponInstance.generated.h"

class UWeaponBaseData;

/**
 * Runtime instance of a weapon.
 *
 * WeaponBaseData holds shared, read-only archetype data (BaseDamage, type, etc.).
 * WeaponInstance holds the per-drop state: level, rarity, random stat rolls,
 * and any enchantments applied after the fact.
 *
 * Stat pipeline for a single bonus:
 *   FinalStat = (Base + SumFlat) * (1 + SumPercentAdd) * SumPercentMult
 *   (Override replaces the result entirely if present)
 */
UCLASS(BlueprintType)
class PRACTICE_API UWeaponInstance : public UObject
{
	GENERATED_BODY()

public:

	// ── Archetype ref ─────────────────────────────────────────

	/** Shared weapon definition. Must be set before this instance is used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Instance")
	TObjectPtr<UWeaponBaseData> BaseData;

	// ── Per-instance state ────────────────────────────────────

	/** Item level — scales BaseDamage and may gate enchantments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Instance", meta = (ClampMin = 1))
	int32 Level = 1;

	/** Drop rarity — controls how many random stat rolls this instance received */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Instance")
	EWeaponRarity Rarity = EWeaponRarity::Common;

	/** Random stat bonuses rolled at drop time (count and ranges come from rarity table, 6-2-3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Instance")
	TArray<FWeaponStatBonus> RandomStats;

	/** Enchantments applied to this instance (may affect stats, abilities, or visuals) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Instance")
	TArray<FEnchantmentEntry> Enchantments;

public:

	// ── Helpers ───────────────────────────────────────────────

	/** Returns combined stat bonus list: BaseData->BaseStatBonuses + RandomStats.
	 *  Enchantment bonuses are resolved separately through GameplayEffects. */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Instance")
	TArray<FWeaponStatBonus> GetAllStatBonuses() const;

	/** Returns level-scaled effective damage: BaseDamage * (1 + 0.1 * (Level - 1)) */
	UFUNCTION(BlueprintPure, Category = "Weapon|Instance")
	float GetEffectiveDamage() const;

	/** Returns level-scaled effective stagger damage */
	UFUNCTION(BlueprintPure, Category = "Weapon|Instance")
	float GetEffectiveStaggerDamage() const;

	/** Returns true if BaseData is set and WeaponID is valid */
	UFUNCTION(BlueprintPure, Category = "Weapon|Instance")
	bool IsValidInstance() const;

	/** Convenience accessor: weapon type from BaseData */
	UFUNCTION(BlueprintPure, Category = "Weapon|Instance")
	EWeaponType GetWeaponType() const;
};
