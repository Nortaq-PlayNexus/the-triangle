#include "Core/ATriangleGameMode.h"

#include "Triangle.h"
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