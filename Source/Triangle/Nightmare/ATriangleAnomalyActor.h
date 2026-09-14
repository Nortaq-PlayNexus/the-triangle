#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dream/ETriangleManifestationKind.h"
#include "ATriangleAnomalyActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

/**
 * A single "cut into the dream" - a temporary environmental anomaly that
 * reads as The Triangle without being a body.
 *
 * Four forms, each buildable from engine basic shapes at runtime:
 *  - TriangularGap     : a razor-thin dark triangular blade cut into a wall.
 *  - CornerSplit       : a vertical seam that sets a corner slightly apart.
 *  - DistantBlackPoint : a tiny black point hanging far away.
 *  - TriangularDoorway : an impossible dark triangle of doorway arranged on a wall.
 *
 * The anomaly fades in and out by scale (no material budget), is completely
 * non-interactive and never collides - it is an intrusion the player looks
 * at, not one they inspect.
 */
UCLASS()
class ATriangleAnomalyActor : public AActor
{
	GENERATED_BODY()

public:
	ATriangleAnomalyActor();

	/** Places the anomaly in the world and starts its lifetime. */
	void Configure(ETriangleAnomalyKind InKind, float InDuration, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	/** Number of forms active right now (shared world counter). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Anomaly")
	bool IsActive() const { return bActive; }

	/** Kind this instance is currently showing. */
	ETriangleAnomalyKind GetAnomalyKind() const { return ActiveKind; }

	/** Instantly removes the anomaly (used when the dream resets). */
	void ShutDownNow();

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UStaticMesh* LoadMesh(const TCHAR* Path);
	void BuildForKind(ETriangleAnomalyKind Kind);
	void ApplyAnimatedScale(float ScaleFactor);
	void Finish();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PrimaryMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AuxMeshA;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AuxMeshB;

	ETriangleAnomalyKind ActiveKind = ETriangleAnomalyKind::None;
	float FadeInTime = 0.9f;
	float FadeOutTime = 1.1f;
	float Lifespan = 5.f;
	float AgeSeconds = 0.f;
	float BuildScale = 1.f;
	bool bActive = false;

	/** The look the dream uses for all intrusions: a near-black wrongness. */
	static FLinearColor IntrusionColor;
};