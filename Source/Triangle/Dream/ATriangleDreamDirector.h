#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "ATriangleDreamDirector.generated.h"

class ATriangleCharacter;
class ATriangleTrianglePoint;
class ATriangleAnomalyActor;
class ULTriangleLucidityComponent;
class ULTriangleRecognitionComponent;
class UTriangleAnomalySubsystem;
class UTriangleAudioSubsystem;
class UTriangleMotifSubsystem;
class UTriangleFalseAwakeningManager;
class UTriangleManifestationDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleDirectorPresenceChanged, float, Presence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriangleDirectorVoid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleDirectorManifest, FName, ManifestId);

/**
 * The Dream Director. Controls environmental changes, entity appearance and
 * The Triangle's presence in the dream - and, after the design update, the
 * Recognition-led loop that lets the player sense it before they see it.
 *
 * The Director owns a 0..1 presence ramp: presence rises while a
 * manifestation is active and decays when nothing is present. The hum follows
 * presence; anomalies (gaps, corners pushed apart, distant points) are its
 * visible bodies; the recurring motifs (twelve, the train) arrive on their own
 * schedule; and a completed false wake asks the Director to withdraw everything
 * and start the presence over.
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

	/** Returns the current recognition level. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Dream")
	ETriangleRecognitionLevel GetCurrentRecognitionLevel() const;

	/** Current Triangle presence (0..1). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Dream")
	float GetTrianglePresence() const { return TrianglePresence; }

	/** Withdraws all current presence and manifestations (wake / reset). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Dream")
	void WithdrawPresence();

	/** Clears environment-bound references after a room/station transition. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Dream")
	void HandleEnvironmentChanged();

	/** Invalidates active manifestation/audio state before applying a loaded snapshot. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Dream")
	void ResetForSaveLoad();

	// --- Events visible/extensible in Blueprint -----------------------------
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorPresenceChanged OnPresenceStart;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorPresenceChanged OnPresenceIncrease;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorVoid OnPresenceDisappear;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorManifest OnTriangleManifestation;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorVoid OnTriangleRecognition;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorVoid OnTriangleHumStart;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorVoid OnTriangleHumStop;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorVoid OnDreamBleed;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Dream|Events")
	FTriangleDirectorManifest OnFalseAwakening;

protected:
	/** Acquire the player pawn + environment references. True when ready. */
	bool AcquireReferences();

	/** Watches the upper corner of the room; grants lucidity while it holds. */
	void WatchCorner(float DeltaSeconds);

	/** Runs the presence ramp + hum following + sensing. */
	void UpdatePresence(float DeltaSeconds);

	/** Picks and executes a manifestation from the pool. */
	void ExecuteManifestation(bool bSenseOnly = false);

	/** Rounds out a seen manifestation with recognition gain + events. */
	void ResolveManifestation(const UTriangleManifestationDefinition* Def, float Duration, int32 ManifestationSerial);

	/** True when the current manifestation was actually in the player's view. */
	bool WasManifestationWitnessed(const UTriangleManifestationDefinition* Def) const;

	/** Master recognition can guess at the presence before any visible form. */
	void TryMasterSense(float DeltaSeconds);

	/** Applies a short motif/reveal hum independently of the presence ramp. */
	void RequestTransientHum(float Target, float Duration);

	/** Maps a recognition milestone to subtitle/hum beats. */
	void OnRecognitionMilestone(ETriangleRecognitionLevel NewLevel);

	UFUNCTION()
	void HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel);

	UFUNCTION()
	void HandleRecognitionLevelChanged(ETriangleRecognitionLevel NewLevel);

	UFUNCTION()
	void HandleMotif(FName MotifId);

	UFUNCTION()
	void HandleWakeCycle(int32 LayerIndex);

	// --- Tunables ------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0.5", ClampMax = "1"))
	float WatchFovDotThreshold = 0.90f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0", ClampMax = "2000"))
	float WatchRange = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0.5", ClampMax = "30"))
	float ManifestInterval = 5.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream", meta = (ClampMin = "0", ClampMax = "20"))
	float WatchLucidityPerSecond = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream|Presence", meta = (ClampMin = "0.05", ClampMax = "2"))
	float PresenceRampRate = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream|Presence", meta = (ClampMin = "0.01", ClampMax = "1"))
	float PresenceDecayRate = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream|Presence")
	float HumFadeSeconds = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream|Presence", meta = (ClampMin = "3", ClampMax = "60"))
	float MasterSenseInterval = 9.f;

	/** The catalogue of bodies The Triangle can take. Populated in the constructor. */
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Dream|Manifestations")
	TArray<TObjectPtr<UTriangleManifestationDefinition>> ManifestationPool;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ATriangleTrianglePoint> CornerPoint;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ATriangleCharacter> PlayerActor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULTriangleLucidityComponent> PlayerLucidity;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULTriangleRecognitionComponent> PlayerRecognition;

private:
	float TimeWatching = 0.f;
	float TimeSinceLastManifest = 0.f;
	float TimeSinceMasterSense = 0.f;
	float TrianglePresence = 0.f;
	float LastAnnouncedPresence = 0.f;
	float CurrentHumTarget = 0.f;
	float TransientHumTarget = 0.f;
	float TransientHumRemaining = 0.f;
	int32 ActiveManifestationSerial = 0;
	bool bHumActive = false;
	bool bPresenceActive = false;
	bool bMaxReachedHandled = false;
	bool bBoundEvents = false;
	bool bManifestationActive = false;
	bool bHumEnabledForCurrentManifestation = false;
	bool bMaximumPointReapplied = false;
	bool bSchedulingStarted = false;
	FVector CornerLocation = FVector::ZeroVector;

	TMap<FName, float> ManifestCooldowns;
	FTimerHandle ManifestationTimerHandle;
	FTimerHandle TrainDoorResetTimerHandle;
	TWeakObjectPtr<ATriangleAnomalyActor> ActiveAnomaly;
	UPROPERTY(Transient)
	TObjectPtr<UTriangleManifestationDefinition> ActiveManifestationDefinition;
	bool bCurrentAnomalyWasWitnessed = false;

	TObjectPtr<UTriangleAnomalySubsystem> AnomalySys;
	TObjectPtr<UTriangleAudioSubsystem> AudioSys;
	TObjectPtr<UTriangleMotifSubsystem> MotifSys;
	TObjectPtr<UTriangleFalseAwakeningManager> WakeSys;
};