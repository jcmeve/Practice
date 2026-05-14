// Fill out your copyright notice in the Description page of Project Settings.

#include "PracticeGameplayTags.h"

namespace PracticeGameplayTags
{
	// ── Input ────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Input_PrimaryAttack,   "Input.PrimaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_SecondaryAttack, "Input.SecondaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Guard,           "Input.Guard");
	UE_DEFINE_GAMEPLAY_TAG(Input_Dodge,           "Input.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact,        "Input.Interact");

	// ── Stat ─────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Stat_Health,           "Stat.Health");
	UE_DEFINE_GAMEPLAY_TAG(Stat_MaxHealth,        "Stat.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Stamina,          "Stat.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Stat_MaxStamina,       "Stat.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Stagger,          "Stat.Stagger");
	UE_DEFINE_GAMEPLAY_TAG(Stat_MaxStagger,       "Stat.MaxStagger");
	UE_DEFINE_GAMEPLAY_TAG(Stat_AttackPower,      "Stat.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Defense,          "Stat.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Stat_MoveSpeed,        "Stat.MoveSpeed");

	// ── Event ─────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Event_Hit,             "Event.Hit");
	UE_DEFINE_GAMEPLAY_TAG(Event_Parry,           "Event.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Event_Stagger,         "Event.Stagger");
	UE_DEFINE_GAMEPLAY_TAG(Event_Death,           "Event.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_WeaponEquipped,  "Event.WeaponEquipped");

	// ── Ability ───────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack,        "Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Combo,  "Ability.Attack.Combo");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Charge, "Ability.Attack.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Guard,         "Ability.Guard");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Parry,         "Ability.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge,         "Ability.Dodge");

	// ── State ─────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,            "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Staggered,       "State.Staggered");
	UE_DEFINE_GAMEPLAY_TAG(State_Guarding,        "State.Guarding");
	UE_DEFINE_GAMEPLAY_TAG(State_Attacking,       "State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Dodging,         "State.Dodging");
	UE_DEFINE_GAMEPLAY_TAG(State_InParryWindow,   "State.InParryWindow");

	// ── Weapon Input Events ───────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_ChargeReleased,   "Event.Weapon.ChargeReleased");
	UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_ChargeAttackFire, "Event.Weapon.ChargeAttackFire");

	// ── Anim ─────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Event_Anim_DoAttackTrace,     "Event.Anim.DoAttackTrace");
	UE_DEFINE_GAMEPLAY_TAG(Event_Anim_CheckCombo,        "Event.Anim.CheckCombo");
	UE_DEFINE_GAMEPLAY_TAG(Event_Anim_ChargeLevel,       "Event.Anim.ChargeLevel");
	UE_DEFINE_GAMEPLAY_TAG(Event_Anim_ParryWindowBegin,  "Event.Anim.ParryWindowBegin");
	UE_DEFINE_GAMEPLAY_TAG(Event_Anim_ParryWindowEnd,    "Event.Anim.ParryWindowEnd");

	// ── Data ──────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage,              "Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_StaggerDamage,       "Data.StaggerDamage");

	// ── Weapon 종류 태그 ─────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Greatsword, "Weapon.Greatsword");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Sword,      "Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Shield,     "Weapon.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Spear,      "Weapon.Spear");

	// ── Fragment 태그 ────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(AbilityFragment_Charge, "AbilityFragment.Charge");

	// ── Skill ─────────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Skill_ChargeDelay, "Skill.ChargeDelay");

	// ── Enchantment ───────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Enchant_Flame,         "Enchant.Flame");
	UE_DEFINE_GAMEPLAY_TAG(Enchant_Lifesteal,     "Enchant.Lifesteal");
	UE_DEFINE_GAMEPLAY_TAG(Enchant_Freeze,        "Enchant.Freeze");
	UE_DEFINE_GAMEPLAY_TAG(Enchant_Poison,        "Enchant.Poison");

} // namespace PracticeGameplayTags
