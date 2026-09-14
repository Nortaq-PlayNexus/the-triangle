#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ATriangleCharacter.generated.h"

class UCameraComponent;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
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
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	/** The currently focused interactable (if any). */
	UPROPERTY(BlueprintReadOnly, Category = "Triangle|Interaction")
	TObjectPtr<UTriangleInteractableComponent> FocusedInteractable;

	/** Range used when an interactable does not specify its own. (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction", meta = (ClampMin = "40", ClampMax = "600"))
	float DefaultInteractionRange = 240.f;

	/** Time between interaction scans. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction", meta = (ClampMin = "0.05", ClampMax = "1"))
	float InteractionScanInterval = 0.08f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

public:
	/** The view camera (used by the Dream Director for line-of-sight logic). */
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:

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
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	void PressInteract(const FInputActionValue& Value);
	void PressWake(const FInputActionValue& Value);
	void QuickSave(const FInputActionValue& Value);
	void QuickLoad(const FInputActionValue& Value);

private:
	void EnsureInputBindings();

	// Actions and mapping context are created at runtime in C++.
	UPROPERTY()
	TObjectPtr<UInputAction> ActionMove;
	UPROPERTY()
	TObjectPtr<UInputAction> ActionLook;
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
	TObjectPtr<UInputMappingContext> InputContext;

	FTimerHandle ScanTimerHandle;
	float EyesHeight = 88.f;
};