// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponPickupActor.h"
#include "GASCharacterBase.h"
#include "WeaponBaseData.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AWeaponPickupActor::AWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 오버랩 감지 구체
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetSphereRadius(80.f);
	OverlapSphere->SetCollisionProfileName(TEXT("Trigger"));
	SetRootComponent(OverlapSphere);

	// 비주얼 메시 (에디터에서 무기 모양 세팅 가능)
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(OverlapSphere);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPickup)
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(
			this, &AWeaponPickupActor::OnOverlapBegin);
	}
}

void AWeaponPickupActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor*              OtherActor,
	UPrimitiveComponent* OtherComp,
	int32                OtherBodyIndex,
	bool                 bFromSweep,
	const FHitResult&    SweepResult)
{
	if (AGASCharacterBase* Character = Cast<AGASCharacterBase>(OtherActor))
	{
		GiveWeaponTo(Character);
	}
}

void AWeaponPickupActor::GiveWeaponTo(AGASCharacterBase* Character)
{
	if (!Character || !BaseData)
	{
		return;
	}

	Character->CreateAndEquipWeapon(BaseData, Level, Rarity);

	if (bDestroyOnPickup)
	{
		Destroy();
	}
}
