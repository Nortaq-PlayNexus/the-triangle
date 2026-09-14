#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleStationBuilder.generated.h"

class ATriangleTrain;
class ATriangleTrianglePoint;

/**
 * Procedurally constructs the underground station — the Phase 2
 * environment. Fluorescent-lit platforms, dark tracks, a wrong train,
 * Platform 12 signage.
 *
 * Built from engine BasicShapes so the slice runs with zero content
 * assets. Isolated here so it can be replaced by asset-based assembly
 * when licensed station art lands.
 */
UCLASS()
class ATriangleStationBuilder : public AActor
{
	GENERATED_BODY()

public:
	ATriangleStationBuilder();

	/** Builds the station immediately on play. */
	virtual void BeginPlay() override;

	/** World position of the Platform 12 marker the Director watches. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Environment")
	FVector GetPlatform12Location() const { return Platform12Location; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float StationHalfX = 700.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float StationHalfY = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float StationHeight = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Environment")
	float PlatformHeight = 120.f;

private:
	UStaticMesh* GetCubeMesh();

	/** Spawns a plain box actor at the given scale. Returns its visible component. */
	UStaticMeshComponent* AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);

	/** The raised platforms on either side of the tracks. */
	void BuildPlatforms();

	/** Dark track bed between the platforms. */
	void BuildTracks();

	/** Side walls, end wall and ceiling of the station tunnel. */
	void BuildShell();

	/** Fluorescent ceiling strips — cold, buzzy, wrong. */
	void BuildFluorescentLights();

	/** Waiting benches on both platforms. */
	void BuildBenches();

	/** The Platform 12 sign / marker. */
	void BuildPlatform12Marker();

	/** Ventilation ducts running along the ceiling. */
	void BuildVentilation();

	/** The wrong train standing at the platform edge. */
	void BuildTrain();

	/** Platform-edge safety lights (thin amber strips). */
	void BuildSafetyLights();

	/** The Triangle point lives here, on the far platform corner. */
	void BuildCornerPoint();

	void BuildLighting();

	FVector Platform12Location = FVector::ZeroVector;
};
