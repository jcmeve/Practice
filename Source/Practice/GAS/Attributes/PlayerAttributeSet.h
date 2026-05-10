// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

/**
 * Helper macro — generates Get/Set/Init accessors for a GAS attribute.
 * Usage: ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Attribute set for player characters.
 *
 * Stat pipeline order (applied by GAS modifiers in this sequence):
 *   Flat → PercentAdd → PercentMult → Override
 *
 * Damage flow:
 *   Hit applies GE → modifies IncomingDamage (meta)
 *   PostGameplayEffectExecute consumes it → subtracts from Health after Defense mitigation
 *
 * Stagger flow:
 *   Hit applies GE → modifies IncomingStaggerDamage (meta)
 *   PostGameplayEffectExecute consumes it → adds to Stagger gauge
 *   Stagger == MaxStagger → broadcast "stagger" GameplayEvent upstream
 */
UCLASS()
class PRACTICE_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPlayerAttributeSet();

	// ~begin UAttributeSet interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	// ~end UAttributeSet interface

	// ── Vitals ───────────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina)

	/** Stagger gauge (0 → MaxStagger). Reaching max triggers stagger state. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_Stagger)
	FGameplayAttributeData Stagger;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stagger)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vitals", ReplicatedUsing = OnRep_MaxStagger)
	FGameplayAttributeData MaxStagger;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStagger)

	// ── Combat ───────────────────────────────────────────────

	/** Scales outgoing damage: EffectiveDamage = WeaponDamage * (AttackPower / BaseAttackPower) */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackPower)

	/** Flat damage reduction applied before Health is modified */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense)

	// ── Movement ─────────────────────────────────────────────

	/** Max walk speed (cm/s). Applied to CharacterMovementComponent on change. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Movement", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MoveSpeed)

	// ── Ability Modifiers ────────────────────────────────────
	// 어빌리티가 직접 읽는 수치. GE로 수정해 어빌리티 동작을 바꾼다.
	// 어빌리티 자체는 이 값이 어디서 왔는지 알지 못한다.

	/** 차지 공격 최대 유지 시간 보너스 (초). GE로 Additive 수정. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityMod", ReplicatedUsing = OnRep_ChargeTimeBonus)
	FGameplayAttributeData ChargeTimeBonus;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ChargeTimeBonus)

	// ── Meta attributes (server-side only, never replicated) ──

	/** Transient incoming damage before mitigation. Set by damage GEs, consumed in PostGE. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingDamage)

	/** Transient incoming stagger damage. Set by hit GEs, consumed in PostGE. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
	FGameplayAttributeData IncomingStaggerDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingStaggerDamage)

protected:

	// OnRep callbacks — required for GAS client prediction
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Stagger(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxStagger(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_AttackPower(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Defense(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_ChargeTimeBonus(const FGameplayAttributeData& Old);
};
