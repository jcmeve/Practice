// Fill out your copyright notice in the Description page of Project Settings.

#include "ChargeAbility.h"
#include "ChargeAbilityFragment.h"
#include "Logic_SphereTrace.h"
#include "PracticeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UChargeAbility::UChargeAbility()
{
	// FragmentRegistry의 스키마 키로 사용됨
	// "Ability.Attack.Charge" → Data.Charge.* 자동 로드
	FGameplayTagContainer Tags;
	Tags.AddTag(PracticeGameplayTags::Ability_Attack_Charge);
	SetAssetTags(Tags);

	ActivationBlockedTags.AddTag(PracticeGameplayTags::State_Dead);
	ActivationBlockedTags.AddTag(PracticeGameplayTags::State_Staggered);
	ActivationOwnedTags.AddTag(PracticeGameplayTags::State_Attacking);
}

// ─────────────────────────────────────────────────────────────────────────────
//  ActivateAbility
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Fragment 복제 + GE Modifier 적용
	ScanAndApplyGEModifiers();

	// ChargeFragment에서 수치 읽기 (없으면 자체 프로퍼티 폴백)
	if (const UChargeAbilityFragment* CF = GetFragment<UChargeAbilityFragment>())
	{
		RuntimeMaxChargeTime    = CF->MaxChargeTime;
		RuntimeMinChargeTime    = CF->MinChargeTime;
		RuntimeDamageMultiplier = CF->DamageMultiplier;
	}
	else
	{
		RuntimeMaxChargeTime    = MaxChargeTime;
		RuntimeMinChargeTime    = MinChargeTime;
		RuntimeDamageMultiplier = MaxChargeDamageMultiplier;
	}

	bReleased = false;
	bFired    = false;
	StartTime = GetWorld()->GetTimeSeconds();

	// 코어: 차지 시작
	PlayChargeMontage();
	StartTimers();
	SubscribeToReleaseEvent();

	// Logic 실행 (Logic_ParryWindow 등 부가 기능)
	for (UAbilityLogicBase* Logic : LogicList)
	{
		if (Logic) Logic->OnExecute(this);
	}
	for (UAbilityLogicBase* Logic : InjectedLogics)
	{
		if (Logic) Logic->OnExecute(this);
	}

	// Logic 이벤트 리스너 설정 (SphereTrace, ParryWindow 등)
	SetupEventListeners();
}

// ─────────────────────────────────────────────────────────────────────────────
//  EndAbility
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MinTimer);
		GetWorld()->GetTimerManager().ClearTimer(MaxTimer);
	}

	ChargeMontageTask = nullptr;
	AttackMontageTask = nullptr;
	ReleaseEventTask  = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  차지 시작
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::PlayChargeMontage()
{
	if (!ChargeMontage) return;

	ChargeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ChargeMontage, PlayRate, NAME_None, true);
	ChargeMontageTask->OnInterrupted.AddDynamic(this, &UChargeAbility::OnChargeInterrupted);
	ChargeMontageTask->ReadyForActivation();
}

void UChargeAbility::StartTimers()
{
	const float ClampedMax = FMath::Max(RuntimeMaxChargeTime, RuntimeMinChargeTime);

	GetWorld()->GetTimerManager().SetTimer(
		MinTimer,
		[this]() { OnMinChargeReached(); },
		RuntimeMinChargeTime,
		false);

	GetWorld()->GetTimerManager().SetTimer(
		MaxTimer,
		[this]() { OnMaxChargeReached(); },
		ClampedMax,
		false);
}

void UChargeAbility::SubscribeToReleaseEvent()
{
	if (!TriggerEventTag.IsValid()) return;

	ReleaseEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, TriggerEventTag, nullptr, false, true);
	ReleaseEventTask->EventReceived.AddDynamic(this, &UChargeAbility::OnReleaseEventReceived);
	ReleaseEventTask->ReadyForActivation();
}

// ─────────────────────────────────────────────────────────────────────────────
//  릴리즈 처리
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::OnReleaseEventReceived(FGameplayEventData Payload)
{
	if (bFired || bReleased) return;
	bReleased = true;

	const float TimeHeld = GetWorld()->GetTimeSeconds() - StartTime;
	OnReleaseEvent(TimeHeld);
}

void UChargeAbility::OnReleaseEvent(float TimeHeld)
{
	if (TimeHeld >= RuntimeMinChargeTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxTimer);
		GetWorld()->GetTimerManager().ClearTimer(MinTimer);
		Fire(TimeHeld);
	}
	// else: 얼리 릴리즈 — MinTimer 만료 시 OnMinChargeReached에서 발동
}

void UChargeAbility::OnMinChargeReached()
{
	if (bFired) return;

	if (bReleased)
	{
		GetWorld()->GetTimerManager().ClearTimer(MaxTimer);
		Fire(RuntimeMinChargeTime);
	}
	// else: 아직 누르고 있음, MaxTimer 대기
}

void UChargeAbility::OnMaxChargeReached()
{
	if (bFired) return;
	GetWorld()->GetTimerManager().ClearTimer(MinTimer);
	Fire(FMath::Max(RuntimeMaxChargeTime, RuntimeMinChargeTime));
}

// ─────────────────────────────────────────────────────────────────────────────
//  발동
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::Fire(float TimeHeld)
{
	if (bFired) return;
	bFired = true;

	// 차지 배율 계산 → SphereTrace에 전달
	const float Range = FMath::Max(RuntimeMaxChargeTime - RuntimeMinChargeTime, SMALL_NUMBER);
	const float t     = FMath::Clamp((TimeHeld - RuntimeMinChargeTime) / Range, 0.f, 1.f);
	const float Mult  = FMath::Lerp(1.0f, RuntimeDamageMultiplier, t);

	if (ULogic_SphereTrace* Trace = GetLogic<ULogic_SphereTrace>())
	{
		Trace->SetDamageMultiplier(Mult);
	}

	PlayAttackMontage();
}

void UChargeAbility::PlayAttackMontage()
{
	// 루프 몽타주 중단 시 어빌리티 종료 방지 — 델리게이트 해제
	if (ChargeMontageTask)
	{
		ChargeMontageTask->OnInterrupted.RemoveDynamic(this, &UChargeAbility::OnChargeInterrupted);
		ChargeMontageTask = nullptr;
	}

	if (!AttackMontage)
	{
		RequestEnd(false);
		return;
	}

	AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, PlayRate, AttackStartSection, true);
	AttackMontageTask->OnCompleted.AddDynamic(this,   &UChargeAbility::OnAttackCompleted);
	AttackMontageTask->OnInterrupted.AddDynamic(this, &UChargeAbility::OnAttackInterrupted);
	AttackMontageTask->ReadyForActivation();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Delegates
// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::OnChargeInterrupted()  { RequestEnd(true);  }
void UChargeAbility::OnAttackCompleted()    { RequestEnd(false); }
void UChargeAbility::OnAttackInterrupted()  { RequestEnd(true);  }
