#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleRoomBuilder.generated.h"

class ADirectionalLight;
class AExponentialHeightFog;
class APointLight;
class APointLightComponent;
class APlayerStart;
class ATriangleDoor;
class ATriangleNotebook;
class ATriangleTrianglePoint;
class UActorComponent;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Procedurally constructs the childhood bedroom - the slice's first playable
 * dream environment - out of engine basic shapes.
 *
 * Every piece is isolated here behind simple named helpers so that, when real
 * assets arrive later, this class can be replaced by map/asset assembly
 * without touching gameplay systems.
 */
UCLASS()
class ATriangleRoomBuilder : public AActor
{
	GENERATED_BODY()

public:
	ATriangleRoomBuilder();

	/** Builds the room immediately on play. */
	virtual void BeginPlay() override;

	/** World position of the upper-corner point the Dream Director watches. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Environment")
	FVector GetTriangleCornerLocation() const { return TriangleCornerLocation; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float RoomHalfX = 410.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float RoomHalfY = 310.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float RoomHeight = 300.f;

private:
	/** World-altitude the room is assembled at, high above any template terrain. */
	static constexpr float RoomElevation = 5000.f;

	UStaticMesh* GetCubeMesh();
	UStaticMesh* GetPlaneMesh();

	/** Spawns a plain cube actor at the given scale. Returns its visible component. */
	UStaticMeshComponent* AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);

	void BuildShell();
	void BuildWindow();
	void BuildBed();
	void BuildDesk();
	void BuildNotebook();
	void BuildDoor();
	void BuildCornerPoint();
	void BuildLighting();

	FVector TriangleCornerLocation;
};