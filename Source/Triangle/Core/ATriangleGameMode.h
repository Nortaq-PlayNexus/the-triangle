#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ATriangleGameMode.generated.h"

class ATriangleDreamDirector;

/**
 * Base GameMode for The Triangle.
 * Spawns the Dream Director so every level has a dream controller.
 * Pawn/HUD/Controller classes are set in the constructor defaults.
 */
UCLASS()
class ATriangleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATriangleGameMode();

protected:
	virtual void StartPlay() override;

public:
	/** Returns the Dream Director owning this session, if one was spawned. */
	TObjectPtr<ATriangleDreamDirector> GetDreamDirectorInstance() const { return DreamDirector; }

	/** The primary Dream Director for this play session. */
	UPROPERTY()
	TObjectPtr<ATriangleDreamDirector> DreamDirector;
};