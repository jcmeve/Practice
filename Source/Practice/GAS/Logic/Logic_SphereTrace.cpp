// Fill out your copyright notice in the Description page of Project Settings.

#include "Logic_SphereTrace.h"
#include "WeaponAbilityBase.h"
#include "PracticeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"

void ULogic_SphereTrace::OnExecute(UWeaponAbilityBase* Ability)
{
	OwnerAbility = Ability;
	// 이벤트 구독은 WeaponAbilityBase::SetupEventListeners가 처리
}

void ULogic_SphereTrace::OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled)
{
	OwnerAbility = nullptr;
}

TArray<FGameplayTag> ULogic_SphereTrace::GetSubscribedEventTags() const
{
	return { PracticeGameplayTags::Event_Anim_DoAttackTrace };
}

void ULogic_SphereTrace::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	DoTrace();
}

void ULogic_SphereTrace::DoTrace()
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability) return;

	const AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	const FVector Start = Avatar->GetActorLocation();
	const FVector End   = Start + Avatar->GetActorForwardVector() * TraceDistance;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	Avatar->GetWorld()->SweepMultiByChannel(
		Hits, Start, End,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(Avatar->GetWorld(), End, TraceRadius, 12, FColor::Red, false, 0.5f);
#endif

	TSet<AActor*> HitActors;
	for (const FHitResult& Hit : Hits)
	{
		AActor* Target = Hit.GetActor();
		if (!Target || HitActors.Contains(Target)) continue;
		HitActors.Add(Target);
		ApplyHit(Target);
	}
}

void ULogic_SphereTrace::ApplyHit(AActor* Target)
{
	UWeaponAbilityBase* Ability = OwnerAbility.Get();
	if (!Ability || !Target) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	UAbilitySystemComponent* SourceASC = Ability->GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(Ability->GetAvatarActorFromActorInfo());

	const float EffectiveDamage  = Ability->GetScaledDamage()        * DamageMultiplier;
	const float EffectiveStagger = Ability->GetScaledStaggerDamage() * DamageMultiplier;

	auto ApplyGE = [&](TSubclassOf<UGameplayEffect> GEClass, FGameplayTag DataTag, float Magnitude)
	{
		if (!GEClass) return;
		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(GEClass, Ability->GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	};

	ApplyGE(DamageEffectClass,  PracticeGameplayTags::Data_Damage,       EffectiveDamage);
	ApplyGE(StaggerEffectClass, PracticeGameplayTags::Data_StaggerDamage, EffectiveStagger);
}
