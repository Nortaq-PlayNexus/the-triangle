#include "Core/UTriangleGameFlowSubsystem.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Dream/ATriangleDreamDirector.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ATriangleHUD.h"
#include "TimerManager.h"
#include "World/ATriangleDoor.h"
#include "World/ATriangleRoomBuilder.h"
#include "World/ATriangleStationBuilder.h"

namespace TriangleGameFlow
{
	constexpr float TransitionSeconds = 0.45f;
	constexpr float TransitionInputUnlockSeconds = 1.0f;

	FName IdFor(ETriangleDreamEnvironment Environment)
	{
		return Environment == ETriangleDreamEnvironment::UndergroundStation
			? FName(TEXT("UndergroundStation"))
			: FName(TEXT("ChildhoodBedroom"));
	}

	ETriangleDreamEnvironment EnvironmentForId(const FName Id)
	{
		return Id == FName(TEXT("UndergroundStation"))
			? ETriangleDreamEnvironment::UndergroundStation
			: ETriangleDreamEnvironment::ChildhoodBedroom;
	}
}

FName UTriangleGameFlowSubsystem::GetCurrentEnvironmentId() const
{
	return TriangleGameFlow::IdFor(CurrentEnvironment);
}

void UTriangleGameFlowSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TransitionTimerHandle);
		World->GetTimerManager().ClearTimer(TransitionInputUnlockHandle);
	}
	SetTransitionInputLocked(false);
	ActiveEnvironmentBuilder = nullptr;
	bTransitioning = false;
	Super::Deinitialize();
}

bool UTriangleGameFlowSubsystem::EnsureEnvironment(ETriangleDreamEnvironment Environment)
{
	if (bTransitioning)
	{
		return false;
	}
	if (CurrentEnvironment == Environment && IsValid(ActiveEnvironmentBuilder))
	{
		return true;
	}
	return SwitchEnvironmentImmediate(Environment);
}

bool UTriangleGameFlowSubsystem::TransitionTo(ETriangleDreamEnvironment Environment)
{
	if (bTransitioning)
	{
		return false;
	}
	if (CurrentEnvironment == Environment && IsValid(ActiveEnvironmentBuilder))
	{
		return true;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	PendingEnvironment = Environment;
	World->GetTimerManager().ClearTimer(TransitionInputUnlockHandle);
	bTransitioning = true;
	SetTransitionInputLocked(true);
	if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(World))
	{
		HUD->StartDreamTransition(0.20f, 0.50f, 0.30f, FLinearColor::Black);
	}
	OnEnvironmentTransitionStarted.Broadcast(Environment);
	World->GetTimerManager().SetTimer(
		TransitionTimerHandle,
		this,
		&UTriangleGameFlowSubsystem::FinishTimedTransition,
		TriangleGameFlow::TransitionSeconds,
		false);
	World->GetTimerManager().SetTimer(
		TransitionInputUnlockHandle,
		this,
		&UTriangleGameFlowSubsystem::UnlockTransitionInput,
		TriangleGameFlow::TransitionInputUnlockSeconds,
		false);
	return true;
}

bool UTriangleGameFlowSubsystem::IsKnownEnvironmentId(const FName EnvironmentId) const
{
	return EnvironmentId == FName(TEXT("ChildhoodBedroom"))
		|| EnvironmentId == FName(TEXT("UndergroundStation"));
}

bool UTriangleGameFlowSubsystem::RestoreEnvironmentId(const FName EnvironmentId)
{
	if (!IsKnownEnvironmentId(EnvironmentId))
	{
		UE_LOG(LogTriangle, Warning, TEXT("Refusing unknown dream environment id '%s'."), *EnvironmentId.ToString());
		return false;
	}
	return TransitionTo(TriangleGameFlow::EnvironmentForId(EnvironmentId));
}

bool UTriangleGameFlowSubsystem::SwitchEnvironmentImmediate(ETriangleDreamEnvironment Environment)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	AActor* PreviousBuilder = ActiveEnvironmentBuilder.Get();
	UClass* BuilderClass = Environment == ETriangleDreamEnvironment::UndergroundStation
		? ATriangleStationBuilder::StaticClass()
		: ATriangleRoomBuilder::StaticClass();
	AActor* NewBuilder = nullptr;

	// Build the replacement before destroying the current environment. If
	// spawning fails, the player keeps a valid world instead of being dropped
	// into an empty level after the fade.
	if (IsValid(PreviousBuilder) && CurrentEnvironment == Environment)
	{
		NewBuilder = PreviousBuilder;
	}
	else
	{
		NewBuilder = UGameplayStatics::GetActorOfClass(World, BuilderClass);
	}

	if (!IsValid(NewBuilder))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		NewBuilder = World->SpawnActor<AActor>(BuilderClass, FTransform::Identity, SpawnParameters);
		if (!IsValid(NewBuilder))
		{
			UE_LOG(LogTriangle, Error, TEXT("Could not build dream environment '%s'."), *TriangleGameFlow::IdFor(Environment).ToString());
			return false;
		}
	}

	if (IsValid(PreviousBuilder) && PreviousBuilder != NewBuilder)
	{
		if (CurrentEnvironment == ETriangleDreamEnvironment::ChildhoodBedroom)
		{
			if (ATriangleDoor* PreviousDoor = UGameplayStatics::GetActorOfClass(World, ATriangleDoor::StaticClass()); IsValid(PreviousDoor))
			{
				bLastBedroomDoorOpen = PreviousDoor->IsOpen();
			}
		}
		PreviousBuilder->Destroy();
	}

	ActiveEnvironmentBuilder = NewBuilder;
	CurrentEnvironment = Environment;

	// Builders create their own tagged DreamStart in BeginPlay. Move the pawn
	// only after the new actor has finished building so startup and loaded
	// transitions use exactly the same path.
	APlayerStart* Destination = nullptr;
	if (ATriangleRoomBuilder* Room = Cast<ATriangleRoomBuilder>(ActiveEnvironmentBuilder))
	{
		Destination = Room->GetDreamStart();
	}
	else if (ATriangleStationBuilder* Station = Cast<ATriangleStationBuilder>(ActiveEnvironmentBuilder))
	{
		Destination = Station->GetDreamStart();
	}
	if (!IsValid(Destination))
	{
		TArray<AActor*> DreamStarts;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, APlayerStart::StaticClass(), TEXT("DreamStart"), DreamStarts);
		for (AActor* Candidate : DreamStarts)
		{
			if (IsValid(Candidate))
			{
				Destination = Cast<APlayerStart>(Candidate);
				break;
			}
		}
	}

	if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			if (Destination)
			{
				Pawn->TeleportTo(Destination->GetActorLocation(), Destination->GetActorRotation(), false, true);
				PC->SetControlRotation(Destination->GetActorRotation());
			}
		}
	}

	if (ATriangleDreamDirector* Director = FTriangleGameplayStatics::GetDreamDirector(World))
	{
		Director->HandleEnvironmentChanged();
	}
	if (CurrentEnvironment == ETriangleDreamEnvironment::ChildhoodBedroom)
	{
		if (ATriangleDoor* Door = UGameplayStatics::GetActorOfClass(World, ATriangleDoor::StaticClass()))
		{
			bLastBedroomDoorOpen = Door->IsOpen();
			Door->OnTransitionRequested.AddUniqueDynamic(this, &UTriangleGameFlowSubsystem::HandleDoorTransitionRequested);
		}
	}

	OnEnvironmentReady.Broadcast(CurrentEnvironment);
	UE_LOG(LogTriangle, Log, TEXT("Dream environment ready: %s."), *GetCurrentEnvironmentId().ToString());
	return true;
}

void UTriangleGameFlowSubsystem::FinishTimedTransition()
{
	const ETriangleDreamEnvironment EnvironmentToBuild = PendingEnvironment;
	if (!SwitchEnvironmentImmediate(EnvironmentToBuild))
	{
		OnEnvironmentTransitionFailed.Broadcast(EnvironmentToBuild);
		if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(GetWorld()))
		{
			HUD->StopDreamTransition();
		}
		UnlockTransitionInput();
	}
}

void UTriangleGameFlowSubsystem::SetTransitionInputLocked(const bool bLocked)
{
	if (bLocked)
	{
		if (bInputLockHeld)
		{
			return;
		}
		if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld()))
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);
			InputLockController = PC;
			bInputLockHeld = true;
		}
	}
	else
	{
		if (bInputLockHeld)
		{
			if (APlayerController* PC = InputLockController.Get())
			{
				PC->SetIgnoreMoveInput(false);
				PC->SetIgnoreLookInput(false);
			}
		}
		InputLockController.Reset();
		bInputLockHeld = false;
	}
}

void UTriangleGameFlowSubsystem::UnlockTransitionInput()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TransitionInputUnlockHandle);
	}
	SetTransitionInputLocked(false);
	bTransitioning = false;
}

void UTriangleGameFlowSubsystem::HandleDoorTransitionRequested(ATriangleDoor* Door)
{
	if (!IsValid(Door) || bTransitioning)
	{
		return;
	}
	bLastBedroomDoorOpen = Door->IsOpen();
	TransitionTo(ETriangleDreamEnvironment::UndergroundStation);
}
