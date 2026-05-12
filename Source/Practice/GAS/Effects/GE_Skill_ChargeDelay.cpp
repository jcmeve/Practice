// Fill out your copyright notice in the Description page of Project Settings.

#include "GE_Skill_ChargeDelay.h"

UGE_Skill_ChargeDelay::UGE_Skill_ChargeDelay()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	// 컴포넌트 설정은 에디터에서 BP로 구성한다.
	// C++ 생성자에서 태그를 설정하면 CDO 생성 타이밍에 태그가 미등록 상태일 수 있음.
}
