// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "WeaponInputAbilityMap.generated.h"

// ─────────────────────────────────────────────────────────────
//  FInputAbilityEntry
// ─────────────────────────────────────────────────────────────

/**
 * One row in the weapon's input → ability table.
 *
 * Example rows for a sword:
 *   Input.PrimaryAttack  / IE_Pressed  → GA_ComboAttack
 *   Input.SecondaryAttack/ IE_Pressed  → GA_ChargeAttack (hold via AbilityTask)
 *   Input.SecondaryAttack/ IE_Released → GA_ChargeRelease
 *   Input.Dodge          / IE_Pressed  → GA_Dodge
 *
 * Example rows for a shield weapon:
 *   Input.PrimaryAttack  / IE_Pressed  → GA_ShieldBash
 *   Input.Guard          / IE_Pressed  → GA_Guard
 *   Input.Guard          / IE_Released → GA_GuardEnd
 *   Input.Guard (parry window hit)     → GA_Parry  ← activated via GameplayEvent, not direct input
 */
USTRUCT(BlueprintType)
struct PRACTICE_API FInputAbilityEntry
{
	GENERATED_BODY()

	/**
	 * Input tag that identifies the player action.
	 * e.g. "Input.PrimaryAttack", "Input.Guard", "Input.Dodge"
	 *
	 * These tags should be defined in your project GameplayTags.ini and
	 * matched in the EnhancedInput binding code on the AbilitySystemComponent owner.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputAbilityMap")
	FGameplayTag InputTag;

	/** Ability class to grant and activate when this input fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputAbilityMap")
	TSubclassOf<UGameplayAbility> AbilityClass;

	/**
	 * When to trigger: IE_Pressed (default), IE_Released, IE_Repeat.
	 * Charge attacks often have Pressed → start hold task, Released → release task.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputAbilityMap")
	TEnumAsByte<EInputEvent> TriggerEvent = IE_Pressed;

	/** Ability level passed to GrantAbility / TryActivateAbility */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputAbilityMap", meta = (ClampMin = 1))
	int32 AbilityLevel = 1;
};

// ─────────────────────────────────────────────────────────────
//  UWeaponInputAbilityMap
// ─────────────────────────────────────────────────────────────

/**
 * Per-weapon-type data asset that maps input tags to GameplayAbilities.
 *
 * Assigned on UWeaponBaseData; swapped in full when the player equips a weapon.
 * The equip flow should:
 *   1. Revoke all abilities granted by the previous map
 *   2. Grant all abilities in the new map
 *   3. Rebind the input tags to the new ability specs
 *
 * This design means Parry is one ability class shared across multiple maps
 * but bound to different input tags (Guard key on shield, SecondaryAttack on sword).
 */
UCLASS(BlueprintType)
class PRACTICE_API UWeaponInputAbilityMap : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Full table of input → ability bindings for this weapon type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAbilityMap")
	TArray<FInputAbilityEntry> Entries;

	// ── Queries ───────────────────────────────────────────────

	/** Returns the ability class for the first entry matching InputTag, or nullptr. */
	UFUNCTION(BlueprintPure, Category = "InputAbilityMap")
	TSubclassOf<UGameplayAbility> FindAbilityForInput(const FGameplayTag& InputTag) const;

	/** Returns all entries that match the given trigger event (useful for batch binding). */
	UFUNCTION(BlueprintPure, Category = "InputAbilityMap")
	TArray<FInputAbilityEntry> GetEntriesByEvent(TEnumAsByte<EInputEvent> Event) const;

	/** Returns all entries matching a specific input tag (a tag may appear twice: Pressed + Released). */
	UFUNCTION(BlueprintPure, Category = "InputAbilityMap")
	TArray<FInputAbilityEntry> GetEntriesByTag(const FGameplayTag& InputTag) const;
};
