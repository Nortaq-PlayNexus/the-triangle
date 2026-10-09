#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ATriangleCharacter.generated.h"

class UCameraComponent;
class UEnhancedInputComponent;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class UTriangleInteractableComponent;
class ULTriangleLucidityComponent;
class ULTriangleRecognitionComponent;

/**
 * First-person dream walker.
 *
 * Holds the interaction finder (a short forward sphere-overlap that locates
 * UTriangleInteractableComponents), the Lucidity component, and input wired
 * through Enhanced Input - all configured in C++ so no content assets are
 * required to run the vertical slice.
 */
UCLASS()
class ATriangleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATriangleCharacter();

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	virtual void UnPossessed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** The currently focused interactable (if any). */
	UPROPERTY(BlueprintReadOnly, Category = "Triangle|Interaction")
	TObjectPtr<UTriangleInteractableComponent> FocusedInteractable;

	/** Range used when an interactable does not specify its own. (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction", meta = (ClampMin = "40", ClampMax = "600"))
	float DefaultInteractionRange = 240.f;

	/** Time between interaction scans. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction", meta = (ClampMin = "0.05", ClampMax = "1"))
	float InteractionScanInterval = 0.08f;

	/** The view camera (used by the Dream Director for line-of-sight logic). */
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Lucidity")
	TObjectPtr<ULTriangleLucidityComponent> Lucidity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Recognition")
	TObjectPtr<ULTriangleRecognitionComponent> Recognition;

	/** View-agnostic way for systems to reach the hidden Recognition stat. */
	ULTriangleRecognitionComponent* GetRecognitionComponent() const { return Recognition; }

	// --- Interaction scanning --------------------------------------------
	void ScanForInteractable();
	void SetFocusedInteractable(UTriangleInteractableComponent* NewFocus);
	void UpdateInteractionPrompt();

	// --- Input actions ----------------------------------------------------
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void LookHorizontal(const FInputActionValue& Value);
	void LookVertical(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	void PressInteract(const FInputActionValue& Value);
	void PressWake(const FInputActionValue& Value);
	void PressPause(const FInputActionValue& Value);
	void QuickSave(const FInputActionValue& Value);
	void QuickLoad(const FInputActionValue& Value);

private:
	void EnsureInputBindings();
	void StartInteractionScan();
	bool TryAddInputContext();
	void RetryAddInputContext();
	void RemoveInputContext();
	void PushHudSubtitle(const FText& Text, float Duration);

	// Actions and mapping context are created at runtime in C++. Movement and
	// look are split into Axis1D actions so keyboard and gamepad axes have
	// explicit, testable signs instead of relying on implicit 2D key merging.
	UPROPERTY()
	TObjectPtr<UInputAction> ActionMoveForward;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionMoveRight;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionLookHorizontal;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionLookVertical;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionJump;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionInteract;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionWake;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionSave;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionLoad;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionPause;
	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY()
	TObjectPtr<UInputComponent> BoundInputComponent;

	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> ActiveInputSubsystem;
	FTimerHandle InputContextRetryTimerHandle;
	FTimerHandle ScanTimerHandle;
	bool bInputContextAddedByCharacter = false;
	bool bInputMappingsInitialized = false;
	float EyesHeight = 88.f;
};
