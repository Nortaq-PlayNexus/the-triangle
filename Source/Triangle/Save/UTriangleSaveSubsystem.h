#pragma once

#include "CoreMinimal.h"
#include "Core/ETriangleDreamEnvironment.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UTriangleSaveSubsystem.generated.h"

class UTriangleSaveGame;

DECLARE_DELEGATE_OneParam(FTriangleSaveAsyncResult, bool /*bSuccess*/);

UENUM(BlueprintType)
enum class ETriangleSaveApplyResult : uint8
{
	Applied,
	Queued,
	Failed
};

/**
 * Owns save/load for the whole game.
 * Saves are written synchronously to the project's local save dir using the
 * standard SaveGame pipeline. Slot names are namespaced so multiple runs and
 * future chapter saves cannot collide.
 *
 * This is a UGameInstanceSubsystem: exactly one exists per game instance and
 * it survives level loads, which is required for dream state that must
 * persist across environment transitions.
 */
UCLASS()
class UTriangleSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static const FString SaveSlotName;

	/** Clears the default slot so a new dream starts without a blank save file. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Save")
	void ResetSave();

	/** Explicitly deletes the default slot. Already-clear slots count as success. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Save")
	bool DeleteSaveData();

	/** Serializes the current dream state and fires OnSaveCompleted with the result. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Save")
	bool SaveGameNow(UTriangleSaveGame* InSnapshot);

	/** Loads the saved dream from disk. Returns the snapshot, or nullptr if none exists. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Save")
	UTriangleSaveGame* LoadGameNow();

	/**
	 * Applies player-owned snapshot state without replaying progression. A
	 * different procedural environment is queued until its fade/swap completes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Save")
	ETriangleSaveApplyResult ApplySnapshot(UTriangleSaveGame* Snapshot);

	/** Whether a saved dream exists. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Save")
	bool HasSaveData() const;

	/** Completion callback for native async/queued callers; fired on every save attempt. */
	FTriangleSaveAsyncResult OnSaveCompleted;

	virtual void Deinitialize() override;

private:
	UFUNCTION()
	void HandleEnvironmentReady(ETriangleDreamEnvironment Environment);

	UFUNCTION()
	void HandleEnvironmentTransitionFailed(ETriangleDreamEnvironment Environment);

	/** Applies the player/world-owned portion after the target environment exists. */
	bool ApplyPlayerState(UTriangleSaveGame* Snapshot);

	/** Builds a fresh snapshot from the world if the caller provided none. */
	UTriangleSaveGame* BuildSnapshot();

	UPROPERTY(Transient)
	TObjectPtr<UTriangleSaveGame> PendingSnapshot;

	UPROPERTY(Transient)
	ETriangleDreamEnvironment PendingEnvironment = ETriangleDreamEnvironment::ChildhoodBedroom;
};