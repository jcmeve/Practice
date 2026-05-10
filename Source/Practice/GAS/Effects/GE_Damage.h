// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Damage.generated.h"

/**
 * 피격 데미지 GameplayEffect — Instant.
 *
 * PlayerAttributeSet::IncomingDamage 메타 어트리뷰트에 AddItive로 값을 더한다.
 * 실제 수치는 Logic_SphereTrace가 ApplyGameplayEffectSpecToTarget 호출 시
 * SetSetByCallerMagnitude("Data.Damage", value) 로 주입한다.
 *
 * PostGameplayEffectExecute에서 IncomingDamage → Health 반영:
 *   Health -= max(0, IncomingDamage - Defense)
 */
UCLASS()
class PRACTICE_API UGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_Damage();
};
