// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAbilityBase.h"
#include "GASCharacterBase.h"
#include "WeaponInstance.h"
#include "WeaponBaseData.h"
#include "PlayerAttributeSet.h"
#include "AbilityFragmentRegistry.h"
#include "AbilityFragmentModifierComponent.h"
#include "LogicInjectorComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UWeaponAbilityBase::UWeaponAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Staggered"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  ActivateAbility
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

	ScanAndApplyGEModifiers();

	for (UAbilityLogicBase* Logic : LogicList)
		if (Logic) Logic->OnExecute(this);
	for (UAbilityLogicBase* Logic : InjectedLogics)
		if (Logic) Logic->OnExecute(this);

	SetupEventListeners();
}

// ─────────────────────────────────────────────────────────────────────────────
//  EndAbility
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	for (UAbilityLogicBase* Logic : LogicList)
		if (Logic) Logic->OnAbilityEnd(this, bWasCancelled);
	for (UAbilityLogicBase* Logic : InjectedLogics)
		if (Logic) Logic->OnAbilityEnd(this, bWasCancelled);

	InjectedLogics.Empty();
	RuntimeFragments.Empty();
	EventListenerTasks.Empty();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Fragment — 복제 + Assert
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::BuildRuntimeFragments()
{
	RuntimeFragments.Empty();

	for (UAbilityFragment* Frag : Fragments)
	{
		if (!Frag) continue;

		ensureMsgf(
			!RuntimeFragments.Contains(Frag->FragmentTag),
			TEXT("[%s] BuildRuntimeFragments: 중복 FragmentTag '%s'. "
			     "같은 Fragment를 두 번 추가하지 마세요."),
			*GetName(), *Frag->FragmentTag.ToString());

		UAbilityFragment* RuntimeFrag = DuplicateObject<UAbilityFragment>(Frag, this);
		RuntimeFragments.Add(Frag->FragmentTag, RuntimeFrag);
	}
}

void UWeaponAbilityBase::ValidateFragments() const
{
	auto Check = [this](const TArray<TObjectPtr<UAbilityLogicBase>>& Logics)
	{
		for (const UAbilityLogicBase* Logic : Logics)
		{
			if (!Logic) continue;
			for (const FGameplayTag& Required : Logic->GetRequiredFragmentTags())
			{
				ensureMsgf(
					RuntimeFragments.Contains(Required),
					TEXT("[%s] Logic '%s'이(가) Fragment '%s'를 필요로 하지만 존재하지 않습니다. "
					     "AbilityTags에 해당 Fragment 태그를 추가하거나 "
					     "Logic을 LogicList에서 제거하세요."),
					*GetName(),
					*Logic->GetClass()->GetName(),
					*Required.ToString());
			}
		}
	};

	Check(LogicList);
	Check(InjectedLogics);
}

// ─────────────────────────────────────────────────────────────────────────────
//  GE 스캔 — Fragment 수정 + Logic 주입
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::ScanAndApplyGEModifiers()
{
	InjectedLogics.Empty();

	// ① Fragments → RuntimeFragments 복제
	BuildRuntimeFragments();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayEffectQuery Query;
	TArray<FActiveGameplayEffectHandle> Handles = ASC->GetActiveEffects(Query);

	for (const FActiveGameplayEffectHandle& Handle : Handles)
	{
		const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle);
		if (!ActiveGE || !ActiveGE->Spec.Def) continue;

		const UGameplayEffect* GEDef = ActiveGE->Spec.Def;

		// ② FragmentModifierComponent → Fragment 프로퍼티 수정
		if (const UAbilityFragmentModifierComponent* ModComp = GEDef->FindComponent<UAbilityFragmentModifierComponent>())
		{
			for (const FAbilityFragmentModifier& Mod : ModComp->Modifiers)
			{
				// Fragment 없으면 이 어빌리티엔 해당 없음 — 조용히 skip
				TObjectPtr<UAbilityFragment>* FragPtr = RuntimeFragments.Find(Mod.TargetFragmentTag);
				if (!FragPtr) continue;

				UAbilityFragment* Frag = FragPtr->Get();
				FFloatProperty* FloatProp = CastField<FFloatProperty>(
					Frag->GetClass()->FindPropertyByName(Mod.PropertyName));

				ensureMsgf(FloatProp != nullptr,
					TEXT("[%s] GE Modifier: Fragment '%s'에 프로퍼티 '%s' 없음."),
					*GetName(), *Mod.TargetFragmentTag.ToString(), *Mod.PropertyName.ToString());

				if (!FloatProp) continue;

				float Val = FloatProp->GetPropertyValue_InContainer(Frag);
				switch (Mod.ModOp.GetValue())
				{
				case EGameplayModOp::Additive:       Val += Mod.Magnitude; break;
				case EGameplayModOp::Multiplicitive: Val *= Mod.Magnitude; break;
				case EGameplayModOp::Override:       Val  = Mod.Magnitude; break;
				default: break;
				}
				FloatProp->SetPropertyValue_InContainer(Frag, Val);
			}
		}

		// ③ Logic 주입
		if (const ULogicInjectorComponent* InjComp = GEDef->FindComponent<ULogicInjectorComponent>())
		{
			// RequiredAbilityTags가 비어있으면 무조건 주입
			// 있으면 AbilityTags + 장착 무기 WeaponTags 합산 후 HasAll (AND) 검사
			bool bTagsMatch = InjComp->RequiredAbilityTags.IsEmpty();

			if (!bTagsMatch)
			{
				FGameplayTagContainer CombinedTags = GetAssetTags();

				// 장착 무기의 WeaponTags 추가
				if (const UWeaponInstance* Weapon = GetCurrentWeapon())
				{
					if (Weapon->BaseData)
					{
						CombinedTags.AppendTags(Weapon->BaseData->WeaponTags);
					}
				}

				bTagsMatch = CombinedTags.HasAll(InjComp->RequiredAbilityTags);
			}

			if (bTagsMatch)
			{
				for (const TObjectPtr<UAbilityLogicBase>& Template : InjComp->LogicsToInject)
				{
					if (Template) InjectedLogics.Add(DuplicateObject<UAbilityLogicBase>(Template, this));
				}
			}
		}
	}

	// ④ 검증
	ValidateFragments();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Event Dispatch
// ─────────────────────────────────────────────────────────────────────────────

void UWeaponAbilityBase::SetupEventListeners()
{
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

	for (const FGameplayTag& Tag : UniqueTags)
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, Tag, nullptr, false, true);
		Task->EventReceived.AddDynamic(this, &UWeaponAbilityBase::OnGameplayEventDispatched);
		Task->ReadyForActivation();
		EventListenerTasks.Add(Task);
	}
}

void UWeaponAbilityBase::OnGameplayEventDispatched(FGameplayEventData Payload)
{
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
			AttackMult = AS->GetAttackPower() / 10.f;
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

// ─────────────────────────────────────────────────────────────────────────────
//  PostEditChangeProperty (에디터 전용)
// ─────────────────────────────────────────────────────────────────────────────

#if WITH_EDITOR

void UWeaponAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SyncFragmentsToTags();
}

void UWeaponAbilityBase::SyncFragmentsToTags()
{
	if (!FragmentRegistry) return;

	// 기존 Fragment를 태그로 색인 (디자이너가 수정한 값 유지)
	TMap<FGameplayTag, UAbilityFragment*> Existing;
	for (UAbilityFragment* Frag : Fragments)
	{
		if (!Frag || !Frag->FragmentTag.IsValid()) continue;

		ensureMsgf(
			!Existing.Contains(Frag->FragmentTag),
			TEXT("[%s] SyncFragmentsToTags: 중복 FragmentTag '%s'. "
			     "같은 Fragment를 두 번 추가할 수 없습니다."),
			*GetName(), *Frag->FragmentTag.ToString());

		Existing.Add(Frag->FragmentTag, Frag);
	}

	// AbilityTags → 스키마 → 필요한 Fragment 목록 구성
	TArray<UAbilityFragment*> NewFragments;
	TSet<FGameplayTag> RequiredTags;

	for (const FGameplayTag& AbilityTag : GetAssetTags())
	{
		for (TSubclassOf<UAbilityFragment> FragClass : FragmentRegistry->GetRequiredFragmentClasses(AbilityTag))
		{
			if (!FragClass) continue;
			const UAbilityFragment* CDO = GetDefault<UAbilityFragment>(FragClass);
			if (!CDO || !CDO->FragmentTag.IsValid()) continue;

			if (RequiredTags.Contains(CDO->FragmentTag)) continue;
			RequiredTags.Add(CDO->FragmentTag);

			if (UAbilityFragment** Found = Existing.Find(CDO->FragmentTag))
			{
				NewFragments.Add(*Found); // 기존 값 유지
			}
			else
			{
				NewFragments.Add(NewObject<UAbilityFragment>(this, FragClass)); // 새로 생성
			}
		}
	}

	// 스키마에 없는 Fragment가 수동으로 남아있는지 경고
	for (const auto& Pair : Existing)
	{
		if (!RequiredTags.Contains(Pair.Key))
		{
			ensureMsgf(
				false,
				TEXT("[%s] SyncFragmentsToTags: Fragment '%s'가 현재 AbilityTags 스키마에서 요구하지 않습니다. "
				     "AbilityTags를 확인하거나 이 Fragment를 직접 제거하세요."),
				*GetName(), *Pair.Key.ToString());
		}
	}

	Fragments = NewFragments;
	Modify();
}



#endif
