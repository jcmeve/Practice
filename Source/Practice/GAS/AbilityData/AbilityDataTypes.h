// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "AbilityDataTypes.generated.h"

/**
 * AbilityTag에 연결된 데이터 하나의 값.
 * AbilityDataRegistry의 기본값 정의와 WeaponAbilityBase의 Override 배열에 사용.
 */
USTRUCT(BlueprintType)
struct PRACTICE_API FAbilityDataValue
{
	GENERATED_BODY()

	/** 데이터 태그 — 예) "Data.Charge.MaxChargeTime" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityData")
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityData")
	float Value = 0.f;
};

/**
 * GE의 UAbilityModifierComponent가 보유하는 단일 수정 명세.
 * Fragment 태그 + 프로퍼티 이름으로 대상을 특정하고 리플렉션으로 수정한다.
 */
USTRUCT(BlueprintType)
struct PRACTICE_API FAbilityTagModifier
{
	GENERATED_BODY()

	/** 수정할 Fragment 태그 — 예) "Fragment.Charge" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityMod")
	FGameplayTag TargetFragmentTag;

	/** Fragment 내 수정할 float 프로퍼티 이름 — 예) "MaxChargeTime" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityMod")
	FName PropertyName;

	/** 적용 방식 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityMod")
	TEnumAsByte<EGameplayModOp::Type> ModOp = EGameplayModOp::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityMod")
	float Magnitude = 0.f;
};
