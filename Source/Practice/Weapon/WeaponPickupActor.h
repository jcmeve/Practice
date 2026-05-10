// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "WeaponPickupActor.generated.h"

class UWeaponBaseData;
class USphereComponent;
class UStaticMeshComponent;
class AGASCharacterBase;

/**
 * 월드에 배치하는 무기 픽업 액터.
 *
 * 플레이어가 오버랩하면 WeaponInstance를 생성하고 캐릭터에 장착한다.
 * 에디터에서 BaseData / Level / Rarity만 세팅하면 바로 사용 가능.
 *
 * bAutoPickup = true  : 오버랩 즉시 자동 장착
 * bAutoPickup = false : 상호작용 입력(Interact) 시 장착 (추후 연결)
 *
 * 픽업 후 bDestroyOnPickup = true면 액터 소멸.
 */
UCLASS()
class PRACTICE_API AWeaponPickupActor : public AActor
{
	GENERATED_BODY()

public:
	AWeaponPickupActor();

protected:
	virtual void BeginPlay() override;

	// ── 컴포넌트 ─────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// ── 무기 설정 ─────────────────────────────────────────────

	/** 장착할 무기 베이스 데이터 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponBaseData> BaseData;

	/** 아이템 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1))
	int32 Level = 1;

	/** 등급 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponRarity Rarity = EWeaponRarity::Common;

	// ── 동작 설정 ─────────────────────────────────────────────

	/** true: 오버랩 즉시 자동 장착 / false: Interact 입력 필요 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	bool bAutoPickup = true;

	/** 픽업 후 이 액터를 소멸시킬지 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	bool bDestroyOnPickup = true;

public:

	/**
	 * 지정한 캐릭터에게 이 무기를 장착.
	 * 블루프린트나 외부 시스템(인터랙트 등)에서 직접 호출 가능.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void GiveWeaponTo(AGASCharacterBase* Character);

private:

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor*              OtherActor,
		UPrimitiveComponent* OtherComp,
		int32                OtherBodyIndex,
		bool                 bFromSweep,
		const FHitResult&    SweepResult);
};
