#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UTriangleSaveSubsystem.generated.h"

class UTriangleSaveGame;

DECLARE_DELEGATE_OneParam(FTriangleSaveAsyncResult, bool /*bSuccess*/);

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

	/** Creates a fresh save for a new dream. */
	void ResetSave();

	/** Serializes the current dream state into the default slot. Returns false if serialization failed. */
	bool SaveGameNow(UTriangleSaveGame* InSnapshot);

	/** Loads the saved dream from disk. Returns the snapshot, or nullptr if none exists. */
	UTriangleSaveGame* LoadGameNow();

	/** Whether a saved dream exists. */
	bool HasSaveData() const;

	/** Queued callback for async callers; fired with bSuccess after SaveGameNow. */
	FTriangleSaveAsyncResult OnSaveCompleted;

private:
	/** Builds a fresh snapshot from the world if the caller provided none. */
	UTriangleSaveGame* BuildSnapshot();
};