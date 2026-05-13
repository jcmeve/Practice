// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilityLogicBase.generated.h"

class UGameplayAbility;
class UWeaponAbilityBase;

/**
 * Abstract logic unit injected into UWeaponAbilityBase.
 *
 * B방식 핵심:
 *   어빌리티 클래스 자체는 로직을 모른다.
 *   무슨 몽타주를 재생할지, 어떻게 판정할지는 전부 이 오브젝트 배열에서 온다.
 *
 * 에디터 사용법:
 *   UWeaponAbilityBase (또는 BP 자식)의 LogicList에
 *   원하는 Logic 서브클래스를 Instanced로 추가하고 파라미터를 세팅.
 *
 *   예) 콤보 공격 어빌리티:
 *     LogicList[0] = ULogic_PlayMontage   (SwordComboMontage, Rate=1.2)
 *     LogicList[1] = ULogic_ComboAdvance  (Sections=["Hit1","Hit2","Hit3"])
 *     LogicList[2] = ULogic_SphereTrace   (Dist=75, Radius=75, DamageGE=...)
 *
 *   예) 방패 가드 어빌리티:
 *     LogicList[0] = ULogic_PlayMontage     (GuardIdleMontage)
 *     LogicList[1] = ULogic_ParryWindow     (Duration=0.35)
 *     LogicList[2] = ULogic_WaitInputRelease(ReleaseTag="Event.Guard.Released")
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType)
class PRACTICE_API UAbilityLogicBase : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * 어빌리티 활성화(ActivateAbility) 시 호출.
	 * AbilityTask 생성, 태그 추가, 타이머 등록 등 "Enter" 동작을 여기서.
	 * ※ GameplayEvent 구독은 여기서 하지 않는다 — GetSubscribedEventTags로 선언하면
	 *   WeaponAbilityBase가 자동으로 라우팅해 준다.
	 */
	virtual void OnExecute(UWeaponAbilityBase* Ability) {}

	/**
	 * 어빌리티 종료(EndAbility) 시 호출.
	 * 태그 제거, 타이머 해제 등 "Exit" 동작을 여기서.
	 */
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) {}

	/**
	 * 이 Logic이 수신하고 싶은 GameplayEvent 태그 목록을 반환.
	 * WeaponAbilityBase가 WaitGameplayEvent 태스크를 관리하고
	 * 이벤트 발생 시 OnGameplayEvent()를 호출해 준다.
	 *
	 * 예) AnimNotifyState(ParryWindow) Begin/End, DoAttackTrace, CheckCombo 등
	 */
	virtual TArray<FGameplayTag> GetSubscribedEventTags() const { return {}; }

	/**
	 * 구독한 태그의 GameplayEvent가 수신됐을 때 호출.
	 * EventTag로 어떤 이벤트인지 구분한다.
	 */
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) {}

	/**
	 * 이 Logic이 동작하기 위해 반드시 필요한 Fragment 태그 목록.
	 * ActivateAbility 시 ValidateFragments()가 이 목록을 검사한다.
	 * 필요한 Fragment가 없으면 ensure 실패로 경고.
	 */
	virtual TArray<FGameplayTag> GetRequiredFragmentTags() const { return {}; }

	/**
	 * 같은 어빌리티의 LogicList에서 타입 T인 첫 번째 Logic을 반환.
	 * Logic 간 협력이 필요할 때 사용 (예: ComboAdvance → PlayMontage 의 JumpToSection 호출).
	 */
	template<typename T>
	T* GetSiblingLogic(UWeaponAbilityBase* Ability) const;
};
