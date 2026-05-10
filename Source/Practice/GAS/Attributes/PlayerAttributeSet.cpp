// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	// Defaults — override via GE_InitStats GameplayEffect in the editor
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	InitStagger(0.0f);
	InitMaxStagger(100.0f);
	InitAttackPower(10.0f);
	InitDefense(5.0f);
	InitMoveSpeed(600.0f);
	InitIncomingDamage(0.0f);
	InitIncomingStaggerDamage(0.0f);
	InitChargeTimeBonus(0.0f);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth,   COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stagger,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStagger,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Defense,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MoveSpeed,       COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ChargeTimeBonus, COND_None, REPNOTIFY_Always);
	// Meta attributes are NOT replicated
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp current values to their max counterparts
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetStaggerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStagger());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(0.0f, NewValue);
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// ── Damage pipeline ───────────────────────────────────────
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float RawDamage = GetIncomingDamage();
		SetIncomingDamage(0.0f); // consume

		if (RawDamage > 0.0f)
		{
			// Flat Defense mitigation (replace with formula as needed)
			const float Mitigated = FMath::Max(0.0f, RawDamage - GetDefense());
			const float NewHealth  = FMath::Clamp(GetHealth() - Mitigated, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			// TODO: broadcast OnDamageReceived delegate / GameplayEvent here
		}
	}

	// ── Stagger pipeline ──────────────────────────────────────
	if (Data.EvaluatedData.Attribute == GetIncomingStaggerDamageAttribute())
	{
		const float RawStagger = GetIncomingStaggerDamage();
		SetIncomingStaggerDamage(0.0f); // consume

		if (RawStagger > 0.0f)
		{
			const float NewStagger = FMath::Clamp(GetStagger() + RawStagger, 0.0f, GetMaxStagger());
			SetStagger(NewStagger);

			// TODO: if NewStagger >= GetMaxStagger(), send "Event.Stagger" GameplayEvent
		}
	}
}

// ── OnRep callbacks ───────────────────────────────────────────────────────────

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& Old)      { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, Old); }
void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Old)   { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, Old); }
void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, Old); }
void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& Old)  { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, Old); }
void UPlayerAttributeSet::OnRep_Stagger(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stagger, Old); }
void UPlayerAttributeSet::OnRep_MaxStagger(const FGameplayAttributeData& Old)  { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStagger, Old); }
void UPlayerAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& Old) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, AttackPower, Old); }
void UPlayerAttributeSet::OnRep_Defense(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Defense, Old); }
void UPlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& Old)       { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MoveSpeed, Old); }
void UPlayerAttributeSet::OnRep_ChargeTimeBonus(const FGameplayAttributeData& Old) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ChargeTimeBonus, Old); }
