// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "LogicModifierBase.h"
#include "LogicModifierComponent.generated.h"

/**
 * GameplayEffect 컴포넌트 — Logic 프로퍼티 수정.
 *
 * Modifiers 배열에 ULogicModifierBase 서브클래스를 추가한다.
 * 각 서브클래스가 대상 Logic을 Cast해서 값을 직접 변경한다.
 *
 * 에디터 사용법:
 *   GE → Gameplay Effect Components → Add → Logic: Property Modifier
 *     TargetLogicTag = "Logic.ChargeTimer"
 *     Modifiers[0]   = Mod: ChargeAttack MaxChargeTime { Bonus=1.2 }
 */
UCLASS(DisplayName = "Logic: Property Modifier")
class PRACTICE_API ULogicModifierComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:

	/** 수정할 Logic의 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicMod")
	FGameplayTag TargetLogicTag;

	/** 적용할 모디파이어 목록 — 각 항목이 대상 Logic을 Cast해서 직접 수정 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "LogicMod")
	TArray<TObjectPtr<ULogicModifierBase>> Modifiers;
};
