#include "Core/FTriangleGameplayStatics.h"

#include "Core/ATriangleGameMode.h"
#include "Characters/ATrianglePlayerController.h"
#include "Dream/ATriangleDreamDirector.h"
#include "Save/UTriangleSaveSubsystem.h"
#include "UI/ATriangleHUD.h"
#include "Lucidity/ULTriangleLucidityComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/UObjectGlobals.h"

ATriangleDreamDirector* FTriangleGameplayStatics::GetDreamDirector(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	ATriangleGameMode* GM = Cast<ATriangleGameMode>(World->GetAuthGameMode());
	return GM ? GM->GetDreamDirectorInstance() : nullptr;
}

ATrianglePlayerController* FTriangleGameplayStatics::GetTrianglePlayerController(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	return Cast<ATrianglePlayerController>(World->GetFirstPlayerController());
}

ATriangleHUD* FTriangleGameplayStatics::GetTriangleHUD(const UWorld* World)
{
	ATrianglePlayerController* PC = GetTrianglePlayerController(World);
	return PC ? Cast<ATriangleHUD>(PC->GetHUD()) : nullptr;
}

UTriangleSaveSubsystem* FTriangleGameplayStatics::GetSaveSubsystem(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	UGameInstance* GI = World->GetGameInstance();
	return GI ? GI->GetSubsystem<UTriangleSaveSubsystem>() : nullptr;
}

ULTriangleLucidityComponent* FTriangleGameplayStatics::GetPlayerLucidity(const UWorld* World)
{
	ATrianglePlayerController* PC = GetTrianglePlayerController(World);
	if (!PC || !PC->GetPawn())
	{
		return nullptr;
	}
	return PC->GetPawn()->FindComponentByClass<ULTriangleLucidityComponent>();
}