// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ParryWindow.generated.h"

/**
 * 패링 윈도우 AnimNotifyState.
 *
 * 몽타주에서 이 구간이 시작/끝날 때 GameplayEvent를 발송한다.
 *   NotifyBegin → Event.Anim.ParryWindowBegin → Logic_ParryWindow::OpenWindow()
 *   NotifyEnd   → Event.Anim.ParryWindowEnd   → Logic_ParryWindow::CloseWindow()
 *
 * 에디터에서 패링 윈도우가 활성화될 구간에 드래그로 배치.
 * 차지 공격 몽타주, 가드 몽타주 모두에 동일하게 사용.
 */
UCLASS(DisplayName = "Parry Window")
class PRACTICE_API UAnimNotifyState_ParryWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	/** NotifyBegin: State.InParryWindow 활성화 이벤트 발송 */
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;

	/** NotifyEnd: State.InParryWindow 비활성화 이벤트 발송 */
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
