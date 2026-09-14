#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Dream/ETriangleManifestationKind.h"
#include "UTriangleAnomalySubsystem.generated.h"

class ATriangleAnomalyActor;

/**
 * Places and tracks the "cuts into the dream" - short-lived non-interactive
 * environmental anomalies the Dream Director requests. Owns nothing visual
 * itself; it spawns ATriangleAnomalyActors, keeps at most a handful alive at
 * once (oldest first), and can clear everything when the dream resets.
 */
UCLASS()
class UTriangleAnomalySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Spawns a manifestation cut of the given kind near the supplied world point. */
	ATriangleAnomalyActor* SpawnAnomaly(ETriangleAnomalyKind Kind, float Duration, const FVector& AroundLocation);

	/** Number of anomalies currently in the world. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Anomaly")
	int32 GetActiveAnomalyCount() const;

	/** Removes every active anomaly (dream reset / wake). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Anomaly")
	void DespawnAll();

	/** Maximum simultaneous intrusions; the oldest is dropped beyond this. */
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Anomaly")
	int32 MaxConcurrent = 2;

private:
	/** Samples an appropriate spawn point near the player for a cut kind. */
	FVector ChooseRenderPoint(const UWorld* World, ETriangleAnomalyKind Kind, const FVector& Around) const;

	/** Safety scanner run every getter: prunes destroyed actors. */
	void PruneGhosts();

	UPROPERTY()
	TArray<TObjectPtr<ATriangleAnomalyActor>> ActiveAnomalies;
};