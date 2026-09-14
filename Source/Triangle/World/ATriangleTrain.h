#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleTrain.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;

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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorL;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorR;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> WindowMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> InteriorLight;

	bool bDoorsOpen = false;
	float DoorAlpha = 0.f;
};
