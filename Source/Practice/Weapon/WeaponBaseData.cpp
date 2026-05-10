// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBaseData.h"

FPrimaryAssetId UWeaponBaseData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("WeaponBase", WeaponID);
}
