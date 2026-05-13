// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "EnhancedInputComponent.h"
#include "WeaponInstance.h"
#include "WeaponBaseData.h"
#include "GASCharacterBase.generated.h"

class UAbilitySystemComponent;
class UPlayerAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UWeaponInstance;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogGASCharacter, Log, All);

// ─────────────────────────────────────────────────────────────────────────────
//  FInputAbilityBinding
//
//  IA 하나와 GameplayTag 하나를 연결하는 데이터.
//  AbilityBindings 배열에 추가하면 코드 변경 없이 어빌리티 입력을 추가할 수 있다.
//
//  에디터 세팅 예시:
//    [0] IA_Attack  → Input.PrimaryAttack
//    [1] IA_Skill   → Input.SecondaryAttack
//    [2] IA_Guard   → Input.Guard
//    [3] IA_Dodge   → Input.Dodge
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct PRACTICE_API FInputAbilityBinding
{
	GENERATED_BODY()

	/** 바인딩할 Enhanced Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InputAction;

	/**
	 * 이 액션이 발생할 때 전달할 Gameplay Input Tag.
	 * WeaponInputAbilityMap의 InputTag와 일치해야 한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FGameplayTag AbilityInputTag;
};

// ─────────────────────────────────────────────────────────────────────────────
//  AGASCharacterBase
// ─────────────────────────────────────────────────────────────────────────────

UCLASS(abstract)
class PRACTICE_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase();

	// ~begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~end IAbilitySystemInterface

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

	// ── 카메라 ────────────────────────────────────────────────

	/** 카메라 붐 — 벽 충돌 시 자동으로 카메라를 당겨줌 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** 3인칭 팔로우 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** 기본 붐 길이 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = 0.f, Units = "cm"))
	float CameraBoomLength = 400.f;

	/** 카메라 소켓 오프셋 (붐 끝 기준) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector CameraSocketOffset = FVector(0.f, 75.f, 60.f);

	// ── GAS Components ────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS|Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	// ── Startup Data ──────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Startup")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Startup")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// ── Weapon State ──────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, Category = "GAS|Weapon")
	TObjectPtr<UWeaponInstance> CurrentWeapon;

	TArray<FGameplayAbilitySpecHandle> ActiveWeaponAbilityHandles;
	FActiveGameplayEffectHandle        ActiveEquipEffectHandle;

	// ── Input: 공통 ───────────────────────────────────────────

	/**
	 * 이 캐릭터에 적용할 Input Mapping Context.
	 * Possess 시 LocalPlayer Subsystem에 자동 등록된다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** IMC 우선순위 (높을수록 먼저 처리) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	int32 MappingContextPriority = 0;

	// ── Input: 어빌리티 ──────────────────────────────────────

	/**
	 * IA → InputTag 매핑 배열.
	 * 에디터(BP 클래스 디폴트)에서 항목을 추가하면 된다.
	 * Started  → ActivateAbilityByInputTag(Tag, false)
	 * Completed → ActivateAbilityByInputTag(Tag, true)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	TArray<FInputAbilityBinding> AbilityBindings;

	// ── Input: 이동 / 카메라 ─────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
	TObjectPtr<UInputAction> IA_Jump;

public:

	// ── Weapon Management ─────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "GAS|Weapon")
	void EquipWeapon(UWeaponInstance* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "GAS|Weapon")
	void UnequipWeapon();

	UFUNCTION(BlueprintPure, Category = "GAS|Weapon")
	UWeaponInstance* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "GAS|Weapon")
	void CreateAndEquipWeapon(
		UWeaponBaseData* BaseData,
		int32            Level  = 1,
		EWeaponRarity    Rarity = EWeaponRarity::Common);

	// ── Input ─────────────────────────────────────────────────

	/**
	 * InputTag에 해당하는 어빌리티를 활성화하거나 InputReleased를 전달.
	 * AbilityBindings에서 자동 바인딩되므로 직접 호출할 일은 드물다.
	 */
	UFUNCTION(BlueprintCallable, Category = "GAS|Input")
	void ActivateAbilityByInputTag(FGameplayTag InputTag, bool bIsRelease = false);

	// ── 스킬 디버그 ───────────────────────────────────────────

	/**
	 * 차지 유예 스킬 토글 — 키보드 1 바인딩.
	 * ChargeDelaySkillEffect를 적용/제거한다.
	 * BP에서 ChargeDelaySkillEffect에 GE 에셋을 연결해야 동작한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "GAS|Debug")
	void ToggleChargeDelaySkill();

	/**
	 * 차지 유예 스킬에 적용할 GE 에셋.
	 * BP에서 UAbilityModifierComponent가 설정된 GE를 지정한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS|Debug")
	TSubclassOf<UGameplayEffect> ChargeDelaySkillEffect;

private:
	/** 현재 적용 중인 ChargeDelay 스킬 GE 핸들 (토글용) */
	FActiveGameplayEffectHandle ActiveChargeDelayHandle;

	// ── Attribute Accessors ───────────────────────────────────

	UFUNCTION(BlueprintPure, Category = "GAS|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "GAS|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "GAS|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintPure, Category = "GAS|Attributes")
	bool IsAlive() const;

protected:

	void GiveStartupAbilities();
	void ApplyStartupEffects();
	void InitializeASC();

	/** IMC를 LocalPlayer Subsystem에 등록 */
	void AddInputMappingContext();

	// ── 어빌리티 입력 디스패처 ───────────────────────────────
	// BindAction은 람다를 지원하지 않으므로 멤버 함수로 바인딩한 뒤
	// FInputActionInstance::GetSourceAction()으로 태그를 역조회한다.

	void OnAbilityInputStarted(const FInputActionInstance& Instance);
	void OnAbilityInputCompleted(const FInputActionInstance& Instance);

	// ── 이동/카메라 핸들러 ────────────────────────────────────

	virtual void HandleMove(const FInputActionValue& Value);
	virtual void HandleLook(const FInputActionValue& Value);
	virtual void HandleJumpStarted();
	virtual void HandleJumpCompleted();
};
