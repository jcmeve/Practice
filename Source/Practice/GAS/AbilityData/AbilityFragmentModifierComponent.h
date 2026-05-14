// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "AbilityFragmentModifierComponent.generated.h"

/**
 * GE의 UAbilityFragmentModifierComponent가 보유하는 단일 수정 명세.
 * Fragment 태그 + 프로퍼티 이름으로 대상을 특정하고 리플렉션으로 수정한다.
 */
USTRUCT(BlueprintType)
struct PRACTICE_API FAbilityFragmentModifier
{
	GENERATED_BODY()

	/** 수정할 Fragment 태그 — 예) "AbilityFragment.Charge" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FragmentMod")
	FGameplayTag TargetFragmentTag;

	/** Fragment 내 수정할 float 프로퍼티 이름 — 예) "MaxChargeTime" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FragmentMod")
	FName PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FragmentMod")
	TEnumAsByte<EGameplayModOp::Type> ModOp = EGameplayModOp::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FragmentMod")
	float Magnitude = 0.f;
};

/**
 * GameplayEffect 컴포넌트 — Fragment 프로퍼티 수정.
 *
 * GE에 이 컴포넌트를 추가하면, 해당 GE가 활성화된 상태에서
 * 어빌리티 발동 시 지정한 Fragment의 값이 수정된다.
 *
 * 에디터 설정 예시:
 *   GE → Gameplay Effect Components → Add → Fragment: Modifier
 *     Modifiers[0]:
 *       TargetFragmentTag = AbilityFragment.Charge
 *       PropertyName      = MaxChargeTime
 *       ModOp             = Additive
 *       Magnitude         = 1.2
 */
UCLASS(DisplayName = "Fragment: Modifier")
class PRACTICE_API UAbilityFragmentModifierComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FragmentMod")
	TArray<FAbilityFragmentModifier> Modifiers;
};
