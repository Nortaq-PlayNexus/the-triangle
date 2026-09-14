#include "Save/UTriangleSaveSubsystem.h"

#include "Triangle.h"
#include "Save/UTriangleSaveGame.h"

#include "Characters/ATriangleCharacter.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Lucidity/ULTriangleLucidityComponent.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

const FString UTriangleSaveSubsystem::SaveSlotName = TEXT("Dream_Slot_0");

void UTriangleSaveSubsystem::ResetSave()
{
	UTriangleSaveGame* Save = Cast<UTriangleSaveGame>(UGameplayStatics::CreateSaveGameObject(UTriangleSaveGame::StaticClass()));
	if (Save && UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0))
	{
		UE_LOG(LogTriangleSave, Log, TEXT("Save slot reset."));
	}
}

UTriangleSaveGame* UTriangleSaveSubsystem::LoadGameNow()
{
	UTriangleSaveGame* Loaded = Cast<UTriangleSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (Loaded)
	{
		UE_LOG(LogTriangleSave, Log, TEXT("Loaded save: lucidity=%.1f, memories=%d."), Loaded->Lucidity, Loaded->DiscoveredMemories.Num());
	}
	else
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("No save data found in slot '%s'."), *SaveSlotName);
	}
	return Loaded;
}

bool UTriangleSaveSubsystem::HasSaveData() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);
}

bool UTriangleSaveSubsystem::SaveGameNow(UTriangleSaveGame* InSnapshot)
{
	UTriangleSaveGame* Snapshot = InSnapshot ? InSnapshot : BuildSnapshot();
	if (!Snapshot)
	{
		UE_LOG(LogTriangleSave, Warning, TEXT("Save aborted: no snapshot could be built."));
		return false;
	}

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(Snapshot, SaveSlotName, 0);
	UE_LOG(LogTriangleSave, Log, TEXT("Save %s (lucidity=%.1f)."), bSuccess ? TEXT("succeeded") : TEXT("FAILED"), Snapshot->Lucidity);
	return bSuccess;
}

UTriangleSaveGame* UTriangleSaveSubsystem::BuildSnapshot()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UTriangleSaveGame* Save = Cast<UTriangleSaveGame>(UGameplayStatics::CreateSaveGameObject(UTriangleSaveGame::StaticClass()));
	if (!Save)
	{
		return nullptr;
	}

	// Player state.
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World);
	if (PC && PC->GetPawn())
	{
		Save->PlayerTransform = PC->GetPawn()->GetActorTransform();
	}

	// Lucidity.
	if (ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(World))
	{
		Save->Lucidity = Lucidity->GetLucidity();
	}

	return Save;
}