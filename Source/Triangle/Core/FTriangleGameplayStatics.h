#pragma once

#include "CoreMinimal.h"

class ATriangleDreamDirector;
class ATrianglePlayerController;
class ATriangleHUD;
class UTriangleSaveSubsystem;
class ULTriangleLucidityComponent;

/**
 * Static accessors for the commonly-needed Triangle gameplay systems.
 * Keeps coupling low: gameplay code asks for systems by type instead of
 * chasing references around the world.
 */
class TRIANGLE_API FTriangleGameplayStatics
{
public:
	static ATriangleDreamDirector* GetDreamDirector(const UWorld* World);
	static ATrianglePlayerController* GetTrianglePlayerController(const UWorld* World);
	static ATriangleHUD* GetTriangleHUD(const UWorld* World);
	static UTriangleSaveSubsystem* GetSaveSubsystem(const UWorld* World);

	/** Returns the player's Lucidity component, or nullptr if the player can't be found. */
	static ULTriangleLucidityComponent* GetPlayerLucidity(const UWorld* World);
};