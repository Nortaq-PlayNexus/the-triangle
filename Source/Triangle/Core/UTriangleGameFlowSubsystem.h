#pragma once

#include "CoreMinimal.h"
#include "Core/ETriangleDreamEnvironment.h"
#include "Subsystems/WorldSubsystem.h"
#include "UTriangleGameFlowSubsystem.generated.h"

class AActor;
class APlayerController;
class ATriangleDoor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleDreamEnvironmentEvent, ETriangleDreamEnvironment, Environment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleDreamEnvironmentFailure, ETriangleDreamEnvironment, Environment);

/**
 * Owns the active procedural environment for the current dream slice.
 *
 * The bedroom and station are built at runtime rather than loaded as separate
 * maps. Keeping that ownership in one world subsystem gives the door, saves,
 * director, and debug tooling one authoritative place to request transitions.
 */
UCLASS()
class UTriangleGameFlowSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Starts a fade-and-swap transition. Returns false while another one is active. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Flow")
	bool TransitionTo(ETriangleDreamEnvironment Environment);

	/** Ensures the requested environment exists, swapping immediately if startup has not begun. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Flow")
	bool EnsureEnvironment(ETriangleDreamEnvironment Environment);

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	ETriangleDreamEnvironment GetCurrentEnvironment() const { return CurrentEnvironment; }

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	FName GetCurrentEnvironmentId() const;

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	bool IsTransitioning() const { return bTransitioning; }

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	bool HasActiveEnvironment() const { return IsValid(ActiveEnvironmentBuilder); }

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	bool GetLastBedroomDoorOpen() const { return bLastBedroomDoorOpen; }

	UFUNCTION(BlueprintCallable, Category = "Triangle|Flow")
	void SetLastBedroomDoorOpen(bool bOpen) { bLastBedroomDoorOpen = bOpen; }

	/** Restores an environment id from a save. Unknown ids are rejected safely. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Flow")
	bool RestoreEnvironmentId(FName EnvironmentId);

	UFUNCTION(BlueprintPure, Category = "Triangle|Flow")
	bool IsKnownEnvironmentId(FName EnvironmentId) const;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Flow")
	FTriangleDreamEnvironmentEvent OnEnvironmentTransitionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Flow")
	FTriangleDreamEnvironmentEvent OnEnvironmentReady;

	/** Fired when a requested environment cannot be assembled. */
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Flow")
	FTriangleDreamEnvironmentFailure OnEnvironmentTransitionFailed;

protected:
	virtual void Deinitialize() override;

private:
	bool SwitchEnvironmentImmediate(ETriangleDreamEnvironment Environment);
	void FinishTimedTransition();
	void SetTransitionInputLocked(bool bLocked);
	void UnlockTransitionInput();

	UFUNCTION()
	void HandleDoorTransitionRequested(ATriangleDoor* Door);

	UPROPERTY(Transient)
	TObjectPtr<AActor> ActiveEnvironmentBuilder;

	UPROPERTY(Transient)
	ETriangleDreamEnvironment CurrentEnvironment = ETriangleDreamEnvironment::ChildhoodBedroom;

	UPROPERTY(Transient)
	ETriangleDreamEnvironment PendingEnvironment = ETriangleDreamEnvironment::ChildhoodBedroom;

	FTimerHandle TransitionTimerHandle;
	FTimerHandle TransitionInputUnlockHandle;
	bool bTransitioning = false;
	bool bInputLockHeld = false;
	bool bLastBedroomDoorOpen = false;
	TWeakObjectPtr<APlayerController> InputLockController;
};
