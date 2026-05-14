// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityFragment.h"
#include "AbilityFragmentRegistry.generated.h"

USTRUCT(BlueprintType)
struct PRACTICE_API FAbilityFragmentRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schema")
	TArray<TSubclassOf<UAbilityFragment>> RequiredFragmentClasses;
};

/**
 * AbilityTag → 필요한 Fragment 클래스 스키마 DataAsset.
 * PostEditChangeProperty가 이 레지스트리를 참조해 Fragments를 자동 구성한다.
 * 부모 BP에 한 번만 설정하면 자식 어빌리티가 상속한다.
 */
UCLASS(BlueprintType)
class PRACTICE_API UAbilityFragmentRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** AbilityTag → 필요한 Fragment 클래스 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schema")
	TMap<FGameplayTag, FAbilityFragmentRequirements> AbilitySchema;

	UFUNCTION(BlueprintPure, Category = "Fragment")
	TArray<TSubclassOf<UAbilityFragment>> GetRequiredFragmentClasses(FGameplayTag AbilityTag) const;
};
