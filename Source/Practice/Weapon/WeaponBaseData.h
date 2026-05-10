// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "WeaponTypes.h"
#include "WeaponBaseData.generated.h"

class UWeaponInputAbilityMap;

// ─────────────────────────────────────────────────────────────
//  Mesh attachment entry
// ─────────────────────────────────────────────────────────────

/** Maps one skeletal mesh to a character socket.
 *  A weapon may have multiple meshes (e.g. blade + guard). */
USTRUCT(BlueprintType)
struct PRACTICE_API FWeaponMeshAttachment
{
	GENERATED_BODY()

	/** The mesh to attach */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Visual")
	TObjectPtr<USkeletalMesh> Mesh;

	/** Socket on the character skeleton to attach the mesh to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Visual")
	FName SocketName;
};

// ─────────────────────────────────────────────────────────────
//  UWeaponBaseData
// ─────────────────────────────────────────────────────────────

/**
 * Static, shared definition of a weapon archetype.
 * All weapon instances of the same type share one WeaponBaseData asset.
 *
 * Visual data (mesh/socket) is separated here from gameplay data so that
 * a skin system can swap MeshAttachments without touching stat or ability data.
 */
UCLASS(BlueprintType)
class PRACTICE_API UWeaponBaseData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// ── Identity ──────────────────────────────────────────────

	/** Unique name identifier for this weapon archetype */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	FName WeaponID;

	/** Broad weapon category — drives animation set and input-ability map */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	EWeaponType WeaponType = EWeaponType::None;

	// ── Visual ────────────────────────────────────────────────

	/** List of mesh-to-socket attachments for this weapon.
	 *  Decoupled from gameplay data so skins can override this array alone. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Visual")
	TArray<FWeaponMeshAttachment> MeshAttachments;

	// ── Base Stats ────────────────────────────────────────────

	/** Raw damage applied on hit (before AttackPower scaling and Defense mitigation) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.0f))
	float BaseDamage = 10.0f;

	/** Damage contributed to the target's stagger gauge on hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.0f))
	float StaggerDamage = 5.0f;

	/** Stat bonuses applied as a GameplayEffect when this weapon is equipped.
	 *  Tag maps to the corresponding PlayerAttributeSet attribute. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	TArray<FWeaponStatBonus> BaseStatBonuses;

	// ── Abilities ─────────────────────────────────────────────

	/** Input → Ability mapping table for this weapon type.
	 *  Swapped on equip — drives which ability fires on which input. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Abilities")
	TObjectPtr<UWeaponInputAbilityMap> InputAbilityMap;

	// ── GAS Integration ───────────────────────────────────────

	/**
	 * GameplayEffect applied to the owner's ASC when this weapon is equipped.
	 * Should contain Infinite-duration modifiers for all BaseStatBonuses.
	 *
	 * Convention: create "GE_Equip_<WeaponID>" in the editor with modifiers
	 * matching BaseStatBonuses (Attribute.AttackPower += BaseDamage, etc.).
	 * The effect is automatically removed on unequip.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|GAS")
	TSubclassOf<UGameplayEffect> EquipGameplayEffect;

	// ── UPrimaryDataAsset ─────────────────────────────────────

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
