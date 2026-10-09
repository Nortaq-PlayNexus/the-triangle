#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleTrain.generated.h"

class APawn;
class UPointLightComponent;
class UStaticMeshComponent;
class UTriangleInteractableComponent;

/**
 * The wrong train.
 *
 * A BasicShapes train car standing at the platform edge. It looks like a
 * train — but it is the wrong colour, the wrong length, and the windows
 * show nothing or the wrong thing. It never leaves. It is the Triangle's
 * invitation to board the dream.
 */
UCLASS()
class ATriangleTrain : public AActor
{
	GENERATED_BODY()

public:
	ATriangleTrain();

	/** Opens/closes the sliding doors. Open = dark interior visible. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Train")
	void SetDoorsOpen(bool bOpen);

	/** Whether the doors are currently open. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Train")
	bool AreDoorsOpen() const { return bDoorsOpen; }

	/** Toggles the train doors and the interior light together. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Train")
	void ToggleDoors();

	/** The component used by the player's interaction finder. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Train")
	UTriangleInteractableComponent* GetInteractableComponent() const { return Interactable; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnInteracted(APawn* Interactor);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorL;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorR;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> WindowMesh;

	/** Black insert revealed when the platform-side door opens. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> InteriorAperture;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> InteriorLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Train", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTriangleInteractableComponent> Interactable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Triangle|Train", meta = (AllowPrivateAccess = "true"))
	bool bDoorsOpen = false;
};
