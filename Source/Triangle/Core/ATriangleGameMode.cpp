#include "Core/ATriangleGameMode.h"

#include "Triangle.h"
#include "Core/UTriangleGameFlowSubsystem.h"
#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Dream/ATriangleDreamDirector.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleRoomBuilder.h"
#include "World/ATriangleStationBuilder.h"

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
		APlayerStart* ScopedStart = nullptr;
		if (ATriangleRoomBuilder* Room = UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()))
		{
			ScopedStart = Room->GetDreamStart();
		}
		else if (ATriangleStationBuilder* Station = UGameplayStatics::GetActorOfClass(World, ATriangleStationBuilder::StaticClass()))
		{
			ScopedStart = Station->GetDreamStart();
		}
		if (IsValid(ScopedStart))
		{
			return ScopedStart;
		}

		TArray<AActor*> DreamStarts;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, APlayerStart::StaticClass(), TEXT("DreamStart"), DreamStarts);
		for (AActor* Candidate : DreamStarts)
		{
			if (IsValid(Candidate))
			{
				return Candidate;
			}
		}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATriangleGameMode::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (DreamDirector == nullptr)
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

	// The slice is entirely code-driven. The flow subsystem owns the active
	// builder and moves the already-possessed pawn onto the generated start.
	bool bEnvironmentReady = false;
	if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
	{
		bEnvironmentReady = Flow->EnsureEnvironment(ETriangleDreamEnvironment::ChildhoodBedroom);
		if (!bEnvironmentReady)
		{
			UE_LOG(LogTriangle, Error, TEXT("GameFlow could not assemble the childhood bedroom."));
		}
	}
	if (!bEnvironmentReady && !UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()))
	{
		// Defensive fallback for a stripped world without normal subsystem init.
		World->SpawnActor<ATriangleRoomBuilder>(ATriangleRoomBuilder::StaticClass(), FTransform::Identity);
	}
}