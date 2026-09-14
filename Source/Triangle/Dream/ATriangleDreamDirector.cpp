#include "Dream/ATriangleDreamDirector.h"

#include "Triangle.h"
#include "Audio/UTriangleAudioSubsystem.h"
#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Dream/ATriangleTrianglePoint.h"
#include "Dream/UTriangleManifestationDefinition.h"
#include "Dream/UTriangleFalseAwakeningManager.h"
#include "Core/UTriangleMotifSubsystem.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "Nightmare/UTriangleAnomalySubsystem.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleRoomBuilder.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace TriangleDirectorPrivate
{
	constexpr int32 NumPoolManifestations = 6;
}

ATriangleDreamDirector::ATriangleDreamDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// Default bodies The Triangle can take in the slice. Authoring can swap the
	// whole pool via the ManifestationPool property on the instance.
	{
		int32 Index = 0;
		const TCHAR* Names[TriangleDirectorPrivate::NumPoolManifestations] = {
			TEXT("UpperCornerPoint"), TEXT("TheHumOnly"), TEXT("GapInTheWall"),
			TEXT("CornerPulled"), TEXT("DistantPoint"), TEXT("DoorwayCut")
		};
		for (const TCHAR* Name : Names)
		{
			UTriangleManifestationDefinition* Def = CreateDefaultSubobject<UTriangleManifestationDefinition>(FName(Name));
			Def->ManifestId = FName(Name);
			Def->ObserverNote = FText::FromString(TEXT("Default slice manifestation."));
			ManifestationPool.Add(Def);
			++Index;
		}
	}
	// Preset the kinds on the default bodies.
	{
		UTriangleManifestationDefinition* Def = ManifestationPool[0];
		Def->Kind = ETriangleManifestationKind::Point;
		Def->AnomalyKind = ETriangleAnomalyKind::None;
		Def->HumPresence = 0.55f;
		Def->MinDuration = 4.f;
		Def->MaxDuration = 7.f;
		Def->RecognitionGainOnSight = 6.f;
		Def->CooldownSeconds = 14.f;

		UTriangleManifestationDefinition* HumOnly = ManifestationPool[1];
		HumOnly->Kind = ETriangleManifestationKind::HumOnly;
		HumOnly->AnomalyKind = ETriangleAnomalyKind::None;
		HumOnly->bUseHum = true;
		HumOnly->HumPresence = 0.85f;
		HumOnly->MinDuration = 4.f;
		HumOnly->MaxDuration = 8.f;
		HumOnly->RecognitionGainOnSight = 4.f;
		HumOnly->CooldownSeconds = 20.f;

		UTriangleManifestationDefinition* Gap = ManifestationPool[2];
		Gap->Kind = ETriangleManifestationKind::TriangularGap;
		Gap->AnomalyKind = ETriangleAnomalyKind::TriangularGap;
		Gap->HumPresence = 0.6f;
		Gap->MinDuration = 5.f;
		Gap->MaxDuration = 9.f;
		Gap->RecognitionGainOnSight = 6.f;
		Gap->CooldownSeconds = 16.f;

		UTriangleManifestationDefinition* Split = ManifestationPool[3];
		Split->Kind = ETriangleManifestationKind::CornerSplit;
		Split->AnomalyKind = ETriangleAnomalyKind::CornerSplit;
		Split->HumPresence = 0.5f;
		Split->MinDuration = 5.f;
		Split->MaxDuration = 8.f;
		Split->RecognitionGainOnSight = 6.f;
		Split->CooldownSeconds = 20.f;

		UTriangleManifestationDefinition* Distant = ManifestationPool[4];
		Distant->Kind = ETriangleManifestationKind::DistantBlackPoint;
		Distant->AnomalyKind = ETriangleAnomalyKind::DistantBlackPoint;
		Distant->HumPresence = 0.45f;
		Distant->MinDuration = 5.f;
		Distant->MaxDuration = 10.f;
		Distant->RecognitionGainOnSight = 5.f;
		Distant->CooldownSeconds = 18.f;

		UTriangleManifestationDefinition* Door = ManifestationPool[5];
		Door->Kind = ETriangleManifestationKind::TriangularDoorway;
		Door->AnomalyKind = ETriangleAnomalyKind::TriangularDoorway;
		Door->HumPresence = 0.75f;
		Door->MinDuration = 5.f;
		Door->MaxDuration = 8.f;
		Door->RecognitionGainOnSight = 7.f;
		Door->CooldownSeconds = 24.f;
	}
}

void ATriangleDreamDirector::InitializeDream()
{
	if (HasActorBegunPlay())
	{
		UE_LOG(LogTriangleDream, Verbose, TEXT("Dream Director initializing."));
	}
	TimeSinceLastManifest = ManifestInterval; // give the player a quiet first beat
}

void ATriangleDreamDirector::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTriangleDream, Log, TEXT("The dream begins."));
}

ETriangleLucidityLevel ATriangleDreamDirector::GetCurrentLucidityLevel() const
{
	return PlayerLucidity ? PlayerLucidity->GetLucidityLevel() : ETriangleLucidityLevel::Low;
}

ETriangleRecognitionLevel ATriangleDreamDirector::GetCurrentRecognitionLevel() const
{
	return PlayerRecognition ? PlayerRecognition->GetRecognitionLevel() : ETriangleRecognitionLevel::Unknowing;
}

bool ATriangleDreamDirector::AcquireReferences()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	if (!PlayerActor)
	{
		PlayerActor = Cast<ATriangleCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
	}

	if (!PlayerLucidity && PlayerActor)
	{
		PlayerLucidity = PlayerActor->FindComponentByClass<ULTriangleLucidityComponent>();
	}
	if (!PlayerRecognition && PlayerActor)
	{
		PlayerRecognition = PlayerActor->FindComponentByClass<ULTriangleRecognitionComponent>();
	}

	if (PlayerLucidity && PlayerRecognition && !bBoundEvents)
	{
		PlayerLucidity->OnLucidityLevelChanged.AddDynamic(this, &ATriangleDreamDirector::HandleLucidityLevelChanged);
		PlayerRecognition->OnRecognitionLevelChanged.AddDynamic(this, &ATriangleDreamDirector::HandleRecognitionLevelChanged);
		bBoundEvents = true;
	}

	if (!RoomBuilder)
	{
		RoomBuilder = Cast<ATriangleRoomBuilder>(UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()));
		if (RoomBuilder)
		{
			CornerLocation = RoomBuilder->GetTriangleCornerLocation();
		}
	}

	if (!CornerPoint)
	{
		CornerPoint = Cast<ATriangleTrianglePoint>(UGameplayStatics::GetActorOfClass(World, ATriangleTrianglePoint::StaticClass()));
	}

	if (!AnomalySys)
	{
		AnomalySys = World->GetSubsystem<UTriangleAnomalySubsystem>();
	}
	if (!AudioSys)
	{
		AudioSys = World->GetSubsystem<UTriangleAudioSubsystem>();
	}
	if (!MotifSys)
	{
		MotifSys = World->GetSubsystem<UTriangleMotifSubsystem>();
		if (MotifSys)
		{
			FTriangleMotifConfig Clock12;
			Clock12.MinCooldown = 28.f;
			Clock12.MaxCooldown = 50.f;
			Clock12.RecognitionRequirement = 25.f;
			MotifSys->RegisterMotif(TEXT("ClockStops12"), Clock12);

			FTriangleMotifConfig Platform12Train;
			Platform12Train.MinCooldown = 45.f;
			Platform12Train.MaxCooldown = 90.f;
			Platform12Train.RecognitionRequirement = 50.f;
			MotifSys->RegisterMotif(TEXT("Platform12Train"), Platform12Train);

			MotifSys->OnMotif.AddDynamic(this, &ATriangleDreamDirector::HandleMotif);
		}
	}
	if (!WakeSys)
	{
		WakeSys = World->GetSubsystem<UTriangleFalseAwakeningManager>();
		if (WakeSys)
		{
			WakeSys->OnFalseAwakening.AddDynamic(this, &ATriangleDreamDirector::HandleWakeCycle);
			WakeSys->OnWakeResetRequested.AddUObject(this, &ATriangleDreamDirector::WithdrawPresence);
		}
	}

	return PlayerActor && PlayerLucidity && PlayerRecognition && RoomBuilder && CornerPoint;
}

void ATriangleDreamDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!AcquireReferences())
	{
		return;
	}

	TimeSinceLastManifest += DeltaSeconds;
	TimeSinceMasterSense += DeltaSeconds;

	WatchCorner(DeltaSeconds);
	UpdatePresence(DeltaSeconds);
	TryMasterSense(DeltaSeconds);

	if (MotifSys && !bSchedulingStarted)
	{
		// First references collected: the dream's motifs begin their schedule.
		bSchedulingStarted = true;
		MotifSys->EnableAutomaticScheduling(true);
	}
}

void ATriangleDreamDirector::WatchCorner(float DeltaSeconds)
{
	UCameraComponent* Camera = PlayerActor ? PlayerActor->GetFirstPersonCamera() : nullptr;
	if (!Camera)
	{
		return;
	}

	const FVector CameraLoc = Camera->GetComponentLocation();
	const FVector ToCorner = CornerLocation - CameraLoc;
	const float Distance = ToCorner.Size();
	const float Forwardness = FVector::DotProduct(Camera->GetForwardVector(), ToCorner.GetSafeNormal());

	const bool bWatching = Distance <= WatchRange && Forwardness >= WatchFovDotThreshold;
	if (bWatching)
	{
		TimeWatching += DeltaSeconds;
		PlayerLucidity->AddLucidity(WatchLucidityPerSecond * DeltaSeconds);

		// The corner only yields its point once the gaze is held, at most once
		// per manifest interval.
		if (TimeWatching >= 1.2f && TimeSinceLastManifest >= ManifestInterval && !(CornerPoint && CornerPoint->IsVisible()))
		{
			TimeSinceLastManifest = 0.f;
			TimeWatching = 0.f;
			ExecuteManifestation();
		}
	}
	else
	{
		TimeWatching = FMath::Max(0.f, TimeWatching - DeltaSeconds);
	}
}

void ATriangleDreamDirector::ExecuteManifestation(bool bSenseOnly)
{
	// Choose a body. Lower recognition keeps things close to the learned corner;
	// higher recognition opens up the stranger bodies and, finally, the hum-only.
	TArray<UTriangleManifestationDefinition*, TInlineAllocator<16>> Candidates;
	for (TObjectPtr<UTriangleManifestationDefinition> Def : ManifestationPool)
	{
		if (!Def)
		{
			continue;
		}
		if (bSenseOnly && Def->Kind != ETriangleManifestationKind::HumOnly)
		{
			continue;
		}
		if (!bSenseOnly && Def->Kind == ETriangleManifestationKind::HumanForm)
		{
			continue; // reserved for later chapters
		}
		Candidates.Add(Def.Get());
	}
	if (Candidates.Num() == 0)
	{
		return;
	}

	// Simple weight: recognize more -> willing to take less familiar bodies.
	const ETriangleRecognitionLevel Level = GetCurrentRecognitionLevel();
	int32 MaxWeightIndex = 0;
	if (Level <= ETriangleRecognitionLevel::Unknowing)
	{
		MaxWeightIndex = 0; // the corner only
	}
	else if (Level <= ETriangleRecognitionLevel::Noticing)
	{
		MaxWeightIndex = 2;
	}
	else if (Level <= ETriangleRecognitionLevel::Recognizing)
	{
		MaxWeightIndex = 4;
	}
	else
	{
		MaxWeightIndex = Candidates.Num() - 1;
	}
	const int32 Pick = FMath::Clamp(FMath::RandRange(0, MaxWeightIndex), 0, Candidates.Num() - 1);
	UTriangleManifestationDefinition* Def = Candidates[Pick];

	const float Duration = Def->GetRandomDuration();
	bManifestationActive = true;

	if (Def->Kind == ETriangleManifestationKind::Point && CornerPoint)
	{
		CornerPoint->Manifest(Duration);
		UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s) as corner point for %.1fs."), *Def->ManifestId.ToString(), Duration);
		ResolveManifestation(Def, Duration);
	}
	else if (Def->Kind == ETriangleManifestationKind::HumOnly)
	{
		// Nothing visible - just the hum swelling like the room has drawn breath.
		CurrentHumTarget = Def->HumPresence;
		OnTriangleManifestation.Broadcast(Def->ManifestId);
		UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s): hum only, nothing to see."), *Def->ManifestId.ToString());
		ResolveManifestation(Def, Duration);
	}
	else if (AnomalySys)
	{
		const FVector Around = PlayerActor ? PlayerActor->GetActorLocation() : CornerLocation;
		AnomalySys->SpawnAnomaly(Def->AnomalyKind, Duration, Around);
		CurrentHumTarget = Def->HumPresence;
		OnTriangleManifestation.Broadcast(Def->ManifestId);
		OnDreamBleed.Broadcast();
		UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s) cut into the dream for %.1fs."), *Def->ManifestId.ToString(), Duration);
		ResolveManifestation(Def, Duration);
	}
	else
	{
		bManifestationActive = false;
	}
}

void ATriangleDreamDirector::ResolveManifestation(const UTriangleManifestationDefinition* Def, float Duration)
{
	// Recognition is granted for watching the pattern resolve. The hum-only and
	// the corner both teach the shape; a witnessed gap teaches more.
	const float Gain = Def ? Def->RecognitionGainOnSight : 4.f;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, [this, Def, Gain]()
	{
		if (PlayerRecognition && Def)
		{
			if (Def->Kind == ETriangleManifestationKind::HumOnly && !PlayerRecognition->CanSensePresenceBeforeVisual())
			{
				// Low recognition: a hum with nothing to show is only half-seen.
				PlayerRecognition->AddRecognition(Gain * 0.35f, Def->ManifestId);
			}
			else
			{
				PlayerRecognition->AddRecognition(Gain, Def->ManifestId);
			}
		}
		bManifestationActive = false;
	}, Duration, false);
}

void ATriangleDreamDirector::UpdatePresence(float DeltaSeconds)
{
	const bool bCornerVisible = CornerPoint && CornerPoint->IsVisible();
	const bool bWantsPresence = bManifestationActive || bCornerVisible;

	if (bWantsPresence)
	{
		TrianglePresence = FMath::Min(1.f, TrianglePresence + PresenceRampRate * DeltaSeconds);
	}
	else
	{
		TrianglePresence = FMath::Max(0.f, TrianglePresence - PresenceDecayRate * DeltaSeconds);
	}

	// Hum follows presence with a little attack lag so it "breathes" in.
	if (bWantsPresence)
	{
		CurrentHumTarget = CurrentHumTarget <= 0.f ? 0.55f : FMath::Max(CurrentHumTarget, TrianglePresence * 0.65f);
		// When a hum-only manifestation carries a higher target, honour it.
	}
	else
	{
		CurrentHumTarget = 0.f;
	}

	if (AudioSys)
	{
		AudioSys->SetHumPresence(CurrentHumTarget, HumFadeSeconds);
	}

	// Events.
	if (!bPresenceActive && TrianglePresence >= 0.05f)
	{
		bPresenceActive = true;
		LastAnnouncedPresence = TrianglePresence;
		OnPresenceStart.Broadcast(TrianglePresence);
		if (!bHumActive)
		{
			bHumActive = true;
			OnTriangleHumStart.Broadcast();
		}
	}
	else if (bPresenceActive)
	{
		if (TrianglePresence >= LastAnnouncedPresence + 0.15f)
		{
			LastAnnouncedPresence = TrianglePresence;
			OnPresenceIncrease.Broadcast(TrianglePresence);
		}
		else if (TrianglePresence <= 0.02f)
		{
			bPresenceActive = false;
			if (bHumActive)
			{
				bHumActive = false;
				OnTriangleHumStop.Broadcast();
			}
			OnPresenceDisappear.Broadcast();
		}
	}
}

void ATriangleDreamDirector::TryMasterSense(float DeltaSeconds)
{
	if (!PlayerRecognition || !PlayerRecognition->HasMasterSense())
	{
		return;
	}
	if (bManifestationActive)
	{
		return;
	}
	if (TimeSinceMasterSense < MasterSenseInterval)
	{
		return;
	}

	// At mastery the player can know The Triangle is here with nothing visible.
	// The hum answers, briefly, and recognition keeps a low-grade presence so
	// the player is never quite alone even between manifestations.
	TimeSinceMasterSense = 0.f;
	ExecuteManifestation(true);
}

void ATriangleDreamDirector::OnRecognitionMilestone(ETriangleRecognitionLevel NewLevel)
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	switch (NewLevel)
	{
	case ETriangleRecognitionLevel::Noticing:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You have seen that shape before. Not remembered - seen.")), 3.5f);
		break;
	case ETriangleRecognitionLevel::Recognizing:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You know what is here before you look.")), 3.5f);
		if (MotifSys)
		{
			MotifSys->RequestMotif(TEXT("ClockStops12"));
		}
		break;
	case ETriangleRecognitionLevel::Anticipating:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The hum is not in the room. It is listening with you.")), 4.f);
		if (AudioSys)
		{
			AudioSys->SetHumPresence(0.35f, 2.f);
		}
		if (MotifSys)
		{
			MotifSys->RequestMotif(TEXT("Platform12Train"));
		}
		break;
	case ETriangleRecognitionLevel::Master:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("It is here. It has always been here.")), 4.f);
		break;
	default:
		break;
	}
	OnTriangleRecognition.Broadcast();
}

void ATriangleDreamDirector::TriggerTriangleManifest(float Duration)
{
	if (!CornerPoint)
	{
		return;
	}
	CornerPoint->Manifest(Duration);
	bManifestationActive = true;
	if (CurrentHumTarget <= 0.f)
	{
		CurrentHumTarget = 0.6f;
	}
	UE_LOG(LogTriangleDream, Log, TEXT("Triangle manifest triggered (lucidity=%.1f)."), PlayerLucidity ? PlayerLucidity->GetLucidity() : -1.f);
}

void ATriangleDreamDirector::WithdrawPresence()
{
	bManifestationActive = false;
	CurrentHumTarget = 0.f;
	if (AudioSys)
	{
		AudioSys->SetHumPresence(0.f, 0.8f);
	}
	if (AnomalySys)
	{
		AnomalySys->DespawnAll();
	}
	if (CornerPoint)
	{
		CornerPoint->Hide();
	}
	UE_LOG(LogTriangleDream, Log, TEXT("All presence withdrawn."));
}

void ATriangleDreamDirector::HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel)
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	switch (NewLevel)
	{
	case ETriangleLucidityLevel::Medium:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("Something in this room is too familiar.")), 3.f);
		break;

	case ETriangleLucidityLevel::High:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You remember this place. You have always been here.")), 3.5f);
		break;

	case ETriangleLucidityLevel::Maximum:
		if (!bMaxReachedHandled)
		{
			bMaxReachedHandled = true;
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The dream knows you are awake.")), 4.f);
			// The point stops hiding at full lucidity.
			if (CornerPoint)
			{
				CornerPoint->Manifest(9999.f);
			}
			bManifestationActive = true;
		}
		break;

	default:
		break;
	}
}

void ATriangleDreamDirector::HandleRecognitionLevelChanged(ETriangleRecognitionLevel NewLevel)
{
	OnRecognitionMilestone(NewLevel);
}

void ATriangleDreamDirector::HandleMotif(FName MotifId)
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	if (MotifId == TEXT("ClockStops12"))
	{
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The clock stops at twelve.")), 3.5f);
		if (PlayerRecognition)
		{
			PlayerRecognition->AddRecognition(2.f, TEXT("ClockStops12"));
		}
	}
	else if (MotifId == TEXT("Platform12Train"))
	{
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("Somewhere far off, a train pulls into platform twelve.")), 4.f);
		if (PlayerRecognition)
		{
			PlayerRecognition->AddRecognition(3.f, TEXT("Platform12Train"));
		}
		if (AudioSys)
		{
			// A distant horn condensed into the hum: presence, briefly.
			AudioSys->SetHumPresence(0.5f, 1.2f);
			FTimerHandle HornRelease;
			GetWorldTimerManager().SetTimer(HornRelease, [this]()
			{
				if (AudioSys)
				{
					AudioSys->SetHumPresence(CurrentHumTarget, 1.5f);
				}
			}, 4.f, false);
		}
	}
}

void ATriangleDreamDirector::HandleWakeCycle(int32 LayerIndex)
{
	UE_LOG(LogTriangleDream, Log, TEXT("False wake layer %d."), LayerIndex);
	OnFalseAwakening.Broadcast(FName(*FString::Printf(TEXT("Wake_%d"), LayerIndex)));
}