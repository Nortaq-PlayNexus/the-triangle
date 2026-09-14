#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UTriangleSaveGame.generated.h"

/**
 * Serializable dream-state snapshot for a save slot.
 * Grows with the systems that need persistence (memories, notebook entries,
 * dream flags) - driven by the FTriangleDreamMemory data added later.
 */
UCLASS()
class UTriangleSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UTriangleSaveGame();

	/** Player transform inside the current dream level. */
	UPROPERTY()
	FTransform PlayerTransform;

	/** Current hidden Lucidity value (0..100). */
	UPROPERTY()
	float Lucidity = 0.f;

	/** Has the player interacted with the notebook at least once. */
	UPROPERTY()
	bool bNotebookInteracted = false;

	/** How many times the corner triangle was witnessed. */
	UPROPERTY()
	int32 TriangleEncounters = 0;

	/** Dream fragments discovered (symbols, phrases, locations). */
	UPROPERTY()
	TArray<FString> DiscoveredMemories;

	/** Notebook page text added so far. */
	UPROPERTY()
	TArray<FString> NotebookPages;

	/** Environment flags (door open, lights off, etc). */
	UPROPERTY()
	TMap<FString, bool> EnvironmentFlags;
};