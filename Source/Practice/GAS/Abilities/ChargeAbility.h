// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAbilityBase.h"
#include "ChargeAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

/**
 * 차지 공격 어빌리티 — 핵심 차지 메커니즘 직접 보유.
 *
 * 코어 (이 클래스가 직접 처리):
 *   - 차지 루프 몽타주 재생/관리
 *   - Min/Max 타이머 (RuntimeData에서 수치 읽기)
 *   - 릴리즈 감지 (TriggerEventTag 구독)
 *   - 발동 → 공격 몽타주 재생 → RequestEnd
 *
 * 가변 (Logic이 담당):
 *   - Logic_ParryWindow : 패링 기능 (선택)
 *   - Logic_SphereTrace : 피격 판정
 *   → 모두 이벤트 기반, 이 클래스와 결합 없음
 *
 * RuntimeData (UAbilityFragmentRegistry에서 자동 읽기, 없으면 아래 프로퍼티 폴백):
 *   Data.Charge.MaxChargeTime    → MaxChargeTime
 *   Data.Charge.MinChargeTime    → MinChargeTime
 *   Data.Charge.DamageMultiplier → MaxChargeDamageMultiplier
 */
UCLASS(BlueprintType, Blueprintable)
class PRACTICE_API UChargeAbility : public UWeaponAbilityBase
{
	GENERATED_BODY()

public:

	// ── 몽타주 ───────────────────────────────────────────────

	/** 홀드 중 재생되는 루프 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Animation")
	TObjectPtr<UAnimMontage> ChargeMontage;

	/** 발동 시 재생되는 공격 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 공격 몽타주 시작 섹션 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Animation")
	FName AttackStartSection = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Animation", meta = (ClampMin = 0.1f))
	float PlayRate = 1.0f;

	// ── 입력 ──────────────────────────────────────────────────

	/**
	 * 릴리즈를 감지할 InputTag.
	 * GASCharacterBase가 릴리즈 시 이 태그로 GameplayEvent를 발송한다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Input")
	FGameplayTag TriggerEventTag;

	// ── 폴백 수치 (DataRegistry 없을 때 사용) ─────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Fallback", meta = (ClampMin = 0.f))
	float MaxChargeTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Fallback", meta = (ClampMin = 0.f))
	float MinChargeTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge|Fallback", meta = (ClampMin = 1.f))
	float MaxChargeDamageMultiplier = 2.0f;

	// ── UGameplayAbility interface ────────────────────────────

	UChargeAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:

	// ── 런타임 상태 ───────────────────────────────────────────

	bool  bReleased              = false;
	bool  bFired                 = false;
	float StartTime              = 0.f;
	float RuntimeMaxChargeTime   = 0.8f;
	float RuntimeMinChargeTime   = 0.8f;
	float RuntimeDamageMultiplier = 2.0f;

	FTimerHandle MinTimer;
	FTimerHandle MaxTimer;

	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> ChargeMontageTask;
	UPROPERTY() TObjectPtr<UAbilityTask_PlayMontageAndWait> AttackMontageTask;
	UPROPERTY() TObjectPtr<UAbilityTask_WaitGameplayEvent>  ReleaseEventTask;

	// ── 내부 로직 ─────────────────────────────────────────────

	void PlayChargeMontage();
	void StartTimers();
	void SubscribeToReleaseEvent();

	void OnReleaseEvent(float TimeHeld);
	void OnMinChargeReached();
	void OnMaxChargeReached();
	void Fire(float TimeHeld);
	void PlayAttackMontage();

	UFUNCTION() void OnChargeInterrupted();
	UFUNCTION() void OnReleaseEventReceived(FGameplayEventData Payload);
	UFUNCTION() void OnAttackCompleted();
	UFUNCTION() void OnAttackInterrupted();
};
