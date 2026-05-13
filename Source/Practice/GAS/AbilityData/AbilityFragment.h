// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "AbilityFragment.generated.h"

/**
 * 어빌리티 데이터 Fragment 베이스.
 *
 * 관련 변수들을 논리적으로 묶는 단위.
 * AbilityTag ↔ Fragment 매핑은 AbilityDataRegistry에서 정의.
 *
 * 서브클래스는 생성자에서 FragmentTag를 설정한다.
 * WeaponAbilityBase의 PostEditChangeProperty가 AbilityTags 변경을 감지하면
 * 해당 FragmentTag의 Fragment 인스턴스를 Fragments 배열에 자동 추가한다.
 * → 에디터에서 세부 수치를 직접 수정 가능.
 *
 * 런타임 시:
 *   ActivateAbility → Fragments를 복제해서 RuntimeFragments 구성
 *   GE AbilityModifierComponent → TargetFragmentTag + PropertyName으로 수정
 *   Ability/Logic → GetFragment<T>()로 읽기
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType)
class PRACTICE_API UAbilityFragment : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * 이 Fragment를 식별하는 태그.
	 * 서브클래스 생성자에서 설정한다. 직접 수정하지 말 것.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fragment")
	FGameplayTag FragmentTag;

	/**
	 * 이 Fragment를 필요로 하는 AbilityTag 목록.
	 * AbilityDataRegistry가 이 정보를 기반으로 스키마를 구성한다.
	 * 서브클래스 생성자에서 설정한다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fragment")
	FGameplayTagContainer SupportedAbilityTags;
};
