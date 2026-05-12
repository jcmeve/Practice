// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAbilityBase.h"
#include "GASCharacterBase.h"
#include "WeaponInstance.h"
#include "WeaponBaseData.h"
#include "PlayerAttributeSet.h"
#include "LogicModifierComponent.h"
#include "LogicInjectorComponent.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UWeaponAbilityBase::UWeaponAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Staggered"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  ActivateAbility — 모든 Logic::OnExecute 호출
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::ActivateAbility(
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

	// 1. GE 스캔 — 값 수정 및 Logic 주입 (OnExecute 이전에 실행)
	ScanAndApplyGEModifiers();

	// 2. 기본 Logic + 주입된 Logic 모두 실행
	for (UAbilityLogicBase* Logic : LogicList)
	{
		if (Logic) Logic->OnExecute(this);
	}
	for (UAbilityLogicBase* Logic : InjectedLogics)
	{
		if (Logic) Logic->OnExecute(this);
	}

	// 3. 기본 + 주입 Logic 모두의 구독 태그로 이벤트 리스너 생성
	SetupEventListeners();
}

// ─────────────────────────────────────────────────────────────────────────────
//  EndAbility — 모든 Logic::OnAbilityEnd 호출
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	for (UAbilityLogicBase* Logic : LogicList)
	{
		if (Logic) Logic->OnAbilityEnd(this, bWasCancelled);
	}
	for (UAbilityLogicBase* Logic : InjectedLogics)
	{
		if (Logic) Logic->OnAbilityEnd(this, bWasCancelled);
	}
	InjectedLogics.Empty();
	EventListenerTasks.Empty();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  GE 스캔 — Logic 값 수정 + 주입
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::ScanAndApplyGEModifiers()
{
	InjectedLogics.Empty();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// 1단계: 수정될 Logic+Modifier 쌍 수집
	struct FPendingMod { UAbilityLogicBase* Logic; ULogicModifierBase* Mod; };
	TArray<FPendingMod> PendingMods;

	FGameplayEffectQuery Query;
	TArray<FActiveGameplayEffectHandle> Handles = ASC->GetActiveEffects(Query);

	for (const FActiveGameplayEffectHandle& Handle : Handles)
	{
		const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle);
		if (!ActiveGE || !ActiveGE->Spec.Def) continue;

		const UGameplayEffect* GEDef = ActiveGE->Spec.Def;

		if (const ULogicModifierComponent* ModComp = GEDef->FindComponent<ULogicModifierComponent>())
		{
			for (UAbilityLogicBase* Logic : LogicList)
			{
				if (!Logic || Logic->LogicTag != ModComp->TargetLogicTag) continue;
				for (ULogicModifierBase* Mod : ModComp->Modifiers)
				{
					if (Mod) PendingMods.Add({ Logic, Mod });
				}
			}
		}

		if (const ULogicInjectorComponent* InjComp = GEDef->FindComponent<ULogicInjectorComponent>())
		{
			for (const TObjectPtr<UAbilityLogicBase>& Template : InjComp->LogicsToInject)
			{
				if (Template) InjectedLogics.Add(DuplicateObject<UAbilityLogicBase>(Template, this));
			}
		}
	}

	// 2단계: CDO로 리셋 (누적 방지)
	for (const FPendingMod& PM : PendingMods)
		PM.Mod->Reset(PM.Logic);

	// 3단계: mod->Apply(logic) — 모디파이어가 Logic을 Cast해서 직접 수정
	for (const FPendingMod& PM : PendingMods)
		PM.Mod->Apply(PM.Logic);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Event Dispatch
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::SetupEventListeners()
{
	// 기본 + 주입 Logic 모두의 구독 태그를 중복 없이 수집
	TSet<FGameplayTag> UniqueTags;
	auto CollectTags = [&](const TArray<TObjectPtr<UAbilityLogicBase>>& Logics)
	{
		for (const UAbilityLogicBase* Logic : Logics)
		{
			if (!Logic) continue;
			for (const FGameplayTag& Tag : Logic->GetSubscribedEventTags())
				UniqueTags.Add(Tag);
		}
	};
	CollectTags(LogicList);
	CollectTags(InjectedLogics);

	// 태그별로 WaitGameplayEvent 태스크 생성
	for (const FGameplayTag& Tag : UniqueTags)
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, Tag, nullptr,
			/*OnlyTriggerOnce=*/ false,
			/*bCallDelegate=*/   true);

		Task->EventReceived.AddDynamic(this, &UWeaponAbilityBase::OnGameplayEventDispatched);
		Task->ReadyForActivation();

		EventListenerTasks.Add(Task);
	}
}

void UWeaponAbilityBase::OnGameplayEventDispatched(FGameplayEventData Payload)
{
	// WaitGameplayEvent가 Payload.EventTag를 채워서 보내준다
	const FGameplayTag& EventTag = Payload.EventTag;

	auto Dispatch = [&](TArray<TObjectPtr<UAbilityLogicBase>>& Logics)
	{
		for (UAbilityLogicBase* Logic : Logics)
		{
			if (!Logic) continue;
			for (const FGameplayTag& Tag : Logic->GetSubscribedEventTags())
			{
				if (Tag == EventTag)
				{
					Logic->OnGameplayEvent(EventTag, Payload);
					break;
				}
			}
		}
	};
	Dispatch(LogicList);
	Dispatch(InjectedLogics);
}

// ─────────────────────────────────────────────────────────────────────────────
//  무기 접근 헬퍼
// ─────────────────────────────────────────────────────────────────────────────

AGASCharacterBase* UWeaponAbilityBase::GetGASCharacter() const
{
	return Cast<AGASCharacterBase>(GetAvatarActorFromActorInfo());
}

UWeaponInstance* UWeaponAbilityBase::GetCurrentWeapon() const
{
	const AGASCharacterBase* Char = GetGASCharacter();
	return Char ? Char->GetCurrentWeapon() : nullptr;
}

float UWeaponAbilityBase::GetScaledDamage() const
{
	const UWeaponInstance* Weapon = GetCurrentWeapon();
	if (!Weapon) return 0.f;

	float AttackMult = 1.f;
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UPlayerAttributeSet* AS = ASC->GetSet<UPlayerAttributeSet>())
		{
			AttackMult = AS->GetAttackPower() / 10.f;
		}
	}

	return Weapon->GetEffectiveDamage() * AttackMult;
}

float UWeaponAbilityBase::GetScaledStaggerDamage() const
{
	const UWeaponInstance* Weapon = GetCurrentWeapon();
	return Weapon ? Weapon->GetEffectiveStaggerDamage() : 0.f;
}

EWeaponType UWeaponAbilityBase::GetWeaponType() const
{
	const UWeaponInstance* Weapon = GetCurrentWeapon();
	return Weapon ? Weapon->GetWeaponType() : EWeaponType::None;
}
