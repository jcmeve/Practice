// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_InitStats.generated.h"

/**
 * 캐릭터 스폰 시 PlayerAttributeSet 초기값을 설정하는 GameplayEffect.
 *
 * GASCharacterBase::StartupEffects 배열에 등록하면 PossessedBy 시점에 자동 적용된다.
 * Duration: Instant (한 번 적용 후 제거)
 *
 * 기본값은 여기서 Override로 설정하고,
 * 캐릭터/직업별로 BP 자식 클래스를 만들어 값을 다르게 줄 수 있다.
 */
UCLASS(BlueprintType)
class PRACTICE_API UGE_InitStats : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_InitStats();
};
