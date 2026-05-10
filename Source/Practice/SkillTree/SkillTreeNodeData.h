// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "SkillTreeNodeData.generated.h"

/**
 * Static authored definition of one node in the skill tree.
 *
 * Position and connections are data — no auto-layout algorithm.
 * The UI reads Position directly and draws edges from ConnectedNodes.
 *
 * Runtime state (unlocked / available / locked) is NOT stored here.
 * Track it in a save game or a USkillTreeComponent on the character.
 *
 * Asset manager type: "SkillTreeNode"
 */
UCLASS(BlueprintType)
class PRACTICE_API USkillTreeNodeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// ── Identity ──────────────────────────────────────────────

	/** Unique name used as the PrimaryAssetId name and for save-game keying */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Node")
	FName NodeID;

	/** Display name shown in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Node")
	FText DisplayName;

	/** Short description shown in the tooltip / detail panel */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Node")
	FText Description;

	/** Icon displayed on the node button */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Node")
	TObjectPtr<UTexture2D> Icon;

	// ── Layout ────────────────────────────────────────────────

	/** Canvas position (pixels) for the skill tree UI widget.
	 *  Authored by hand — deliberately not auto-computed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Layout")
	FVector2D Position = FVector2D::ZeroVector;

	// ── Unlock cost ───────────────────────────────────────────

	/** Skill points required to unlock this node */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Unlock", meta = (ClampMin = 1))
	int32 PointCost = 1;

	// ── Ability reward ────────────────────────────────────────

	/** GameplayAbility granted to the owning ASC when this node is unlocked.
	 *  Can be nullptr if the node only provides passive stat bonuses via a GE. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Ability")
	TSubclassOf<UGameplayAbility> GrantedAbility;

	// ── Graph topology ────────────────────────────────────────

	/** Nodes that must be unlocked before this one becomes available.
	 *  The UI should grey out this node until all prerequisites are met. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Graph")
	TArray<TObjectPtr<USkillTreeNodeData>> PrerequisiteNodes;

	/** Sibling nodes connected to this one — used only for edge rendering.
	 *  Directionality is encoded in PrerequisiteNodes; ConnectedNodes is for display. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillTree|Graph")
	TArray<TObjectPtr<USkillTreeNodeData>> ConnectedNodes;

	// ── UPrimaryDataAsset ─────────────────────────────────────

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// ── Helpers ───────────────────────────────────────────────

	/** Returns true if all prerequisite nodes are contained in the UnlockedNodeIDs set */
	UFUNCTION(BlueprintPure, Category = "SkillTree|Node")
	bool ArePrerequisitesMet(const TSet<FName>& UnlockedNodeIDs) const;
};
