// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Skill_ChargeDelay.generated.h"

/**
 * 차지 유예 스킬 GameplayEffect — Infinite.
 *
 * PlayerAttributeSet::ChargeTimeBonus에 Additive로 값을 더한다.
 * Logic_ChargeAttack의 MaxChargeTime을 연장한다.
 *
 * 차지 공격은 이 GE의 존재를 모른다.
 * ChargeTimeBonus 어트리뷰트만 읽을 뿐이다.
 */
UCLASS()
class PRACTICE_API UGE_Skill_ChargeDelay : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_Skill_ChargeDelay();
};
