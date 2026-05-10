// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "GameplayEffect.h"
#include "Logic_SphereTrace.generated.h"

class UAbilityTask_WaitGameplayEvent;

/**
 * 전투 판정 레이어 — 구체 트레이스 히트 감지.
 *
 * Event.Anim.DoAttackTrace 이벤트를 구독하고,
 * 이벤트 발생 시 캐릭터 전방으로 구체 트레이스를 수행해 피격 대상에게 GE를 적용한다.
 *
 * 애니메이션 레이어(ULogic_PlayMontage)와 완전히 독립적:
 *   어떤 몽타주를 쓰든, 해당 몽타주가 Event.Anim.DoAttackTrace를 발생시키면 이 로직이 처리한다.
 *
 * 데미지 배율이 필요한 경우(차지 공격 등) DamageMultiplier를 조정한다.
 */
UCLASS(DisplayName = "Logic: Sphere Trace Attack")
class PRACTICE_API ULogic_SphereTrace : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/** 트레이스 전방 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace", meta = (ClampMin = 0.f, Units = "cm"))
	float TraceDistance = 75.0f;

	/** 구체 반지름 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace", meta = (ClampMin = 0.f, Units = "cm"))
	float TraceRadius = 75.0f;

	/** 충돌 채널 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	/** IncomingDamage 메타 어트리뷰트에 적용할 Instant GE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** IncomingStaggerDamage 메타 어트리뷰트에 적용할 Instant GE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> StaggerEffectClass;

	/**
	 * 데미지 배율 — 차지 공격처럼 상황에 따라 배율이 달라질 때 사용.
	 * 런타임에 SetDamageMultiplier()로 변경 가능.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = 0.f))
	float DamageMultiplier = 1.0f;

	// ── UAbilityLogicBase interface ───────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	virtual TArray<FGameplayTag> GetSubscribedEventTags() const override;
	virtual void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload) override;

	// ── Public API ────────────────────────────────────────────

	/** 런타임 데미지 배율 변경 (차지 레벨 등에서 호출) */
	void SetDamageMultiplier(float NewMultiplier) { DamageMultiplier = NewMultiplier; }

private:

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	void DoTrace();
	void ApplyHit(AActor* Target);
};
