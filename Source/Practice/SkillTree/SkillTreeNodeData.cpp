// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillTreeNodeData.h"

FPrimaryAssetId USkillTreeNodeData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("SkillTreeNode", NodeID);
}

bool USkillTreeNodeData::ArePrerequisitesMet(const TSet<FName>& UnlockedNodeIDs) const
{
	for (const TObjectPtr<USkillTreeNodeData>& Prereq : PrerequisiteNodes)
	{
		if (!Prereq || !UnlockedNodeIDs.Contains(Prereq->NodeID))
		{
			return false;
		}
	}
	return true;
}
