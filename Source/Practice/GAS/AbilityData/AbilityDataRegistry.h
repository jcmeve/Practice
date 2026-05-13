// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityFragment.h"
#include "AbilityDataRegistry.generated.h"

/**
 * AbilityTag 하나가 필요로 하는 Fragment 클래스 목록.
 */
USTRUCT(BlueprintType)
struct PRACTICE_API FAbilityFragmentRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schema")
	TArray<TSubclassOf<UAbilityFragment>> RequiredFragmentClasses;
};

/**
 * 프로젝트 전역 어빌리티 데이터 레지스트리 DataAsset.
 *
 * AbilitySchema:
 *   AbilityTag → 해당 태그를 가진 어빌리티가 자동으로 보유해야 할 Fragment 클래스 목록
 *   예) "Ability.Attack.Charge" → [UChargeFragment]
 *
 * PostEditChangeProperty 흐름:
 *   AbilityTags에 "Ability.Attack.Charge" 추가
 *   → GetRequiredFragmentClasses("Ability.Attack.Charge") 호출
 *   → UChargeFragment 인스턴스 자동 생성 + Fragments 배열에 추가
 *   → 에디터에서 MaxChargeTime / MinChargeTime / DamageMultiplier 수정 가능
 *
 * 런타임 흐름:
 *   ActivateAbility → Fragments 복제 → RuntimeFragments 구성
 *   GE AbilityModifier → Fragment 태그 + PropertyName으로 리플렉션 수정
 */
UCLASS(BlueprintType)
class PRACTICE_API UAbilityDataRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** AbilityTag → 필요한 Fragment 클래스 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schema")
	TMap<FGameplayTag, FAbilityFragmentRequirements> AbilitySchema;

	/** AbilityTag가 필요로 하는 Fragment 클래스 목록 반환 */
	UFUNCTION(BlueprintPure, Category = "AbilityData")
	TArray<TSubclassOf<UAbilityFragment>> GetRequiredFragmentClasses(FGameplayTag AbilityTag) const;
};
