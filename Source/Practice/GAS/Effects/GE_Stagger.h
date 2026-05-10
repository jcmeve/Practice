// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Stagger.generated.h"

/**
 * 경직 데미지 GameplayEffect — Instant.
 *
 * PlayerAttributeSet::IncomingStaggerDamage 메타 어트리뷰트에 Additive로 값을 더한다.
 * 실제 수치는 Logic_SphereTrace가 SetSetByCallerMagnitude("Data.StaggerDamage", value) 로 주입.
 *
 * PostGameplayEffectExecute에서 IncomingStaggerDamage → Stagger 반영:
 *   Stagger += IncomingStaggerDamage
 *   Stagger >= MaxStagger → Event.Stagger 발송 → 경직 상태 진입
 */
UCLASS()
class PRACTICE_API UGE_Stagger : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_Stagger();
};
