#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleDoor.generated.h"

class UStaticMeshComponent;
class UTriangleInteractableComponent;

/**
 * A bedroom door that pretends to behave like a door.
 *
 * Before the player is lucid enough, it will not move at all - it is the
 * dream refusing to let the player leave. At medium lucidity the dream
 * relents and the door swings open, revealing only darkness beyond.
 */
UCLASS()
class ATriangleDoor : public AActor
{
	GENERATED_BODY()

public:
	ATriangleDoor();

	void Open(bool bInstant);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Door")
	float SwingDuration = 1.4f;

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

	bool bIsOpen = false;
	bool bSwinging = false;
	float SwingAlpha = 0.f;
	FRotator ClosedRotation;
	FRotator OpenRotation;
};