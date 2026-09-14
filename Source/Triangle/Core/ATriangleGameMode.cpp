#include "Core/ATriangleGameMode.h"

#include "Triangle.h"
#include "AI/ENightmareEntityState.h"
#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Dream/ATriangleDreamDirector.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleRoomBuilder.h"

#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ATriangleGameMode::ATriangleGameMode()
{
	DefaultPawnClass = ATriangleCharacter::StaticClass();
	PlayerControllerClass = ATrianglePlayerController::StaticClass();
	HUDClass = ATriangleHUD::StaticClass();
}

AActor* ATriangleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Always spawn the player inside the procedurally-assembled room when the
	// room builder has placed its own start (tagged DreamStart). The room is
	// the only place with content in the slice; map-native starts scatter the
	// player across a template landscape with nothing to see.
	if (UWorld* World = GetWorld())
	{
		TArray<AActor*> DreamStarts;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, APlayerStart::StaticClass(), TEXT("DreamStart"), DreamStarts);
		if (DreamStarts.Num() > 0)
		{
			return DreamStarts[0];
		}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATriangleGameMode::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();

	// The slice is entirely code-driven: if no level has placed the room
	// builder, assemble the childhood bedroom from the origin so PIE always
	// has a world to walk around in.
	if (World && !UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()))
	{
		World->SpawnActor<ATriangleRoomBuilder>(ATriangleRoomBuilder::StaticClass(), FTransform::Identity);
	}

	if (World && DreamDirector == nullptr)
	{
		DreamDirector = World->SpawnActor<ATriangleDreamDirector>(ATriangleDreamDirector::StaticClass(), FTransform::Identity);
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