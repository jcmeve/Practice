// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityFragment.h"
#include "ChargeFragment.generated.h"

/**
 * 차지 공격 관련 수치 Fragment.
 *
 * AbilityTag "Ability.Attack.Charge"를 가진 어빌리티에 자동 추가된다.
 * 에디터에서 수치를 직접 수정할 수 있다.
 *
 * GE_Skill_ChargeDelay 예시:
 *   UAbilityModifierComponent
 *     TargetFragmentTag = "Fragment.Charge"
 *     PropertyName      = "MaxChargeTime"
 *     ModOp             = Additive
 *     Magnitude         = 1.2
 */
UCLASS(DisplayName = "Fragment: Charge Attack")
class PRACTICE_API UChargeFragment : public UAbilityFragment
{
	GENERATED_BODY()

public:
	UChargeFragment();

	/** 최대 차징 시간 (초). GE로 증가 가능. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge", meta = (ClampMin = 0.f, Units = "s"))
	float MaxChargeTime = 0.8f;

	/** 최소 차징 시간 — 이전 릴리즈 시 이 시간까지 대기 후 발동 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge", meta = (ClampMin = 0.f, Units = "s"))
	float MinChargeTime = 0.8f;

	/** 풀 차지 도달 시 데미지 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge", meta = (ClampMin = 1.f))
	float DamageMultiplier = 2.0f;
};
