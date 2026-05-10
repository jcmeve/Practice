// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponInstance.h"
#include "WeaponBaseData.h"

TArray<FWeaponStatBonus> UWeaponInstance::GetAllStatBonuses() const
{
	TArray<FWeaponStatBonus> AllBonuses;

	if (BaseData)
	{
		AllBonuses.Append(BaseData->BaseStatBonuses);
	}

	AllBonuses.Append(RandomStats);

	return AllBonuses;
}

float UWeaponInstance::GetEffectiveDamage() const
{
	if (!BaseData)
	{
		return 0.0f;
	}

	// Linear level scaling: BaseDamage * (1 + 0.1 * (Level - 1))
	// Override this in a subclass or curve table for non-linear progressions.
	return BaseData->BaseDamage * (1.0f + 0.1f * static_cast<float>(Level - 1));
}

float UWeaponInstance::GetEffectiveStaggerDamage() const
{
	if (!BaseData)
	{
		return 0.0f;
	}

	return BaseData->StaggerDamage * (1.0f + 0.1f * static_cast<float>(Level - 1));
}

bool UWeaponInstance::IsValidInstance() const
{
	return BaseData != nullptr && !BaseData->WeaponID.IsNone();
}

EWeaponType UWeaponInstance::GetWeaponType() const
{
	return BaseData ? BaseData->WeaponType : EWeaponType::None;
}
