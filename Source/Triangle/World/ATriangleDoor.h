#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleDoor.generated.h"

class APawn;
class ATriangleDoor;
class UStaticMeshComponent;
class UTriangleInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleDoorTransitionRequested, ATriangleDoor*, Door);

/**
 * A bedroom door that pretends to behave like a door.
 *
 * Before the player is lucid enough, it will not move at all - it is the
 * dream refusing to let the player leave. At High Lucidity the dream relents
 * and the door swings open, revealing only darkness beyond.
 */
UCLASS()
class ATriangleDoor : public AActor
{
	GENERATED_BODY()

public:
	ATriangleDoor();

	UFUNCTION(BlueprintCallable, Category = "Triangle|Door")
	void Open(bool bInstant);

	/** Restores a closed door when loading a snapshot or resetting the room. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Door")
	void Close(bool bInstant);

	UFUNCTION(BlueprintPure, Category = "Triangle|Door")
	bool IsOpen() const { return bIsOpen; }

	/** World-space point just beyond the doorway for environment flow. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Door")
	FVector GetBeyondLocation() const;

	/** Alias that reads naturally at a transition call site. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Door")
	FVector GetTransitionLocation() const { return GetBeyondLocation(); }

	/** Broadcast after a deliberate second interaction on an open door. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Door")
	void RequestTransition();

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Door")
	FTriangleDoorTransitionRequested OnTransitionRequested;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Door")
	float SwingDuration = 1.4f;

	/** Local offset from the closed door to the point beyond it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Door")
	FVector BeyondOffset = FVector(350.f, 0.f, 0.f);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnInteracted(APawn* Interactor);

	void AdvanceSwing(float DeltaTime);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTriangleInteractableComponent> Interactable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Triangle|Door", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;
	bool bSwinging = false;
	float SwingAlpha = 0.f;
	FRotator ClosedRotation;
	FRotator OpenRotation;
};
