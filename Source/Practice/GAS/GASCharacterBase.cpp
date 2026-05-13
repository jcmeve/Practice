// Fill out your copyright notice in the Description page of Project Settings.

#include "GASCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.h"
#include "WeaponBaseData.h"
#include "WeaponInputAbilityMap.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedPlayerInput.h"
#include "InputCoreTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
// Camera
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogGASCharacter);

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────

AGASCharacterBase::AGASCharacterBase()
{
	// ── 카메라 ────────────────────────────────────────────────

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength         = CameraBoomLength;
	CameraBoom->SocketOffset            = CameraSocketOffset;
	CameraBoom->bUsePawnControlRotation = true;   // 컨트롤러 회전 따라감
	CameraBoom->bEnableCameraLag        = true;
	CameraBoom->CameraLagSpeed          = 10.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed  = 15.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 붐이 회전 처리

	// 캐릭터 몸은 컨트롤러 Yaw를 따라가지 않음 (이동 방향으로 회전)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate              = FRotator(0.f, 540.f, 0.f);

	// ── GAS ───────────────────────────────────────────────────

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  Lifecycle
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGASCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeASC();
	GiveStartupAbilities();
	ApplyStartupEffects();
	AddInputMappingContext();
}

void AGASCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeASC();
}

void AGASCharacterBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	AddInputMappingContext();
}

void AGASCharacterBase::InitializeASC()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  Input Mapping Context
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::AddInputMappingContext()
{
	if (!DefaultMappingContext) return;

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  SetupPlayerInputComponent
//
//  AbilityBindings 배열을 순회해 모든 IA에 Started / Completed를 자동 바인딩.
//  Move / Look / Jump 는 별도 핸들러로 바인딩.
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogGASCharacter, Error,
			TEXT("SetupPlayerInputComponent: UEnhancedInputComponent not found. "
			     "프로젝트 세팅 > Input > Default Input Component Class를 확인하세요."));
		return;
	}

	// ── 어빌리티 입력 자동 바인딩 ─────────────────────────────
	// BindAction은 멤버 함수 포인터만 허용하므로
	// 공통 핸들러(OnAbilityInputStarted/Completed)로 바인딩하고
	// 내부에서 GetSourceAction()으로 태그를 역조회한다.
	for (const FInputAbilityBinding& Binding : AbilityBindings)
	{
		if (!Binding.InputAction || !Binding.AbilityInputTag.IsValid())
		{
			continue;
		}

		EIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
			this, &AGASCharacterBase::OnAbilityInputStarted);

		EIC->BindAction(Binding.InputAction, ETriggerEvent::Completed,
			this, &AGASCharacterBase::OnAbilityInputCompleted);
	}

	// ── 스킬 디버그 키 (IA 없이 raw 바인딩) ─────────────────
	PlayerInputComponent->BindKey(
		EKeys::One, IE_Pressed, this,
		&AGASCharacterBase::ToggleChargeDelaySkill);

	// ── 이동 ─────────────────────────────────────────────────
	if (IA_Move)
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this,
			&AGASCharacterBase::HandleMove);
	}

	// ── 카메라 ───────────────────────────────────────────────
	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this,
			&AGASCharacterBase::HandleLook);
	}

	// ── 점프 ─────────────────────────────────────────────────
	if (IA_Jump)
	{
		EIC->BindAction(IA_Jump, ETriggerEvent::Started,   this,
			&AGASCharacterBase::HandleJumpStarted);
		EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this,
			&AGASCharacterBase::HandleJumpCompleted);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  어빌리티 입력 디스패처
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::OnAbilityInputStarted(const FInputActionInstance& Instance)
{
	const UInputAction* FiredAction = Instance.GetSourceAction();
	for (const FInputAbilityBinding& Binding : AbilityBindings)
	{
		if (Binding.InputAction == FiredAction)
		{
			ActivateAbilityByInputTag(Binding.AbilityInputTag, false);
			return;
		}
	}
}

void AGASCharacterBase::OnAbilityInputCompleted(const FInputActionInstance& Instance)
{
	const UInputAction* FiredAction = Instance.GetSourceAction();
	for (const FInputAbilityBinding& Binding : AbilityBindings)
	{
		if (Binding.InputAction == FiredAction)
		{
			ActivateAbilityByInputTag(Binding.AbilityInputTag, true);
			return;
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  이동 / 카메라 핸들러 (기본 구현 — 서브클래스에서 override 가능)
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::HandleMove(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D Axis = Value.Get<FVector2D>();

	const FRotator Rot   = Controller->GetControlRotation();
	const FRotator YawRot(0.f, Rot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, Axis.Y);
	AddMovementInput(RightDir,   Axis.X);
}

void AGASCharacterBase::HandleLook(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero()) return;

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(-Axis.Y);
}

void AGASCharacterBase::HandleJumpStarted()
{
	Jump();
}

void AGASCharacterBase::HandleJumpCompleted()
{
	StopJumping();
}

// ─────────────────────────────────────────────────────────────────────────────
//  IAbilitySystemInterface
// ─────────────────────────────────────────────────────────────────────────────

UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Startup
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::GiveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (!AbilityClass) continue;
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	}
}

void AGASCharacterBase::ApplyStartupEffects()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		if (!EffectClass) continue;

		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(
			EffectClass, 1, Context);
		if (Spec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  Weapon Management
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::EquipWeapon(UWeaponInstance* NewWeapon)
{
	if (!NewWeapon || !NewWeapon->IsValidInstance())
	{
		UE_LOG(LogGASCharacter, Warning, TEXT("EquipWeapon: invalid weapon instance"));
		return;
	}
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogGASCharacter, Warning, TEXT("EquipWeapon: no AbilitySystemComponent"));
		return;
	}

	if (CurrentWeapon) UnequipWeapon();

	CurrentWeapon = NewWeapon;
	const UWeaponBaseData* Base = NewWeapon->BaseData;

	// 장착 GE (스탯 보너스, Infinite)
	if (Base->EquipGameplayEffect)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(
			Base->EquipGameplayEffect, NewWeapon->Level, Context);
		if (Spec.IsValid())
		{
			ActiveEquipEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	// InputAbilityMap에서 어빌리티 부여
	if (Base->InputAbilityMap)
	{
		for (const FInputAbilityEntry& Entry : Base->InputAbilityMap->Entries)
		{
			if (!Entry.AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(Entry.AbilityClass, Entry.AbilityLevel);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag);

			ActiveWeaponAbilityHandles.Add(
				AbilitySystemComponent->GiveAbility(AbilitySpec));
		}
	}

	UE_LOG(LogGASCharacter, Log, TEXT("Equipped: %s (Lv%d)"),
		*Base->WeaponID.ToString(), NewWeapon->Level);
}

void AGASCharacterBase::UnequipWeapon()
{
	if (!AbilitySystemComponent) return;

	for (const FGameplayAbilitySpecHandle& Handle : ActiveWeaponAbilityHandles)
	{
		AbilitySystemComponent->ClearAbility(Handle);
	}
	ActiveWeaponAbilityHandles.Empty();

	if (ActiveEquipEffectHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEquipEffectHandle);
		ActiveEquipEffectHandle = FActiveGameplayEffectHandle();
	}

	CurrentWeapon = nullptr;
}

void AGASCharacterBase::CreateAndEquipWeapon(
	UWeaponBaseData* BaseData,
	int32            Level,
	EWeaponRarity    Rarity)
{
	if (!BaseData)
	{
		UE_LOG(LogGASCharacter, Warning, TEXT("CreateAndEquipWeapon: BaseData is null"));
		return;
	}

	UWeaponInstance* Instance = NewObject<UWeaponInstance>(this);
	Instance->BaseData = BaseData;
	Instance->Level    = Level;
	Instance->Rarity   = Rarity;

	EquipWeapon(Instance);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Input → Ability
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::ActivateAbilityByInputTag(FGameplayTag InputTag, bool bIsRelease)
{
	if (!AbilitySystemComponent) return;

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTag(InputTag)) continue;

		if (bIsRelease)
		{
			// 릴리즈 — InputTag를 GameplayEvent로 발송
			// Logic_WaitInputRelease가 TriggerEventTag=InputTag로 구독 중
			FGameplayEventData Payload;
			Payload.Instigator = this;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				this, InputTag, Payload);
		}
		else if (Spec.IsActive())
		{
			// 활성 중 → InputPressed 전달 (콤보 큐잉 등)
			AbilitySystemComponent->AbilitySpecInputPressed(
				const_cast<FGameplayAbilitySpec&>(Spec));
		}
		else
		{
			// 비활성 → 어빌리티 활성화
			AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
		break;
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  스킬 디버그
// ─────────────────────────────────────────────────────────────────────────────

void AGASCharacterBase::ToggleChargeDelaySkill()
{
	if (!AbilitySystemComponent) return;

	if (ActiveChargeDelayHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveChargeDelayHandle);
		ActiveChargeDelayHandle = FActiveGameplayEffectHandle();
		UE_LOG(LogGASCharacter, Log, TEXT("ChargeDelay 스킬 해제"));
	}
	else
	{
		if (!ChargeDelaySkillEffect)
		{
			UE_LOG(LogGASCharacter, Warning,
				TEXT("ToggleChargeDelaySkill: ChargeDelaySkillEffect가 설정되지 않았습니다. "
				     "BP_GASCharacter의 ChargeDelaySkillEffect에 GE 에셋을 연결하세요."));
			return;
		}

		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(
			ChargeDelaySkillEffect, 1, Context);

		if (Spec.IsValid())
		{
			ActiveChargeDelayHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			UE_LOG(LogGASCharacter, Log, TEXT("ChargeDelay 스킬 획득"));
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  Attribute Accessors
// ─────────────────────────────────────────────────────────────────────────────

float AGASCharacterBase::GetHealth()    const { return AttributeSet ? AttributeSet->GetHealth()    : 0.f; }
float AGASCharacterBase::GetMaxHealth() const { return AttributeSet ? AttributeSet->GetMaxHealth() : 0.f; }
float AGASCharacterBase::GetStamina()   const { return AttributeSet ? AttributeSet->GetStamina()   : 0.f; }
bool  AGASCharacterBase::IsAlive()      const { return GetHealth() > 0.f; }
