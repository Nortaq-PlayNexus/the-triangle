#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UTriangleSaveGame.generated.h"

/**
 * Serializable dream-state snapshot for a save slot. The current vertical
 * slice persists player progression, pattern memory, notebook progress,
 * environment identity, wake depth, and owned world flags.
 */
UCLASS()
class UTriangleSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Version written by the current persistence format. Zero denotes a pre-versioned save. */
	static const int32 CurrentSchemaVersion;

	UTriangleSaveGame();

	/** Version of the serialized field layout used by this snapshot. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triangle|Save")
	int32 SchemaVersion;

	/** Absolute player transform retained for legacy/debug compatibility. */
	UPROPERTY()
	FTransform PlayerTransform;

	/** Player transform relative to the active environment's generated start. */
	UPROPERTY()
	FTransform PlayerTransformInEnvironment = FTransform::Identity;

	/** True when PlayerTransformInEnvironment is authoritative for schema 2+. */
	UPROPERTY()
	bool bHasPlayerTransformInEnvironment = false;

	/** Current hidden Lucidity value (0..100). */
	UPROPERTY()
	float Lucidity = 0.f;

	/** Current hidden Recognition value (0..100). */
	UPROPERTY()
	float Recognition = 0.f;

	/** Stable pattern IDs already recorded in the player's Recognition catalogue. */
	UPROPERTY()
	TArray<FName> RecognizedPatternIds;

	/** Legacy compatibility flag for pre-count notebook saves. */
	UPROPERTY()
	bool bNotebookInteracted = false;

	/** Total notebook reads, including repeated reads of earlier pages. */
	UPROPERTY()
	int32 NotebookInteractionCount = 0;

	/** Number of distinct Triangle patterns currently catalogued. */
	UPROPERTY()
	int32 TriangleEncounters = 0;

	/** Dream fragments discovered (symbols, phrases, locations). */
	UPROPERTY()
	TArray<FString> DiscoveredMemories;

	/** Stable IDs for notebook pages added so far. */
	UPROPERTY()
	TArray<FString> NotebookPages;

	/** Stable identifier for the environment containing the player. */
	UPROPERTY()
	FName CurrentEnvironmentId = NAME_None;

	/** Number of completed false awakenings; zero means still in the first dream layer. */
	UPROPERTY()
	int32 FalseAwakeningLayer = 0;

	/** Whether the childhood bedroom's interactive door is open. */
	UPROPERTY()
	bool bBedroomDoorOpen = false;

	/** Environment flags (lights off and other extensible world state). */
	UPROPERTY()
	TMap<FString, bool> EnvironmentFlags;
};