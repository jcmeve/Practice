// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityLogicBase.h"
#include "Logic_PlayMontage.generated.h"

class UAbilityTask_PlayMontageAndWait;

/**
 * 애니메이션 레이어.
 * 지정한 몽타주를 재생하고, 완료/중단 시 어빌리티를 종료시킨다.
 *
 * 콤보처럼 섹션을 중간에 바꿔야 할 경우 JumpToSection()을 호출한다.
 * (ULogic_ComboAdvance가 이 메서드를 통해 다음 섹션으로 전환)
 */
UCLASS(DisplayName = "Logic: Play Montage")
class PRACTICE_API ULogic_PlayMontage : public UAbilityLogicBase
{
	GENERATED_BODY()

public:

	/** 재생할 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> Montage;

	/** 재생 시작 섹션 이름 (없으면 처음부터) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	FName StartSection;

	/** 재생 배속 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (ClampMin = 0.1f))
	float PlayRate = 1.0f;

	/**
	 * true: 어빌리티가 다른 이유로 끝날 때 몽타주도 같이 멈춤
	 * false: 몽타주가 끝날 때까지 계속 재생
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	bool bStopWhenAbilityEnds = true;

	/**
	 * true: 몽타주 완료/중단 시 어빌리티를 종료시킴
	 * false: 몽타주가 끝나도 어빌리티 계속 유지 (가드 등 홀드 어빌리티)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	bool bEndAbilityOnMontageEnd = true;

	// ── UAbilityLogicBase interface ───────────────────────────

	virtual void OnExecute(UWeaponAbilityBase* Ability) override;
	virtual void OnAbilityEnd(UWeaponAbilityBase* Ability, bool bWasCancelled) override;

	// ── Public API for sibling logics ─────────────────────────

	/** 현재 재생 중인 몽타주의 섹션을 변경 (콤보 어드밴스 등에서 호출) */
	void JumpToSection(FName SectionName);

	/**
	 * 몽타주 태스크 델리게이트 연결을 끊고 오너 참조를 해제.
	 * 차지 공격처럼 릴리즈 시 다른 Logic이 새 몽타주를 재생하면
	 * 루프 몽타주가 중단(Interrupted)될 때 어빌리티가 종료되지 않도록 호출.
	 */
	void Detach();

private:

	TWeakObjectPtr<UWeaponAbilityBase> OwnerAbility;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveTask;

	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
	UFUNCTION() void OnMontageBlendOut();
};
