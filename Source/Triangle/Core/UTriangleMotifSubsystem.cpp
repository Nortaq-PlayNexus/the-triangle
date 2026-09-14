#include "Core/UTriangleMotifSubsystem.h"

#include "Triangle.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Engine/World.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "TimerManager.h"

void UTriangleMotifSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTriangleMotifSubsystem::RegisterMotif(FName Id, const FTriangleMotifConfig& Config)
{
	FMotifEntry& Entry = MotifTable.FindOrAdd(Id);
	Entry.Config = Config;
	Entry.bEnabled = true;
	Entry.NextScheduledTime = DreamClock + FMath::FRandRange(Config.MinCooldown, Config.MaxCooldown);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Motif '%s' registered."), *Id.ToString());
}

void UTriangleMotifSubsystem::SetMotifEnabled(FName Id, bool bEnabled)
{
	FMotifEntry* Entry = MotifTable.Find(Id);
	if (Entry)
	{
		Entry->bEnabled = bEnabled;
	}
}

void UTriangleMotifSubsystem::EnableAutomaticScheduling(bool bOn)
{
	if (bSchedulingEnabled == bOn)
	{
		return;
	}
	bSchedulingEnabled = bOn;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (bOn)
	{
		World->GetTimerManager().ClearTimer(ClockTimerHandle);
		World->GetTimerManager().SetTimer(ClockTimerHandle, this, &UTriangleMotifSubsystem::ClockTick, 1.f, true);
		UE_LOG(LogTriangleDream, Log, TEXT("Motif scheduling clock started."));
	}
	else
	{
		World->GetTimerManager().ClearTimer(ClockTimerHandle);
	}
}

void UTriangleMotifSubsystem::QueueAutomaticMotif(FName Id)
{
	FMotifEntry* Entry = MotifTable.Find(Id);
	if (!Entry)
	{
		return;
	}
	Entry->NextScheduledTime = DreamClock + FMath::FRandRange(Entry->Config.MinCooldown, Entry->Config.MaxCooldown);
}

bool UTriangleMotifSubsystem::RequestMotif(FName Id)
{
	FMotifEntry* Entry = MotifTable.Find(Id);
	if (!Entry)
	{
		UE_LOG(LogTriangleDream, Warning, TEXT("RequestMotif('%s') but that motif is not registered."), *Id.ToString());
		return false;
	}

	OnMotif.Broadcast(Id);
	QueueAutomaticMotif(Id);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Motif '%s' occurred."), *Id.ToString());
	return true;
}

void UTriangleMotifSubsystem::ClockTick()
{
	DreamClock += 1.f;

	TArray<FName> DueMotifs;
	for (auto& Pair : MotifTable)
	{
		const FName Id = Pair.Key;
		FMotifEntry& Entry = Pair.Value;
		if (!Entry.bEnabled || DreamClock < Entry.NextScheduledTime)
		{
			continue;
		}

		const float RecognitionRequirement = Entry.Config.RecognitionRequirement;
		if (RecognitionRequirement > 0.f)
		{
			float PlayerRecognition = 0.f;
			if (ULTriangleRecognitionComponent* Recog = FTriangleGameplayStatics::GetPlayerRecognition(GetWorld()))
			{
				PlayerRecognition = Recog->GetRecognition();
			}
			// Motifs gate on the player's actual pattern knowledge. Until then
			// the motif reschedules inside its window and interleaves naturally.
			if (PlayerRecognition < RecognitionRequirement)
			{
				Entry.NextScheduledTime = DreamClock + FMath::FRandRange(Entry.Config.MinCooldown, Entry.Config.MaxCooldown);
				continue;
			}
		}

		DueMotifs.Add(Id);
		Entry.NextScheduledTime = DreamClock + FMath::FRandRange(Entry.Config.MinCooldown, Entry.Config.MaxCooldown);
	}

	// Broadcast after iterating so handlers can safely RequestMotif without
	// mutating the table mid-loop.
	for (const FName& Id : DueMotifs)
	{
		OnMotif.Broadcast(Id);
	}
}