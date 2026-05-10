// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "GameplayEffect.h"
#include "Logic_ChargeAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

/**
 * 차지 공격 전용 로직 — 타이밍 + 공격 몽타주 + 얼리 릴리즈 처리 통합.
 *
 * Phase 1 (루프):
 *   OnExecute → MinChargeTimer + MaxChargeTimer 시작
 *              + TriggerEventTag(릴리즈 이벤트) 구독
 *
 * 릴리즈 처리:
 *   · TimeHeld >= MinChargeTime → 즉시 발동
 *   · TimeHeld < MinChargeTime  → MinChargeTime까지 대기 후 발동 (1.0x 배율)
 *   · MaxChargeTime 도달         → 자동 발동 (MaxChargeDamageMultiplier)
 *
 * Phase 2 (공격):
 *   → Logic_PlayMontage Detach (루프 중단 시 어빌리티 종료 방지)
 *   → 공격 몽타주 재생
 *   → Event.Anim.DoAttackTrace → Logic_SphereTrace가 데미지 처리
 *   → 몽타주 완료 → RequestEnd
 *
 * BP 세팅:
 *   LogicTag            = "Logic.ChargeTimer"  ← GE 모디파이어가 이 태그로 수정
 *   TriggerEventTag     = "Input.SecondaryAttack"
 *   AttackMontage       = SM_ChargeAttack
 *   MinChargeTime       = 0.8
 *   MaxChargeTime       = 0.8  (스킬로 늘어남)
 *
 * LogicList (BP_Ability_SwordCharge):
 *   [0] Logic_PlayMontage   { SM_ChargeLoop, EndOnEnd=false }
 *   [1] Logic_ChargeAttack  ← 이 클래스
 *   [2] Logic_SphereTrace   { DamageGE, StaggerGE }
 */
UCLASS(DisplayName = "Logic: Charge Attack")
class PRACTICE_API ULogic_ChargeAttack : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	// ── 타이밍 ────────────────────────────────────────────────

	/** 최소 차징 시간 — 이전 릴리즈 시 대기 후 발동 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Timer", meta = (ClampMin = 0.f, Units = "s"))
	float MinChargeTime = 0.8f;

	/** 최대 차징 시간 — 도달 시 자동 발동. GE로 늘릴 수 있음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Timer", meta = (ClampMin = 0.f, Units = "s"))
	float MaxChargeTime = 0.8f;

	/** 풀 차지 시 데미지 배율 (Logic_SphereTrace에 전달) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Timer", meta = (ClampMin = 1.f))
	float MaxChargeDamageMultiplier = 2.0f;

	/** 릴리즈를 감지할 InputTag (Logic_WaitInputRelease의 TriggerEventTag와 역할 동일) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Input")
	FGameplayTag TriggerEventTag;

	// ── 공격 몽타주 ───────────────────────────────────────────

	/** 릴리즈/자동발동 시 재생할 공격 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Montage")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 공격 몽타주 시작 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Montage")
	FName AttackStartSection = NAME_None;

	/** 재생 배속 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge|Montage", meta = (ClampMin = 0.1f))
	float PlayRate = 1.0f;

	// ── UAbilityLogicBase ─────────────────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;


private:

	bool  bReleased   = false;
	bool  bFired      = false;
	float StartTime   = 0.f;

	FTimerHandle MinTimer;
	FTimerHandle MaxTimer;

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> AttackTask;

	void OnReleaseEvent(float TimeHeld);
	void OnMinChargeReached();
	void OnMaxChargeReached();
	void Fire(float TimeHeld);
	void PlayAttackMontage();

	UFUNCTION() void OnAttackCompleted();
	UFUNCTION() void OnAttackInterrupted();
};
