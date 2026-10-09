#include "Save/UTriangleSaveSubsystem.h"

#include "Triangle.h"
#include "Save/UTriangleSaveGame.h"

#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Core/UTriangleGameFlowSubsystem.h"
#include "Dream/ATriangleDreamDirector.h"
#include "Dream/UTriangleFalseAwakeningManager.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "World/ATriangleDoor.h"
#include "World/ATriangleNotebook.h"
#include "World/ATriangleRoomBuilder.h"
#include "World/ATriangleStationBuilder.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

const FString UTriangleSaveSubsystem::SaveSlotName = TEXT("Dream_Slot_0");

void UTriangleSaveSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			Flow->OnEnvironmentReady.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentReady);
			Flow->OnEnvironmentTransitionFailed.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed);
		}
	}
	PendingSnapshot = nullptr;
	Super::Deinitialize();
}

namespace
{
	const FName& ChildhoodBedroomEnvironmentId()
	{
		static const FName Id(TEXT("ChildhoodBedroom"));
		return Id;
	}

	const FName& UndergroundStationEnvironmentId()
	{
		static const FName Id(TEXT("UndergroundStation"));
		return Id;
	}

	FName DiscoverCurrentEnvironmentId(UWorld* World)
	{
		if (!IsValid(World))
		{
			return NAME_None;
		}

		// The flow subsystem is authoritative when procedural environment swapping
		// is available. Builder/map checks keep snapshots useful in simpler worlds.
		if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			if (IsValid(Flow) && Flow->HasActiveEnvironment())
			{
				return Flow->GetCurrentEnvironmentId();
			}
		}

		if (IsValid(UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass())))
		{
			return ChildhoodBedroomEnvironmentId();
		}
		if (IsValid(UGameplayStatics::GetActorOfClass(World, ATriangleStationBuilder::StaticClass())))
		{
			return UndergroundStationEnvironmentId();
		}

		// Asset-built maps do not need a builder actor. The map name is a stable
		// fallback; an empty world deliberately leaves the ID unset.
		const FString MapName = World->GetMapName();
		return MapName.IsEmpty() ? NAME_None : FName(*MapName);
	}

	AActor* FindActiveProceduralBuilder(UWorld* World)
	{
		if (!IsValid(World))
		{
			return nullptr;
		}
		if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			if (Flow->GetCurrentEnvironment() == ETriangleDreamEnvironment::UndergroundStation)
			{
				if (ATriangleStationBuilder* Station = UGameplayStatics::GetActorOfClass(World, ATriangleStationBuilder::StaticClass()); IsValid(Station))
				{
					return Station;
				}
			}
			else if (ATriangleRoomBuilder* Room = UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()); IsValid(Room))
			{
				return Room;
			}
		}
		if (ATriangleRoomBuilder* Room = UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()); IsValid(Room))
		{
			return Room;
		}
		return UGameplayStatics::GetActorOfClass(World, ATriangleStationBuilder::StaticClass());
	}

	APlayerStart* GetProceduralEnvironmentStart(AActor* Builder)
	{
		if (ATriangleRoomBuilder* Room = Cast<ATriangleRoomBuilder>(Builder))
		{
			return Room->GetDreamStart();
		}
		if (ATriangleStationBuilder* Station = Cast<ATriangleStationBuilder>(Builder))
		{
			return Station->GetDreamStart();
		}
		return nullptr;
	}
}

void UTriangleSaveSubsystem::ResetSave()
{
	DeleteSaveData();
}

bool UTriangleSaveSubsystem::DeleteSaveData()
{
	if (!HasSaveData())
	{
		UE_LOG(LogTriangleSave, Log, TEXT("Save slot '%s' is already clear."), *SaveSlotName);
		return true;
	}

	const bool bDeleted = UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0);
	if (bDeleted)
	{
		UE_LOG(LogTriangleSave, Log, TEXT("Deleted save slot '%s'."), *SaveSlotName);
	}
	else
	{
		UE_LOG(LogTriangleSave, Error, TEXT("Failed to delete save slot '%s'."), *SaveSlotName);
	}
	return bDeleted;
}

UTriangleSaveGame* UTriangleSaveSubsystem::LoadGameNow()
{
	UTriangleSaveGame* Loaded = Cast<UTriangleSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!IsValid(Loaded))
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("No save data found in slot '%s'."), *SaveSlotName);
		return nullptr;
	}

	// Bound collection sizes before they reach gameplay code. SaveGame files are
	// local, but malformed or hand-edited data should not create unbounded work.
	constexpr int32 MaxSerializedEntries = 256;
	if (Loaded->RecognizedPatternIds.Num() > MaxSerializedEntries)
	{
		Loaded->RecognizedPatternIds.SetNum(MaxSerializedEntries);
	}
	if (Loaded->NotebookPages.Num() > MaxSerializedEntries)
	{
		Loaded->NotebookPages.SetNum(MaxSerializedEntries);
	}
	if (Loaded->DiscoveredMemories.Num() > MaxSerializedEntries)
	{
		Loaded->DiscoveredMemories.SetNum(MaxSerializedEntries);
	}
	for (int32 Index = Loaded->RecognizedPatternIds.Num() - 1; Index >= 0; --Index)
	{
		if (Loaded->RecognizedPatternIds[Index].IsNone())
		{
			Loaded->RecognizedPatternIds.RemoveAt(Index);
		}
	}

	// Preserve the intent of legacy fields without pretending an exact count or
	// state can be reconstructed when the dedicated value did not exist yet.
	if (Loaded->NotebookInteractionCount <= 0 && Loaded->bNotebookInteracted)
	{
		Loaded->NotebookInteractionCount = 1;
	}
	Loaded->NotebookInteractionCount = FMath::Max(0, Loaded->NotebookInteractionCount);
	Loaded->bNotebookInteracted = Loaded->NotebookInteractionCount > 0;
	Loaded->FalseAwakeningLayer = FMath::Max(0, Loaded->FalseAwakeningLayer);
	if (const bool* bLegacyDoorOpen = Loaded->EnvironmentFlags.Find(FString(TEXT("BedroomDoorOpen"))))
	{
		Loaded->bBedroomDoorOpen = *bLegacyDoorOpen;
	}

	if (Loaded->SchemaVersion > UTriangleSaveGame::CurrentSchemaVersion)
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Save '%s' uses newer schema %d (current is %d); refusing to load it."), *SaveSlotName, Loaded->SchemaVersion, UTriangleSaveGame::CurrentSchemaVersion);
		return nullptr;
	}
	if (Loaded->SchemaVersion <= 0)
	{
		if (Loaded->RecognizedPatternIds.IsEmpty() && Loaded->TriangleEncounters > 0)
		{
			// Legacy saves stored a count but no stable IDs. Preserve the fact
			// that the catalogue was non-empty with an explicit migration ID.
			Loaded->RecognizedPatternIds.Add(FName(TEXT("LegacyTrianglePattern")));
		}
		Loaded->bHasPlayerTransformInEnvironment = false;
		Loaded->SchemaVersion = UTriangleSaveGame::CurrentSchemaVersion;
		UE_LOG(LogTriangleSave, Log, TEXT("Migrated legacy pre-versioned save from slot '%s'."), *SaveSlotName);
	}
	else if (Loaded->SchemaVersion == 1)
	{
		// Schema 1 stored only an absolute world transform. Keep it usable in
		// the same environment, but force cross-environment loads to the new
		// builder start rather than interpreting it in the wrong world.
		Loaded->bHasPlayerTransformInEnvironment = false;
		Loaded->SchemaVersion = UTriangleSaveGame::CurrentSchemaVersion;
		UE_LOG(LogTriangleSave, Log, TEXT("Migrated schema 1 save from slot '%s' to environment-relative transform support."), *SaveSlotName);
	}

	UE_LOG(LogTriangleSave, Log,
		TEXT("Loaded save (schema=%d, lucidity=%.1f, patterns=%d, environment=%s, wake-layer=%d, door-open=%s)."),
		Loaded->SchemaVersion,
		Loaded->Lucidity,
		Loaded->RecognizedPatternIds.Num(),
		*Loaded->CurrentEnvironmentId.ToString(),
		Loaded->FalseAwakeningLayer,
		Loaded->bBedroomDoorOpen ? TEXT("true") : TEXT("false"));
	return Loaded;
}

bool UTriangleSaveSubsystem::HasSaveData() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}

bool UTriangleSaveSubsystem::SaveGameNow(UTriangleSaveGame* InSnapshot)
{
	UTriangleSaveGame* Snapshot = IsValid(InSnapshot) ? InSnapshot : BuildSnapshot();
	if (!IsValid(Snapshot))
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Save aborted: no valid snapshot could be built."));
		OnSaveCompleted.ExecuteIfBound(false);
		return false;
	}

	// A pre-versioned object may still be re-saved by native code. Stamp only
	// old/unknown inputs; never downgrade a snapshot written by a newer build.
	if (Snapshot->SchemaVersion <= 0)
	{
		Snapshot->SchemaVersion = UTriangleSaveGame::CurrentSchemaVersion;
	}

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(Snapshot, SaveSlotName, 0);
	UE_LOG(LogTriangleSave, Log, TEXT("Save %s (schema=%d, lucidity=%.1f)."),
		bSuccess ? TEXT("succeeded") : TEXT("FAILED"),
		Snapshot->SchemaVersion,
		Snapshot->Lucidity);
	OnSaveCompleted.ExecuteIfBound(bSuccess);
	return bSuccess;
}

ETriangleSaveApplyResult UTriangleSaveSubsystem::ApplySnapshot(UTriangleSaveGame* Snapshot)
{
	if (!IsValid(Snapshot))
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Cannot apply an invalid save snapshot."));
		return ETriangleSaveApplyResult::Failed;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Cannot apply save snapshot without a valid world."));
		return ETriangleSaveApplyResult::Failed;
	}

	UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>();
	if (Flow && Snapshot->CurrentEnvironmentId != NAME_None)
	{
		const bool bKnownEnvironment = Flow->IsKnownEnvironmentId(Snapshot->CurrentEnvironmentId);
		if (!bKnownEnvironment)
		{
			if (Flow->HasActiveEnvironment())
			{
				UE_LOG(LogTriangleSave, Warning, TEXT("Save contains unknown environment '%s' for a procedural world."), *Snapshot->CurrentEnvironmentId.ToString());
				return ETriangleSaveApplyResult::Failed;
			}
			// Asset-built fallback maps may legitimately use their map name as
			// the environment ID; there is no procedural swap to request.
		}
		else if (Snapshot->CurrentEnvironmentId != Flow->GetCurrentEnvironmentId())
		{
			if (Flow->IsTransitioning())
			{
				UE_LOG(LogTriangleSave, Warning, TEXT("Cannot load a snapshot while an environment transition is active."));
				return ETriangleSaveApplyResult::Failed;
			}

			PendingSnapshot = Snapshot;
			PendingEnvironment = Snapshot->CurrentEnvironmentId == FName(TEXT("UndergroundStation"))
				? ETriangleDreamEnvironment::UndergroundStation
				: ETriangleDreamEnvironment::ChildhoodBedroom;
			Flow->OnEnvironmentReady.AddUniqueDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentReady);
			Flow->OnEnvironmentTransitionFailed.AddUniqueDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed);
			if (!Flow->RestoreEnvironmentId(Snapshot->CurrentEnvironmentId))
			{
				Flow->OnEnvironmentReady.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentReady);
				Flow->OnEnvironmentTransitionFailed.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed);
				PendingSnapshot = nullptr;
				return ETriangleSaveApplyResult::Failed;
			}
			UE_LOG(LogTriangleSave, Log, TEXT("Queued snapshot for environment '%s'."), *Snapshot->CurrentEnvironmentId.ToString());
			return ETriangleSaveApplyResult::Queued;
		}
	}

	return ApplyPlayerState(Snapshot) ? ETriangleSaveApplyResult::Applied : ETriangleSaveApplyResult::Failed;
}

void UTriangleSaveSubsystem::HandleEnvironmentReady(ETriangleDreamEnvironment Environment)
{
	UTriangleSaveGame* Snapshot = PendingSnapshot.Get();
	PendingSnapshot = nullptr;
	if (UWorld* World = GetWorld())
	{
		if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			Flow->OnEnvironmentReady.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentReady);
			Flow->OnEnvironmentTransitionFailed.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed);
		}
	}
	if (Snapshot && Environment == PendingEnvironment)
	{
		const bool bApplied = ApplyPlayerState(Snapshot);
		UE_LOG(LogTriangleSave, Log, TEXT("Post-transition snapshot application %s."), bApplied ? TEXT("succeeded") : TEXT("failed"));
	}
	else if (Snapshot)
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Discarded queued snapshot because the ready environment did not match the requested environment."));
	}
}

void UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed(ETriangleDreamEnvironment Environment)
{
	PendingSnapshot = nullptr;
	if (UWorld* World = GetWorld())
	{
		if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			Flow->OnEnvironmentReady.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentReady);
			Flow->OnEnvironmentTransitionFailed.RemoveDynamic(this, &UTriangleSaveSubsystem::HandleEnvironmentTransitionFailed);
		}
	}
	UE_LOG(LogTriangleSave, Warning, TEXT("Queued snapshot discarded because environment transition failed."));
	if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld()); IsValid(PC))
	{
		PC->PushSubtitle(FText::FromString(TEXT("The room will not open for that memory.")), 2.5f);
	}
	(void)Environment;
}

bool UTriangleSaveSubsystem::ApplyPlayerState(UTriangleSaveGame* Snapshot)
{
	if (!IsValid(Snapshot))
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	bool bAppliedState = false;
	if (ATriangleDreamDirector* Director = FTriangleGameplayStatics::GetDreamDirector(World))
	{
		Director->ResetForSaveLoad();
	}
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World);
	APawn* Pawn = IsValid(PC) ? PC->GetPawn() : nullptr;
	FTransform DesiredWorldTransform = Snapshot->PlayerTransform;
	bool bCanApplyTransform = true;
	if (Snapshot->bHasPlayerTransformInEnvironment)
	{
		if (AActor* EnvironmentBuilder = FindActiveProceduralBuilder(World); IsValid(EnvironmentBuilder))
		{
			if (APlayerStart* EnvironmentStart = GetProceduralEnvironmentStart(EnvironmentBuilder); IsValid(EnvironmentStart))
			{
				DesiredWorldTransform = Snapshot->PlayerTransformInEnvironment * EnvironmentStart->GetActorTransform();
			}
			else
			{
				bCanApplyTransform = false;
			}
		}
		else
		{
			// A cross-environment load normally leaves the pawn at the new
			// builder's start when the destination has not finished spawning.
			bCanApplyTransform = false;
		}
	}
	else if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>();
		Flow && Flow->HasActiveEnvironment() && Snapshot->CurrentEnvironmentId != NAME_None
		&& Snapshot->CurrentEnvironmentId != Flow->GetCurrentEnvironmentId())
	{
		// Schema 1 saves contain only an absolute transform. It is unsafe to
		// apply that position in a different procedural world, so retain the
		// destination start instead.
		bCanApplyTransform = false;
	}

	if (IsValid(Pawn) && bCanApplyTransform)
	{
		if (DesiredWorldTransform.ContainsNaN())
		{
			UE_LOG(LogTriangleSave, Warning, TEXT("Save snapshot contains an invalid player transform; transform was not applied."));
		}
		else if (Pawn->SetActorTransform(DesiredWorldTransform, false, nullptr, ETeleportType::TeleportPhysics))
		{
			if (PC)
			{
				PC->SetControlRotation(DesiredWorldTransform.Rotator());
			}
			bAppliedState = true;
		}
		else
		{
			UE_LOG(LogTriangleSave, Warning, TEXT("Player transform could not be applied from save snapshot."));
		}
	}

	if (ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(World))
	{
		if (IsValid(Lucidity))
		{
			Lucidity->RestoreFromSave(Snapshot->Lucidity);
			bAppliedState = true;
		}
	}

	if (ULTriangleRecognitionComponent* Recognition = FTriangleGameplayStatics::GetPlayerRecognition(World))
	{
		if (IsValid(Recognition))
		{
			Recognition->RestoreFromSaveWithCatalogue(Snapshot->Recognition, Snapshot->RecognizedPatternIds);
			bAppliedState = true;
		}
	}

	if (UTriangleFalseAwakeningManager* WakeSystem = World->GetSubsystem<UTriangleFalseAwakeningManager>())
	{
		if (IsValid(WakeSystem))
		{
			WakeSystem->RestoreState(Snapshot->FalseAwakeningLayer);
			bAppliedState = true;
		}
	}

	if (ATriangleNotebook* Notebook = UGameplayStatics::GetActorOfClass(World, ATriangleNotebook::StaticClass()); IsValid(Notebook))
	{
		Notebook->SetInteractionCount(Snapshot->NotebookInteractionCount);
		bAppliedState = true;
	}

	if (ATriangleDoor* Door = UGameplayStatics::GetActorOfClass(World, ATriangleDoor::StaticClass()); IsValid(Door))
	{
		if (Snapshot->bBedroomDoorOpen)
		{
			if (!Door->IsOpen())
			{
				Door->Open(true);
			}
		}
		else if (Door->IsOpen())
		{
			Door->Close(true);
		}
		bAppliedState = true;
	}
	if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
	{
		Flow->SetLastBedroomDoorOpen(Snapshot->bBedroomDoorOpen);
	}

	if (!bAppliedState)
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Save snapshot was valid, but no world state could be applied."));
	}
	return bAppliedState;
}

UTriangleSaveGame* UTriangleSaveSubsystem::BuildSnapshot()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}

	UTriangleSaveGame* Save = Cast<UTriangleSaveGame>(UGameplayStatics::CreateSaveGameObject(UTriangleSaveGame::StaticClass()));
	if (!IsValid(Save))
	{
		return nullptr;
	}

	// Player state.
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World);
	APawn* Pawn = IsValid(PC) ? PC->GetPawn() : nullptr;
	if (IsValid(Pawn))
	{
		Save->PlayerTransform = Pawn->GetActorTransform();
		if (AActor* EnvironmentBuilder = FindActiveProceduralBuilder(World); IsValid(EnvironmentBuilder))
		{
			if (APlayerStart* EnvironmentStart = GetProceduralEnvironmentStart(EnvironmentBuilder); IsValid(EnvironmentStart))
			{
				Save->PlayerTransformInEnvironment = EnvironmentStart->GetActorTransform().GetRelativeTransform(Save->PlayerTransform);
				Save->bHasPlayerTransformInEnvironment = true;
			}
		}
	}

	// Lucidity.
	if (ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(World))
	{
		if (IsValid(Lucidity))
		{
			Save->Lucidity = Lucidity->GetLucidity();
		}
	}

	// Recognition and its stable pattern catalogue.
	if (ULTriangleRecognitionComponent* Recognition = FTriangleGameplayStatics::GetPlayerRecognition(World))
	{
		if (IsValid(Recognition))
		{
			Save->Recognition = Recognition->GetRecognition();
			Save->RecognizedPatternIds = Recognition->GetRecognizedPatternIds();
			Save->TriangleEncounters = Save->RecognizedPatternIds.Num();
		}
	}

	Save->CurrentEnvironmentId = DiscoverCurrentEnvironmentId(World);

	if (UTriangleFalseAwakeningManager* WakeSystem = World->GetSubsystem<UTriangleFalseAwakeningManager>())
	{
		if (IsValid(WakeSystem))
		{
			Save->FalseAwakeningLayer = FMath::Max(0, WakeSystem->GetCurrentLayer());
		}
	}

	// Actor-owned slice state is captured through explicit persistence hooks.
	if (ATriangleNotebook* Notebook = UGameplayStatics::GetActorOfClass(World, ATriangleNotebook::StaticClass()))
	{
		Save->NotebookInteractionCount = FMath::Max(0, Notebook->GetInteractionCount());
		Save->bNotebookInteracted = Save->NotebookInteractionCount > 0;
		const int32 PersistedPageCount = FMath::Clamp(Save->NotebookInteractionCount, 0, 3);
		for (int32 PageIndex = 0; PageIndex < PersistedPageCount; ++PageIndex)
		{
			const FString PageId = FString::Printf(TEXT("NotebookPage_%d"), PageIndex + 1);
			Save->NotebookPages.Add(PageId);
			Save->DiscoveredMemories.Add(PageId);
		}
	}
	if (ATriangleDoor* Door = UGameplayStatics::GetActorOfClass(World, ATriangleDoor::StaticClass()); IsValid(Door))
	{
		Save->bBedroomDoorOpen = Door->IsOpen();
		if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
		{
			Flow->SetLastBedroomDoorOpen(Save->bBedroomDoorOpen);
		}
	}
	else if (UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>())
	{
		Save->bBedroomDoorOpen = Flow->GetLastBedroomDoorOpen();
	}
	Save->EnvironmentFlags.Add(FString(TEXT("BedroomDoorOpen")), Save->bBedroomDoorOpen);

	return Save;
}
