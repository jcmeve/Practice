// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "AbilityLogicBase.h"
#include "LogicInjectorComponent.generated.h"

/**
 * GameplayEffect 컴포넌트 — 어빌리티 발동 시 Logic을 동적 주입.
 *
 * GE에 이 컴포넌트를 추가하면 해당 GE가 활성화된 상태에서
 * 어빌리티 발동 시 LogicsToInject의 Logic들이 임시로 추가된다.
 * EndAbility 시 자동 제거.
 *
 * 에디터 사용법:
 *   GE 에셋 → Details → Gameplay Effect Components → Add → Logic Injector
 *     LogicsToInject[0] = Logic_ParryWindow
 *       Duration = 0.4
 *
 * 차지 어빌리티에 패링 창을 동적으로 추가하는 예시:
 *   GE_Skill_ChargeParry (Infinite)
 *     LogicInjectorComponent
 *       LogicsToInject[0] = Logic_ParryWindow { Duration = 0.4 }
 */
UCLASS(DisplayName = "Logic: Injector")
class PRACTICE_API ULogicInjectorComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:

	/**
	 * 주입 조건 — 어빌리티가 이 태그를 모두 보유할 때만 주입한다 (AND 조건).
	 * 비어있으면 모든 어빌리티에 적용 (기존 동작).
	 *
	 * 예) [Ability.Attack.Charge, Weapon.Greatsword]
	 *   → 대검 차지 공격 어빌리티에만 주입
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicInject")
	FGameplayTagContainer RequiredAbilityTags;

	/**
	 * 발동 시 어빌리티 LogicList에 추가될 Logic 템플릿 목록.
	 * Instanced이므로 에디터에서 각 Logic의 프로퍼티를 직접 세팅할 수 있다.
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "LogicInject")
	TArray<TObjectPtr<UAbilityLogicBase>> LogicsToInject;
};
