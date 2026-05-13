// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "AbilityDataTypes.h"
#include "AbilityModifierComponent.generated.h"

/**
 * GameplayEffect 컴포넌트 — 어빌리티 RuntimeData 수정.
 *
 * GE에 이 컴포넌트를 추가하면, 해당 GE가 활성화된 상태에서
 * 어빌리티 발동 시 지정한 DataTag의 값이 수정된다.
 *
 * Logic 클래스를 전혀 모른다. DataTag만 안다.
 * DataTag를 보유한 어빌리티라면 구현과 무관하게 동일하게 적용된다.
 *
 * 에디터 설정 예시:
 *   GE_Skill_ChargeDelay → AbilityModifierComponent 추가
 *     Modifiers[0]:
 *       TargetDataTag = Data.Charge.MaxChargeTime
 *       ModOp         = Additive
 *       Magnitude     = 1.2
 */
UCLASS(DisplayName = "Ability: Data Modifier")
class PRACTICE_API UAbilityModifierComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityMod")
	TArray<FAbilityTagModifier> Modifiers;
};
