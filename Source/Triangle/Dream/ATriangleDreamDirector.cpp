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
#include "Nightmare/ATriangleAnomalyActor.h"
#include "Nightmare/UTriangleAnomalySubsystem.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleTrain.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace TriangleDirectorPrivate
{
	constexpr int32 NumPoolManifestations = 6;

	void PushSubtitle(const UWorld* World, const FText& Text, const float Duration)
	{
		ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World);
		ATriangleHUD* HUD = PC ? PC->GetTriangleHUD() : nullptr;
		if (HUD)
		{
			HUD->PushSubtitle(Text, Duration);
		}
	}
}

ATriangleDreamDirector::ATriangleDreamDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// Default bodies The Triangle can take in the slice. Authoring can swap the
	// whole pool via the ManifestationPool property on the instance.
	{
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
		Def->MinimumRecognition = 0.f;
		Def->SelectionWeight = 5.f;
		Def->CooldownSeconds = 14.f;

		UTriangleManifestationDefinition* HumOnly = ManifestationPool[1];
		HumOnly->Kind = ETriangleManifestationKind::HumOnly;
		HumOnly->AnomalyKind = ETriangleAnomalyKind::None;
		HumOnly->bUseHum = true;
		HumOnly->HumPresence = 0.85f;
		HumOnly->MinDuration = 4.f;
		HumOnly->MaxDuration = 8.f;
		HumOnly->RecognitionGainOnSight = 4.f;
		HumOnly->MinimumRecognition = 75.f;
		HumOnly->SelectionWeight = 0.7f;
		HumOnly->CooldownSeconds = 20.f;

		UTriangleManifestationDefinition* Gap = ManifestationPool[2];
		Gap->Kind = ETriangleManifestationKind::TriangularGap;
		Gap->AnomalyKind = ETriangleAnomalyKind::TriangularGap;
		Gap->HumPresence = 0.6f;
		Gap->MinDuration = 5.f;
		Gap->MaxDuration = 9.f;
		Gap->RecognitionGainOnSight = 6.f;
		Gap->MinimumRecognition = 25.f;
		Gap->SelectionWeight = 1.2f;
		Gap->CooldownSeconds = 16.f;

		UTriangleManifestationDefinition* Split = ManifestationPool[3];
		Split->Kind = ETriangleManifestationKind::CornerSplit;
		Split->AnomalyKind = ETriangleAnomalyKind::CornerSplit;
		Split->HumPresence = 0.5f;
		Split->MinDuration = 5.f;
		Split->MaxDuration = 8.f;
		Split->RecognitionGainOnSight = 6.f;
		Split->MinimumRecognition = 50.f;
		Split->SelectionWeight = 1.f;
		Split->CooldownSeconds = 20.f;

		UTriangleManifestationDefinition* Distant = ManifestationPool[4];
		Distant->Kind = ETriangleManifestationKind::DistantBlackPoint;
		Distant->AnomalyKind = ETriangleAnomalyKind::DistantBlackPoint;
		Distant->HumPresence = 0.45f;
		Distant->MinDuration = 5.f;
		Distant->MaxDuration = 10.f;
		Distant->RecognitionGainOnSight = 5.f;
		Distant->MinimumRecognition = 50.f;
		Distant->SelectionWeight = 0.8f;
		Distant->CooldownSeconds = 18.f;

		UTriangleManifestationDefinition* Door = ManifestationPool[5];
		Door->Kind = ETriangleManifestationKind::TriangularDoorway;
		Door->AnomalyKind = ETriangleAnomalyKind::TriangularDoorway;
		Door->HumPresence = 0.75f;
		Door->MinDuration = 5.f;
		Door->MaxDuration = 8.f;
		Door->RecognitionGainOnSight = 7.f;
		Door->MinimumRecognition = 75.f;
		Door->SelectionWeight = 0.6f;
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

	ATriangleCharacter* CurrentPlayer = Cast<ATriangleCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
	if (PlayerActor.Get() != CurrentPlayer)
	{
		if (IsValid(PlayerActor.Get()) || bManifestationActive || bPresenceActive || ActiveAnomaly.IsValid())
		{
			// A pending manifestation belongs to the old pawn. Reset it before
			// rebinding so its timer cannot award recognition to the replacement.
			WithdrawPresence();
		}
		if (IsValid(PlayerLucidity))
		{
			PlayerLucidity->OnLucidityLevelChanged.RemoveDynamic(this, &ATriangleDreamDirector::HandleLucidityLevelChanged);
		}
		if (IsValid(PlayerRecognition))
		{
			PlayerRecognition->OnRecognitionLevelChanged.RemoveDynamic(this, &ATriangleDreamDirector::HandleRecognitionLevelChanged);
		}
		PlayerActor = CurrentPlayer;
		PlayerLucidity = nullptr;
		PlayerRecognition = nullptr;
		bBoundEvents = false;
		bMaxReachedHandled = false;
		bMaximumPointReapplied = false;
	}

	if (!PlayerLucidity && PlayerActor)
	{
		PlayerLucidity = PlayerActor->FindComponentByClass<ULTriangleLucidityComponent>();
	}
	if (!PlayerRecognition && PlayerActor)
	{
		PlayerRecognition = PlayerActor->FindComponentByClass<ULTriangleRecognitionComponent>();
	}
	if (!IsValid(PlayerLucidity) || !IsValid(PlayerRecognition))
	{
		bBoundEvents = false;
	}

	if (!IsValid(CornerPoint))
	{
		CornerPoint = Cast<ATriangleTrianglePoint>(UGameplayStatics::GetActorOfClass(World, ATriangleTrianglePoint::StaticClass()));
		CornerLocation = CornerPoint ? CornerPoint->GetActorLocation() : FVector::ZeroVector;
	}

	if (PlayerLucidity && PlayerRecognition && !bBoundEvents)
	{
		PlayerLucidity->OnLucidityLevelChanged.AddDynamic(this, &ATriangleDreamDirector::HandleLucidityLevelChanged);
		PlayerRecognition->OnRecognitionLevelChanged.AddDynamic(this, &ATriangleDreamDirector::HandleRecognitionLevelChanged);
		bBoundEvents = true;
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
			Clock12.RecognitionRequirement = 50.f;
			MotifSys->RegisterMotif(TEXT("ClockStops12"), Clock12);

			FTriangleMotifConfig Platform12Train;
			Platform12Train.MinCooldown = 45.f;
			Platform12Train.MaxCooldown = 90.f;
			Platform12Train.RecognitionRequirement = 75.f;
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

	const bool bReady = IsValid(PlayerActor) && IsValid(PlayerLucidity) && IsValid(PlayerRecognition) && IsValid(CornerPoint);
	if (bReady && !bMaximumPointReapplied && IsValid(PlayerLucidity)
		&& PlayerLucidity->GetLucidityLevel() == ETriangleLucidityLevel::Maximum)
	{
		CornerPoint->Manifest(9999.f);
		bManifestationActive = true;
		bHumEnabledForCurrentManifestation = true;
		CurrentHumTarget = FMath::Max(CurrentHumTarget, 0.65f);
		bMaximumPointReapplied = true;
	}
	return bReady;
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
	for (auto It = ManifestCooldowns.CreateIterator(); It; ++It)
	{
		It.Value() -= DeltaSeconds;
		if (It.Value() <= 0.f)
		{
			It.RemoveCurrent();
		}
	}

	if (bManifestationActive && ActiveAnomaly.IsValid() && ActiveManifestationDefinition
		&& WasManifestationWitnessed(ActiveManifestationDefinition))
	{
		bCurrentAnomalyWasWitnessed = true;
	}

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
	bool bOccluded = false;
	if (Distance > 1.f && GetWorld())
	{
		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TriangleCornerWatch), false);
		QueryParams.AddIgnoredActor(this);
		const FVector TraceEnd = CameraLoc + ToCorner - ToCorner.GetSafeNormal() * 12.f;
		bOccluded = GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, TraceEnd, ECC_Visibility, QueryParams)
			&& Hit.bBlockingHit && Hit.Distance < Distance - 24.f;
	}

	const bool bWatching = !bOccluded && Distance <= WatchRange && Forwardness >= WatchFovDotThreshold;
	if (bWatching)
	{
		TimeWatching += DeltaSeconds;
		PlayerLucidity->AddLucidity(WatchLucidityPerSecond * DeltaSeconds);

		// The corner only yields its point once the gaze is held, at most once
		// per manifest interval.
		if (!bManifestationActive && TimeWatching >= 1.2f && TimeSinceLastManifest >= ManifestInterval && !(IsValid(CornerPoint) && CornerPoint->IsVisible()))
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
	const float Recognition = PlayerRecognition ? PlayerRecognition->GetRecognition() : 0.f;
	TArray<UTriangleManifestationDefinition*, TInlineAllocator<16>> Candidates;
	TArray<float, TInlineAllocator<16>> Weights;
	float TotalWeight = 0.f;

	for (TObjectPtr<UTriangleManifestationDefinition> DefObject : ManifestationPool)
	{
		UTriangleManifestationDefinition* Def = DefObject.Get();
		if (!Def || Recognition < Def->MinimumRecognition)
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
		if (const float* Remaining = ManifestCooldowns.Find(Def->ManifestId); Remaining && *Remaining > 0.f)
		{
			continue;
		}

		const float Weight = FMath::IsFinite(Def->SelectionWeight) ? FMath::Max(0.f, Def->SelectionWeight) : 0.f;
		Candidates.Add(Def);
		Weights.Add(Weight);
		TotalWeight += Weight;
	}

	if (Candidates.IsEmpty() || TotalWeight <= UE_KINDA_SMALL_NUMBER)
	{
		return;
	}

	float Pick = FMath::FRandRange(0.f, TotalWeight);
	int32 SelectedIndex = INDEX_NONE;
	for (int32 Index = 0; Index < Candidates.Num(); ++Index)
	{
		Pick -= Weights[Index];
		if (Pick <= 0.f)
		{
			SelectedIndex = Index;
			break;
		}
	}
	if (SelectedIndex == INDEX_NONE)
	{
		SelectedIndex = Candidates.Num() - 1;
	}

	UTriangleManifestationDefinition* Def = Candidates[SelectedIndex];
	const float Duration = FMath::Max(2.5f, Def->GetRandomDuration());
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ManifestationTimerHandle);
	}
	++ActiveManifestationSerial;
	const int32 ManifestationSerial = ActiveManifestationSerial;
	ActiveAnomaly.Reset();
	ActiveManifestationDefinition = Def;
	bCurrentAnomalyWasWitnessed = false;
	bManifestationActive = true;

	bool bStarted = false;
	if (Def->Kind == ETriangleManifestationKind::Point && IsValid(CornerPoint))
	{
		CornerPoint->Manifest(Duration);
		bStarted = true;
		UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s) as corner point for %.1fs."), *Def->ManifestId.ToString(), Duration);
	}
	else if (Def->Kind == ETriangleManifestationKind::HumOnly)
	{
		bStarted = true;
		UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s): hum only, nothing to see."), *Def->ManifestId.ToString(), Duration);
	}
	else if (AnomalySys)
	{
		const FVector Around = IsValid(PlayerActor) ? PlayerActor->GetActorLocation() : CornerLocation;
		ActiveAnomaly = AnomalySys->SpawnAnomaly(Def->AnomalyKind, Duration, Around);
		bStarted = ActiveAnomaly.IsValid();
		if (bStarted)
		{
			OnDreamBleed.Broadcast();
			UE_LOG(LogTriangleDream, Log, TEXT("Manifest (%s) cut into the dream for %.1fs."), *Def->ManifestId.ToString(), Duration);
		}
	}

	if (!bStarted)
	{
		bManifestationActive = false;
		bHumEnabledForCurrentManifestation = false;
		ActiveAnomaly.Reset();
		ActiveManifestationDefinition = nullptr;
		return;
	}

	const float SafeCooldown = FMath::IsFinite(Def->CooldownSeconds) ? FMath::Max(0.5f, Def->CooldownSeconds) : 14.f;
	ManifestCooldowns.FindOrAdd(Def->ManifestId) = SafeCooldown;
	const bool bHumWasActive = bHumActive;
	bHumEnabledForCurrentManifestation = Def->bUseHum;
	if (!bHumEnabledForCurrentManifestation && bHumWasActive)
	{
		bHumActive = false;
		OnTriangleHumStop.Broadcast();
	}
	CurrentHumTarget = bHumEnabledForCurrentManifestation
		? (FMath::IsFinite(Def->HumPresence) ? FMath::Clamp(Def->HumPresence, 0.f, 1.f) : 0.f)
		: 0.f;
	OnTriangleManifestation.Broadcast(Def->ManifestId);
	ResolveManifestation(Def, Duration, ManifestationSerial);
}

void ATriangleDreamDirector::ResolveManifestation(const UTriangleManifestationDefinition* Def, const float Duration, const int32 ManifestationSerial)
{
	if (!Def)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float ConfiguredGain = Def->RecognitionGainOnSight > 0.f
		? Def->RecognitionGainOnSight
		: (PlayerRecognition ? PlayerRecognition->BaseGainPerSighting : 0.f);
	const float Gain = FMath::IsFinite(ConfiguredGain) ? FMath::Max(0.f, ConfiguredGain) : 0.f;
	const FName ManifestId = Def->ManifestId;
	TWeakObjectPtr<ATriangleDreamDirector> WeakThis(this);
	World->GetTimerManager().SetTimer(ManifestationTimerHandle, [WeakThis, Def, Gain, ManifestId, ManifestationSerial]()
	{
		ATriangleDreamDirector* Director = WeakThis.Get();
		if (!Director || Director->ActiveManifestationSerial != ManifestationSerial)
		{
			return;
		}

		if (IsValid(Director->PlayerRecognition))
		{
			float GainToApply = 0.f;
			if (Def->Kind == ETriangleManifestationKind::HumOnly)
			{
				// An unexplained hum teaches only a fragment until the player can
				// reliably attribute it to the Triangle.
				GainToApply = Director->PlayerRecognition->CanSensePresenceBeforeVisual()
					? Gain
					: Gain * 0.2f;
			}
			else if (Director->WasManifestationWitnessed(Def))
			{
				GainToApply = Gain;
			}

			if (GainToApply > 0.f)
			{
				Director->PlayerRecognition->AddRecognition(GainToApply, ManifestId);
			}
		}

		Director->bManifestationActive = false;
		Director->bHumEnabledForCurrentManifestation = false;
		Director->ActiveAnomaly.Reset();
		Director->ActiveManifestationDefinition = nullptr;
	}, Duration, false);
}

bool ATriangleDreamDirector::WasManifestationWitnessed(const UTriangleManifestationDefinition* Def) const
{
	if (!Def || !IsValid(PlayerActor))
	{
		return false;
	}

	UCameraComponent* Camera = PlayerActor->GetFirstPersonCamera();
	if (!Camera)
	{
		return false;
	}
	if (Def->Kind == ETriangleManifestationKind::HumOnly)
	{
		return PlayerRecognition && PlayerRecognition->CanSensePresenceBeforeVisual();
	}

	FVector Target = FVector::ZeroVector;
	if (Def->Kind == ETriangleManifestationKind::Point)
	{
		if (!IsValid(CornerPoint) || !CornerPoint->IsVisible())
		{
			return false;
		}
		Target = CornerPoint->GetActorLocation();
	}
	else
	{
		const ATriangleAnomalyActor* Anomaly = ActiveAnomaly.Get();
		if (!Anomaly)
		{
			// A spawned anomaly that naturally expired exactly at resolution was
			// present for the full manifestation window, but it only counts if
			// the camera actually witnessed it while it was alive.
			return bCurrentAnomalyWasWitnessed;
		}
		if (!Anomaly->IsActive())
		{
			return bCurrentAnomalyWasWitnessed;
		}
		Target = Anomaly->GetActorLocation();
	}

	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector ToTarget = Target - CameraLocation;
	const float Distance = ToTarget.Size();
	if (Distance <= 1.f || Distance > 4000.f)
	{
		return false;
	}
	if (FVector::DotProduct(Camera->GetForwardVector(), ToTarget / Distance) < 0.35f)
	{
		return false;
	}

	FHitResult Hit;
	const FVector TraceEnd = Target - ToTarget.GetSafeNormal() * 8.f;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TriangleManifestationWitness), false);
	QueryParams.AddIgnoredActor(PlayerActor);
	const bool bBlocked = GetWorld() && GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams);
	return !bBlocked || Hit.Distance >= Distance - 16.f;
}

void ATriangleDreamDirector::UpdatePresence(float DeltaSeconds)
{
	const bool bCornerVisible = IsValid(CornerPoint) && CornerPoint->IsVisible();
	const bool bWantsPresence = bManifestationActive || bCornerVisible;

	if (bWantsPresence)
	{
		TrianglePresence = FMath::Min(1.f, TrianglePresence + PresenceRampRate * DeltaSeconds);
	}
	else
	{
		TrianglePresence = FMath::Max(0.f, TrianglePresence - PresenceDecayRate * DeltaSeconds);
	}

	const bool bHasTransientHum = TransientHumRemaining > 0.f;
	const bool bWantsHum = bWantsPresence || bHasTransientHum;
	if (bWantsHum)
	{
		CurrentHumTarget = 0.f;
		if (bWantsPresence && bHumEnabledForCurrentManifestation)
		{
			CurrentHumTarget = FMath::Max(CurrentHumTarget, TrianglePresence * 0.65f);
		}
		if (bHasTransientHum)
		{
			CurrentHumTarget = FMath::Max(CurrentHumTarget, TransientHumTarget);
		}
	}
	else
	{
		CurrentHumTarget = 0.f;
	}

	if (TransientHumRemaining > 0.f)
	{
		TransientHumRemaining = FMath::Max(0.f, TransientHumRemaining - DeltaSeconds);
		if (TransientHumRemaining <= 0.f)
		{
			TransientHumTarget = 0.f;
		}
	}

	if (AudioSys)
	{
		AudioSys->SetHumPresence(CurrentHumTarget, HumFadeSeconds);
	}

	// Presence events describe the world intrusion, independently of a brief
	// motif hum that may be audible without a visible manifestation.
	if (!bPresenceActive && TrianglePresence >= 0.05f)
	{
		bPresenceActive = true;
		LastAnnouncedPresence = TrianglePresence;
		OnPresenceStart.Broadcast(TrianglePresence);
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
			OnPresenceDisappear.Broadcast();
		}
	}

	const bool bShouldHum = bWantsHum && CurrentHumTarget > 0.01f;
	if (bShouldHum && !bHumActive)
	{
		bHumActive = true;
		OnTriangleHumStart.Broadcast();
	}
	else if (!bShouldHum && bHumActive)
	{
		bHumActive = false;
		OnTriangleHumStop.Broadcast();
	}
}

void ATriangleDreamDirector::RequestTransientHum(const float Target, const float Duration)
{
	TransientHumTarget = FMath::IsFinite(Target) ? FMath::Clamp(Target, 0.f, 1.f) : 0.f;
	TransientHumRemaining = FMath::IsFinite(Duration) ? FMath::Max(0.f, Duration) : 0.f;
}

void ATriangleDreamDirector::TryMasterSense(float DeltaSeconds)
{
	(void)DeltaSeconds;
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
	switch (NewLevel)
	{
	case ETriangleRecognitionLevel::Noticing:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("You have seen that shape before. Not remembered - seen.")), 3.5f);
		break;
	case ETriangleRecognitionLevel::Recognizing:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("You know what is here before you look.")), 3.5f);
		if (MotifSys)
		{
			MotifSys->RequestMotif(TEXT("ClockStops12"));
		}
		break;
	case ETriangleRecognitionLevel::Anticipating:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("The hum is not in the room. It is listening with you.")), 4.f);
		RequestTransientHum(0.35f, 2.f);
		// Do not announce Platform 12 before the station exists. The motif is
		// requested when the player actually arrives there.
		if (MotifSys && IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ATriangleTrain::StaticClass())))
		{
			MotifSys->RequestMotif(TEXT("Platform12Train"));
		}
		break;
	case ETriangleRecognitionLevel::Master:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("It is here. It has always been here.")), 4.f);
		break;
	default:
		break;
	}
	OnTriangleRecognition.Broadcast();
}

void ATriangleDreamDirector::TriggerTriangleManifest(float Duration)
{
	if (!IsValid(CornerPoint) || ManifestationPool.IsEmpty() || !ManifestationPool[0])
	{
		return;
	}

	UTriangleManifestationDefinition* Def = ManifestationPool[0].Get();
	const float SafeDuration = FMath::IsFinite(Duration) ? FMath::Max(0.25f, Duration) : 1.f;
	if (bManifestationActive)
	{
		if (AnomalySys)
		{
			AnomalySys->DespawnAll();
		}
		if (IsValid(CornerPoint))
		{
			CornerPoint->Hide();
		}
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ManifestationTimerHandle);
	}
	++ActiveManifestationSerial;
	const int32 ManifestationSerial = ActiveManifestationSerial;
	CornerPoint->Manifest(SafeDuration);
	ActiveAnomaly.Reset();
	ActiveManifestationDefinition = Def;
	bCurrentAnomalyWasWitnessed = false;
	bManifestationActive = true;
	const bool bHumWasActive = bHumActive;
	bHumEnabledForCurrentManifestation = Def->bUseHum;
	if (!bHumEnabledForCurrentManifestation && bHumWasActive)
	{
		bHumActive = false;
		OnTriangleHumStop.Broadcast();
	}
	CurrentHumTarget = bHumEnabledForCurrentManifestation
		? (FMath::IsFinite(Def->HumPresence) ? FMath::Clamp(Def->HumPresence, 0.f, 1.f) : 0.f)
		: 0.f;
	OnTriangleManifestation.Broadcast(Def ? Def->ManifestId : FName(TEXT("UpperCornerPoint")));
	if (Def)
	{
		const float SafeCooldown = FMath::IsFinite(Def->CooldownSeconds) ? FMath::Max(0.5f, Def->CooldownSeconds) : 14.f;
		ManifestCooldowns.FindOrAdd(Def->ManifestId) = SafeCooldown;
		ResolveManifestation(Def, SafeDuration, ManifestationSerial);
	}
	UE_LOG(LogTriangleDream, Log, TEXT("Triangle manifest triggered (lucidity=%.1f)."), PlayerLucidity ? PlayerLucidity->GetLucidity() : -1.f);
}

void ATriangleDreamDirector::WithdrawPresence()
{
	++ActiveManifestationSerial;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ManifestationTimerHandle);
		World->GetTimerManager().ClearTimer(TrainDoorResetTimerHandle);
	}

	const bool bHadPresence = bPresenceActive;
	const bool bHadHum = bHumActive;
	bManifestationActive = false;
	bHumEnabledForCurrentManifestation = false;
	bPresenceActive = false;
	bHumActive = false;
	TrianglePresence = 0.f;
	LastAnnouncedPresence = 0.f;
	TransientHumTarget = 0.f;
	TransientHumRemaining = 0.f;
	CurrentHumTarget = 0.f;
	ManifestCooldowns.Reset();
	TimeWatching = 0.f;
	TimeSinceLastManifest = ManifestInterval;
	TimeSinceMasterSense = 0.f;
	ActiveAnomaly.Reset();
	ActiveManifestationDefinition = nullptr;
	bCurrentAnomalyWasWitnessed = false;
	bMaximumPointReapplied = false;

	if (AudioSys)
	{
		AudioSys->SetHumPresence(0.f, 0.8f);
	}
	if (AnomalySys)
	{
		AnomalySys->DespawnAll();
	}
	if (UWorld* World = GetWorld())
	{
		if (ATriangleTrain* Train = UGameplayStatics::GetActorOfClass(World, ATriangleTrain::StaticClass()); IsValid(Train))
		{
			Train->SetDoorsOpen(false);
		}
	}
	if (IsValid(CornerPoint))
	{
		CornerPoint->Hide();
	}
	if (bHadHum)
	{
		OnTriangleHumStop.Broadcast();
	}
	if (bHadPresence)
	{
		OnPresenceDisappear.Broadcast();
	}
	UE_LOG(LogTriangleDream, Log, TEXT("All presence withdrawn."));
}

void ATriangleDreamDirector::HandleEnvironmentChanged()
{
	// Environment changes use the same complete withdrawal path as a wake:
	// anomalies are world-owned, so builder teardown alone cannot remove them.
	WithdrawPresence();
	CornerPoint = nullptr;
	CornerLocation = FVector::ZeroVector;
	bMaximumPointReapplied = false;

	// If the player reaches the station after already anticipating the motif,
	// deliver the station beat at the point where its target actually exists.
	if (MotifSys && IsValid(PlayerRecognition) && PlayerRecognition->CanAttemptWake()
		&& IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ATriangleTrain::StaticClass())))
	{
		MotifSys->RequestMotif(TEXT("Platform12Train"));
	}
}

void ATriangleDreamDirector::ResetForSaveLoad()
{
	WithdrawPresence();
	// Loading is state synchronisation, not a new progression milestone. The
	// next acquire pass will reapply the maximum point if the restored value
	// warrants it, without replaying the original story event.
	bMaxReachedHandled = false;
	bMaximumPointReapplied = false;
}

void ATriangleDreamDirector::HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel)
{
	switch (NewLevel)
	{
	case ETriangleLucidityLevel::Medium:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("Something in this room is too familiar.")), 3.f);
		break;

	case ETriangleLucidityLevel::High:
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("You remember this place. You have always been here.")), 3.5f);
		break;

	case ETriangleLucidityLevel::Maximum:
		if (!bMaxReachedHandled)
		{
			if (!IsValid(CornerPoint))
			{
				// The pawn can cross an environment boundary while the level
				// event is in flight. Let the next acquire pass reapply the
				// maximum point instead of consuming the one-shot flag.
				bMaximumPointReapplied = false;
				break;
			}
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(ManifestationTimerHandle);
			}
			if (AnomalySys)
			{
				AnomalySys->DespawnAll();
			}
			ActiveAnomaly.Reset();
			++ActiveManifestationSerial;
			bMaxReachedHandled = true;
			bMaximumPointReapplied = true;
			TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("The dream knows you are awake.")), 4.f);
			// The point stops hiding at full lucidity.
			if (IsValid(CornerPoint))
			{
				CornerPoint->Manifest(9999.f);
			}
			bManifestationActive = true;
			bHumEnabledForCurrentManifestation = true;
			CurrentHumTarget = FMath::Max(CurrentHumTarget, 0.65f);
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
	if (MotifId == TEXT("ClockStops12"))
	{
		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("The clock stops at twelve.")), 3.5f);
		if (PlayerRecognition)
		{
			PlayerRecognition->AddRecognition(PlayerRecognition->BaseGainPerMotif, TEXT("ClockStops12"));
		}
	}
	else if (MotifId == TEXT("Platform12Train"))
	{
		ATriangleTrain* Train = UGameplayStatics::GetActorOfClass(GetWorld(), ATriangleTrain::StaticClass());
		if (!IsValid(Train))
		{
			// Defer the motif instead of consuming a station beat in the bedroom.
			if (MotifSys)
			{
				MotifSys->QueueAutomaticMotif(MotifId);
			}
			return;
		}

		TriangleDirectorPrivate::PushSubtitle(GetWorld(), FText::FromString(TEXT("Somewhere far off, a train pulls into platform twelve.")), 4.f);
		if (PlayerRecognition)
		{
			PlayerRecognition->AddRecognition(PlayerRecognition->BaseGainPerMotif * 1.5f, TEXT("Platform12Train"));
		}
		// A distant horn condensed into the hum, independent of the presence
		// ramp so the motif remains audible even when nothing is visible.
		RequestTransientHum(0.5f, 4.f);
		Train->SetDoorsOpen(true);
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TrainDoorResetTimerHandle);
			TWeakObjectPtr<ATriangleDreamDirector> WeakThis(this);
			World->GetTimerManager().SetTimer(TrainDoorResetTimerHandle, [WeakThis]()
			{
				if (ATriangleDreamDirector* Director = WeakThis.Get())
				{
					if (ATriangleTrain* CurrentTrain = UGameplayStatics::GetActorOfClass(Director->GetWorld(), ATriangleTrain::StaticClass()); IsValid(CurrentTrain))
					{
						CurrentTrain->SetDoorsOpen(false);
					}
				}
			}, 8.f, false);
		}
	}
}

void ATriangleDreamDirector::HandleWakeCycle(int32 LayerIndex)
{
	UE_LOG(LogTriangleDream, Log, TEXT("False wake layer %d."), LayerIndex);
	OnFalseAwakening.Broadcast(FName(*FString::Printf(TEXT("Wake_%d"), LayerIndex)));
}