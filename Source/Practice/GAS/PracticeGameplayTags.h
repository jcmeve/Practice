// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * Centralized native GameplayTag declarations for the Practice project.
 *
 * Usage (C++):
 *   #include "PracticeGameplayTags.h"
 *   FGameplayTag Tag = PracticeGameplayTags::Input_PrimaryAttack;
 *
 * Usage (Blueprint / DataAsset):
 *   Type the tag string directly (e.g. "Input.PrimaryAttack").
 *   Tags are registered on module startup so they appear in the tag picker.
 *
 * Naming convention: Category_SubCategory_Name (dots in the tag string, underscores in C++)
 */
namespace PracticeGameplayTags
{
	// ── Input ────────────────────────────────────────────────────────────────
	// Bound to EnhancedInput actions; matched in WeaponInputAbilityMap entries.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_PrimaryAttack);    // "Input.PrimaryAttack"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_SecondaryAttack);  // "Input.SecondaryAttack"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Guard);            // "Input.Guard"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Dodge);            // "Input.Dodge"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact);         // "Input.Interact"

	// ── Stat ─────────────────────────────────────────────────────────────────
	// Used in FWeaponStatBonus.StatTag to identify the target PlayerAttributeSet attribute.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Health);            // "Stat.Health"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_MaxHealth);         // "Stat.MaxHealth"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Stamina);           // "Stat.Stamina"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_MaxStamina);        // "Stat.MaxStamina"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Stagger);           // "Stat.Stagger"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_MaxStagger);        // "Stat.MaxStagger"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_AttackPower);       // "Stat.AttackPower"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Defense);           // "Stat.Defense"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_MoveSpeed);         // "Stat.MoveSpeed"

	// ── Weapon Input Events ───────────────────────────────────────────────────
	// Logic_WaitInputRelease / Logic_ChargeAttack 간 통신용

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Weapon_ChargeReleased);   // "Event.Weapon.ChargeReleased"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Weapon_ChargeAttackFire); // "Event.Weapon.ChargeAttackFire"

	// ── Event ─────────────────────────────────────────────────────────────────
	// Sent via UAbilitySystemBlueprintLibrary::SendGameplayEventToActor.
	// Abilities listen for these using WaitGameplayEvent AbilityTask.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit);              // "Event.Hit"          — weapon landed a hit
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Parry);            // "Event.Parry"        — parry window hit
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Stagger);          // "Event.Stagger"      — stagger gauge full
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);            // "Event.Death"        — health reached 0
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_WeaponEquipped);   // "Event.WeaponEquipped"

	// ── Ability ───────────────────────────────────────────────────────────────
	// UGameplayAbility::AbilityTags에 설정.
	// AbilityDataRegistry::AbilitySchema 키로도 사용된다.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);         // "Ability.Attack"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Combo);   // "Ability.Attack.Combo"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Charge);  // "Ability.Attack.Charge"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Guard);          // "Ability.Guard"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Parry);          // "Ability.Parry"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);          // "Ability.Dodge"

	// ── State ─────────────────────────────────────────────────────────────────
	// Applied as LooseGameplayTags or via GE GrantedTags.
	// Used in ActivationBlockedTags / ActivationRequiredTags on abilities.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);             // "State.Dead"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Staggered);        // "State.Staggered"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Guarding);         // "State.Guarding"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking);        // "State.Attacking"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dodging);          // "State.Dodging"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_InParryWindow);    // "State.InParryWindow"

	// ── Anim (AnimNotify → GameplayEvent) ────────────────────────────────────
	// Fired by AnimNotify classes via SendGameplayEventToActor.
	// Abilities listen with UAbilityTask_WaitGameplayEvent.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Anim_DoAttackTrace);    // "Event.Anim.DoAttackTrace"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Anim_CheckCombo);      // "Event.Anim.CheckCombo"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Anim_ChargeLevel);     // "Event.Anim.ChargeLevel"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Anim_ParryWindowBegin);// "Event.Anim.ParryWindowBegin" — ANS NotifyBegin
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Anim_ParryWindowEnd);  // "Event.Anim.ParryWindowEnd"   — ANS NotifyEnd

	// ── Data (SetByCaller magnitudes) ─────────────────────────────────────────
	// Used in FGameplayEffectSpec::SetSetByCallerMagnitude when applying damage GEs.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);              // "Data.Damage"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_StaggerDamage);       // "Data.StaggerDamage"

	// ── Weapon 종류 태그 ─────────────────────────────────────────────────────
	// UWeaponBaseData::WeaponTags에 세팅. 활성화 시 AbilityTags와 합쳐져 주입 조건 검사에 사용.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Greatsword); // "Weapon.Greatsword"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Sword);      // "Weapon.Sword"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Shield);     // "Weapon.Shield"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Spear);      // "Weapon.Spear"

	// ── Fragment 태그 ────────────────────────────────────────────────────────
	// UAbilityFragment 서브클래스가 FragmentTag로 사용.
	// AbilityDataRegistry 스키마의 키이자 RuntimeFragments의 키.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fragment_Charge); // "Fragment.Charge"

	// ── Skill ─────────────────────────────────────────────────────────────────
	// 스킬 획득 시 ASC에 추가되는 태그. Logic에서 읽어 동작을 변경한다.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Skill_ChargeDelay);  // "Skill.ChargeDelay"

	// ── Enchantment ───────────────────────────────────────────────────────────
	// Used in FEnchantmentEntry.EnchantmentTag.

	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enchant_Flame);          // "Enchant.Flame"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enchant_Lifesteal);      // "Enchant.Lifesteal"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enchant_Freeze);         // "Enchant.Freeze"
	PRACTICE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enchant_Poison);         // "Enchant.Poison"

} // namespace PracticeGameplayTags
