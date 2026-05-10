// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LogicModifierBase.generated.h"

class UAbilityLogicBase;

/**
 * Logic 프로퍼티를 수정하는 모디파이어 베이스.
 *
 * 서브클래스가 대상 Logic 타입을 Cast해서 직접 값을 변경한다.
 * Logic은 모디파이어의 존재를 전혀 모른다.
 *
 * 새로운 수정이 필요할 때:
 *   → 이 클래스를 상속하는 새 서브클래스만 추가
 *   → 기존 Logic 코드는 건드릴 필요 없음
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType)
class PRACTICE_API ULogicModifierBase : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Logic에 수정을 적용한다.
	 * 서브클래스에서 Cast<TargetLogicType>(Logic)으로 대상을 특정한다.
	 */
	virtual void Apply(UAbilityLogicBase* Logic) {}

	/**
	 * 적용했던 수정을 CDO 기본값으로 되돌린다.
	 * ScanAndApplyGEModifiers 시작 시 호출되어 누적을 방지한다.
	 */
	virtual void Reset(UAbilityLogicBase* Logic) {}
};
