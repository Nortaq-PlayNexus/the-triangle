#include "Audio/UTriangleAudioSubsystem.h"

#include "Triangle.h"
#include "Audio/ATriangleProceduralHumSource.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName UTriangleAudioSubsystem::StateCalm = TEXT("Calm");
const FName UTriangleAudioSubsystem::StateWatch = TEXT("Watch");
const FName UTriangleAudioSubsystem::StateManifest = TEXT("Manifest");
const FName UTriangleAudioSubsystem::StateNearSilence = TEXT("NearSilence");

void UTriangleAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	StateVolume.Add(StateCalm, 0.40f);
	StateVolume.Add(StateWatch, 0.22f);
	StateVolume.Add(StateManifest, 0.08f);
	StateVolume.Add(StateNearSilence, 0.f);

	CurrentVolume = StateVolume[StateCalm];
	CurrentState = StateCalm;
	bSilenced = false;
}

void UTriangleAudioSubsystem::Deinitialize()
{
	if (MusicComponent)
	{
		MusicComponent->Stop();
		MusicComponent = nullptr;
	}
	if (IsValid(HumSource))
	{
		HumSource->SetPresence(0.f, 0.05f);
		HumSource->Destroy();
	}
	HumSource = nullptr;
	MusicHost = nullptr;
	Super::Deinitialize();
}

void UTriangleAudioSubsystem::EnsureMusicHost()
{
	if (MusicHost)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	MusicHost = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity);
	if (!MusicHost)
	{
		UE_LOG(LogTriangle, Error, TEXT("Audio subsystem could not spawn its music host."));
		return;
	}
#if WITH_EDITOR
	MusicHost->SetActorLabel(TEXT("DreamMusicBed"));
#endif

	MusicComponent = NewObject<UAudioComponent>(MusicHost);
	MusicComponent->SetMobility(EComponentMobility::Movable);
	MusicHost->AddInstanceComponent(MusicComponent);
	MusicHost->SetRootComponent(MusicComponent);
	MusicHost->RegisterAllComponents();

	MusicComponent->Stop();
	MusicComponent->bAutoActivate = true;
	MusicComponent->bIsUISound = true; // keeps the dream bed audible from any camera
}

void UTriangleAudioSubsystem::SetMasterMusic(USoundBase* Music)
{
	BoundMusic = Music;

	if (!Music)
	{
		BoundMusic = nullptr;
		if (MusicComponent)
		{
			MusicComponent->Stop();
		}
		if (!bWarnedNoMusic)
		{
			UE_LOG(LogTriangle, Warning, TEXT("SetMasterMusic called with no sound asset - the dream runs silent until audio content lands."));
			bWarnedNoMusic = true;
		}
		return;
	}

	EnsureMusicHost();
	if (!MusicComponent)
	{
		return;
	}

	MusicComponent->SetSound(Music);
	const float StateTarget = StateVolume.Contains(CurrentState) ? StateVolume[CurrentState] : 0.f;
	const float TargetVolume = (bSilenced ? StateVolume[StateNearSilence] : StateTarget) * MusicBaseVolume;
	CurrentVolume = TargetVolume;
	MusicComponent->FadeIn(2.f, TargetVolume);
	UE_LOG(LogTriangle, Log, TEXT("Master dream bed bound to '%s'."), *Music->GetName());
}

void UTriangleAudioSubsystem::SetMusicState(FName State, float FadeTimeSeconds)
{
	if (!StateVolume.Contains(State))
	{
		UE_LOG(LogTriangle, Warning, TEXT("Ignoring unknown dream-bed state '%s'."), *State.ToString());
		return;
	}
	CurrentState = State;
	if (!BoundMusic)
	{
		return;
	}
	ApplyStateVolume(State, FadeTimeSeconds);
}

void UTriangleAudioSubsystem::SetNearSilence(bool bSilence, float FadeTimeSeconds)
{
	bSilenced = bSilence;
	if (!BoundMusic)
	{
		return;
	}
	ApplyStateVolume(bSilence ? StateNearSilence : CurrentState, FadeTimeSeconds);
}

void UTriangleAudioSubsystem::ApplyStateVolume(FName State, float FadeTime)
{
	float* Target = StateVolume.Find(State);
	FadeTo(bSilenced ? StateVolume[StateNearSilence] : (Target ? *Target : CurrentVolume), FadeTime);
}

void UTriangleAudioSubsystem::FadeTo(float TargetVolume, float FadeTime)
{
	if (!MusicComponent)
	{
		CurrentVolume = TargetVolume;
		return;
	}

	CurrentVolume = TargetVolume * MusicBaseVolume;
	const float SafeFadeTime = FMath::IsFinite(FadeTime) ? FMath::Max(0.05f, FadeTime) : 1.f;
	MusicComponent->AdjustVolume(SafeFadeTime, CurrentVolume, EAudioFaderCurve::Linear);
}

void UTriangleAudioSubsystem::PlayOneShot(USoundBase* Sound, const FVector& Location, float Volume)
{
	UWorld* World = GetWorld();
	if (!World || !Sound)
	{
		return;
	}
	UGameplayStatics::SpawnSoundAtLocation(World, Sound, Location, FRotator::ZeroRotator, Volume);
}

void UTriangleAudioSubsystem::EnsureHumSource()
{
	if (HumSource && IsValid(HumSource))
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	HumSource = World->SpawnActor<ATriangleProceduralHumSource>(ATriangleProceduralHumSource::StaticClass(), FTransform::Identity, Params);
}

void UTriangleAudioSubsystem::SetHumPresence(float Target01, float FadeTimeSeconds)
{
	// Hysteresis: tiny presences are not worth waking the hum for, matching the
	// design beat where the hum is sometimes simply not there.
	if (Target01 < 0.04f)
	{
		Target01 = 0.f;
	}

	EnsureHumSource();
	if (HumSource)
	{
		HumSource->SetPresence(Target01, FadeTimeSeconds);
	}
}

bool UTriangleAudioSubsystem::IsHumActive() const
{
	return HumSource && IsValid(HumSource) && HumSource->IsHumming();
}

float UTriangleAudioSubsystem::GetHumPresence() const
{
	return HumSource && IsValid(HumSource) ? HumSource->GetHumPresence() : 0.f;
}