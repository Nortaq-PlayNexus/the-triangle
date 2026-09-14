#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "ATriangleDreamDirector.generated.h"

class ATriangleCharacter;
class ATriangleRoomBuilder;
class ATriangleTrianglePoint;
class ULTriangleLucidityComponent;

/**
 * The Dream Director. Controls environmental changes, entity appearance and
 * the Triangle's manifestations. Data-driven and event-based: it does not
 * tick gameplay actors every frame meaninglessly - it only watches the corner
 * while the player exists.
 *
 * In the vertical slice it drives the bedroom's dream-loop:
 *   watch the corner -> lucidity rises -> the point manifests -> recognition
 *   the door stays shut until the player is lucid enough to will it open.
 */
UCLASS()
class ATriangleDreamDirector : public AActor
{
	GENERATED_BODY()

public:
	ATriangleDreamDirector();

	/** Called by the GameMode once, after StartPlay. */
	void InitializeDream();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Forces the corner point to manifest for the given duration. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Dream")
	void TriggerTriangleManifest(float Duration);

	/** Returns the current lucidity level, if the player exists. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Dream")
	ETriangleLucidityLevel GetCurrentLucidityLevel() const;

protected:
	/** Acquire the player pawn + environment references. True when ready. */
	bool AcquireReferences();

	/** Watches the upper corner of the room; grants lucidity while it holds. */
	void WatchCorner(float DeltaSeconds);

	UFUNCTION()
	void HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel);

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0.5", ClampMax = "1"))
	float WatchFovDotThreshold = 0.90f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0", ClampMax = "2000"))
	float WatchRange = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0.5", ClampMax = "30"))
	float ManifestInterval = 5.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0.5", ClampMax = "30"))
	float ManifestDuration = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0", ClampMax = "20"))
	float WatchLucidityPerSecond = 1.5f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ATriangleTrianglePoint> CornerPoint;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ATriangleRoomBuilder> RoomBuilder;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ATriangleCharacter> PlayerActor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULTriangleLucidityComponent> PlayerLucidity;

private:
	float TimeWatching = 0.f;
	float TimeSinceLastManifest = 0.f;
	bool bMaxReachedHandled = false;
	bool bBoundLucidityEvents = false;
	FVector CornerLocation = FVector::ZeroVector;
};