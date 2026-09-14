#include "Core/ATriangleGameMode.h"

#include "AI/ENightmareEntityState.h"
#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Dream/ATriangleDreamDirector.h"
#include "UI/ATriangleHUD.h"

#include "Engine/World.h"

ATriangleGameMode::ATriangleGameMode()
{
	DefaultPawnClass = ATriangleCharacter::StaticClass();
	PlayerControllerClass = ATrianglePlayerController::StaticClass();
	HUDClass = ATriangleHUD::StaticClass();

	// Preferred setup for a tight first-person horror: no player start required,
	// the room builder positions the camera itself.
	DefaultPlayerStart = nullptr;
}

void ATriangleGameMode::StartPlay()
{
	Super::StartPlay();

	if (DreamDirector == nullptr)
	{
		DreamDirector = GetWorld()->SpawnActor<ATriangleDreamDirector>(ATriangleDreamDirector::StaticClass(), FTransform::Identity);
		if (DreamDirector)
		{
			DreamDirector->InitializeDream();
		}
		else
		{
			UE_LOG(LogTriangle, Error, TEXT("Failed to spawn Dream Director in StartPlay."));
		}
	}
}