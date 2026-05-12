// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_ChargeAttack.h"
#include "WeaponAbilityBase.h"
#include "Logic_SphereTrace.h"
#include "PracticeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

// ─────────────────────────────────────────────────────────────────────────────
//  OnExecute
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ChargeAttack::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
	bReleased    = false;
	bFired       = false;
	StartTime    = Ability->GetWorld()->GetTimeSeconds();

	// 차지 루프 몽타주 직접 재생
	if (ChargeMontage)
	{
		ChargeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			Ability, NAME_None, ChargeMontage, 1.0f, NAME_None, true);
		ChargeMontageTask->OnInterrupted.AddDynamic(this, &ULogic_ChargeAttack::OnChargeInterrupted);
		ChargeMontageTask->ReadyForActivation();
	}

	const float ClampedMax = FMath::Max(MaxChargeTime, MinChargeTime);

	// MinCharge 타이머 — 릴리즈가 이미 왔다면 여기서 발동
	Ability->GetWorld()->GetTimerManager().SetTimer(
		MinTimer,
		[this]() { OnMinChargeReached(); },
		MinChargeTime,
		false);

	// MaxCharge 타이머 — 자동 발동
	Ability->GetWorld()->GetTimerManager().SetTimer(
		MaxTimer,
		[this]() { OnMaxChargeReached(); },
		ClampedMax,
		false);
}

// ─────────────────────────────────────────────────────────────────────────────
//  OnAbilityEnd
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ChargeAttack::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	Ability->GetWorld()->GetTimerManager().ClearTimer(MinTimer);
	Ability->GetWorld()->GetTimerManager().ClearTimer(MaxTimer);
	ChargeMontageTask = nullptr;
	AttackTask        = nullptr;
	OwnerAbility      = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  이벤트 구독 — TriggerEventTag (릴리즈 InputTag)
// ─────────────────────────────────────────────────────────────────────────────

TArray<FGameplayTag> ULogic_ChargeAttack::GetSubscribedEventTags() const
{
	if (TriggerEventTag.IsValid())
		return { TriggerEventTag };
	return {};
}

void ULogic_ChargeAttack::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	if (bFired || bReleased) return;

	bReleased = true;
	const float TimeHeld = OwnerAbility.IsValid()
		? OwnerAbility->GetWorld()->GetTimeSeconds() - StartTime
		: 0.f;

	OnReleaseEvent(TimeHeld);
}

// ─────────────────────────────────────────────────────────────────────────────
//  릴리즈 처리
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ChargeAttack::OnReleaseEvent(float TimeHeld)
{
	if (TimeHeld >= MinChargeTime)
	{
		// 최소 충전 달성 → 즉시 발동, MaxTimer 해제
		if (UWeaponAbilityBase* Ability = OwnerAbility.Get())
			Ability->GetWorld()->GetTimerManager().ClearTimer(MaxTimer);

		// MinTimer도 해제 (이미 릴리즈가 왔으니 MinTimer가 다시 트리거되면 안 됨)
		if (UWeaponAbilityBase* Ability = OwnerAbility.Get())
			Ability->GetWorld()->GetTimerManager().ClearTimer(MinTimer);

		Fire(TimeHeld);
	}
	// else: 얼리 릴리즈 — MinTimer가 만료될 때 OnMinChargeReached에서 발동
	// MaxTimer는 그대로 유지 (MinTimer보다 늦게 만료되므로 사실상 무의미하지만 안전망)
}

void ULogic_ChargeAttack::OnMinChargeReached()
{
	if (bFired) return;

	if (bReleased)
	{
		// 얼리 릴리즈 후 MinChargeTime 도달 → 1.0x 배율로 발동
		if (UWeaponAbilityBase* Ability = OwnerAbility.Get())
			Ability->GetWorld()->GetTimerManager().ClearTimer(MaxTimer);

		Fire(MinChargeTime);
	}
	// else: 아직 버튼을 누르고 있음 → 아무것도 안 함, MaxTimer 대기
}

void ULogic_ChargeAttack::OnMaxChargeReached()
{
	if (bFired) return;

	// MaxChargeTime 도달 → MaxChargeDamageMultiplier로 자동 발동
	if (UWeaponAbilityBase* Ability = OwnerAbility.Get())
		Ability->GetWorld()->GetTimerManager().ClearTimer(MinTimer);

	Fire(FMath::Max(MaxChargeTime, MinChargeTime));
}

// ─────────────────────────────────────────────────────────────────────────────
//  발동
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ChargeAttack::Fire(float TimeHeld)
{
	if (bFired) return;
	bFired = true;

	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability) return;

	// 차지 비율 계산 → Logic_SphereTrace 배율 설정
	const float Range = FMath::Max(MaxChargeTime - MinChargeTime, SMALL_NUMBER);
	const float t     = FMath::Clamp((TimeHeld - MinChargeTime) / Range, 0.f, 1.f);
	const float Mult  = FMath::Lerp(1.0f, MaxChargeDamageMultiplier, t);

	if (ULogic_SphereTrace* Trace = GetSiblingLogic<ULogic_SphereTrace>(Ability))
	{
		Trace->SetDamageMultiplier(Mult);
	}

	PlayAttackMontage();
}

void ULogic_ChargeAttack::PlayAttackMontage()
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability) return;

	// 차지 루프 태스크 델리게이트 해제
	// 공격 몽타주가 루프를 중단시켜도 OnChargeInterrupted가 호출되지 않도록
	if (ChargeMontageTask)
	{
		ChargeMontageTask->OnInterrupted.RemoveDynamic(this, &ULogic_ChargeAttack::OnChargeInterrupted);
		ChargeMontageTask = nullptr;
	}

	if (!AttackMontage)
	{
		// 공격 몽타주 없으면 바로 종료
		Ability->RequestEnd(false);
		return;
	}

	AttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		Ability, NAME_None, AttackMontage, PlayRate, AttackStartSection, true);

	AttackTask->OnCompleted.AddDynamic(this,   &ULogic_ChargeAttack::OnAttackCompleted);
	AttackTask->OnInterrupted.AddDynamic(this, &ULogic_ChargeAttack::OnAttackInterrupted);
	AttackTask->ReadyForActivation();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Delegates
// ─────────────────────────────────────────────────────────────────────────────

void ULogic_ChargeAttack::OnChargeInterrupted()
{
	// 외부 원인으로 루프 몽타주가 중단됐을 때 어빌리티 취소
	if (OwnerAbility.IsValid())
		OwnerAbility->RequestEnd(true);
}

void ULogic_ChargeAttack::OnAttackCompleted()
{
	if (OwnerAbility.IsValid())
		OwnerAbility->RequestEnd(false);
}

void ULogic_ChargeAttack::OnAttackInterrupted()
{
	if (OwnerAbility.IsValid())
		OwnerAbility->RequestEnd(true);
}
