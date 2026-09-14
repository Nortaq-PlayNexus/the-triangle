#include "Audio/ATriangleProceduralHumSource.h"

#include "Triangle.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Sound/SoundWaveProcedural.h"

namespace TriangleHum
{
	constexpr float SynthTickInterval = 0.06f;
	constexpr float FreqA = 43.2f;   // a sleeping-room vibration, not quite a note
	constexpr float FreqB = 46.7f;
	constexpr float FallbackRate = 44100;
	constexpr int32 FallbackChannels = 1;
}

ATriangleProceduralHumSource::ATriangleProceduralHumSource()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ATriangleProceduralHumSource::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// One procedural wave, generated and streamed entirely at runtime.
	HumWave = NewObject<USoundWaveProcedural>(this);
	if (!HumWave)
	{
		UE_LOG(LogTriangle, Error, TEXT("Could not create the procedural hum wave - the dream stays silent."));
		return;
	}
	HumWave->Duration = 300.f;

	HumComponent = NewObject<UAudioComponent>(this);
	HumComponent->SetSound(HumWave);
	HumComponent->bIsUISound = true; // undirectional: the hum is inside the ears, not in the room
	HumComponent->bAutoActivate = false;
	HumComponent->SetVolumeMultiplier(0.f);
	HumComponent->RegisterComponent();

	World->GetTimerManager().SetTimer(FeedTimerHandle, this, &ATriangleProceduralHumSource::FeedAudio, TriangleHum::SynthTickInterval, true);

	UE_LOG(LogTriangleDream, Log, TEXT("The hum source is live."));
}

void ATriangleProceduralHumSource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FeedTimerHandle);
	}
	if (HumComponent)
	{
		HumComponent->Stop();
	}
	Super::EndPlay(EndPlayReason);
}

void ATriangleProceduralHumSource::SetPresence(float Target01, float FadeTimeSeconds)
{
	TargetPresence = FMath::Clamp(Target01, 0.f, 1.f);
	FadeSeconds = FMath::Max(0.05f, FadeTimeSeconds);
}

void ATriangleProceduralHumSource::UpdatePlaybackState(float DeltaSeconds)
{
	const float Step = FMath::Min(1.f, DeltaSeconds / FadeSeconds);
	CurrentEnvelope += (TargetPresence - CurrentEnvelope) * Step;
	CurrentEnvelope = FMath::Clamp(CurrentEnvelope, 0.f, 1.f);

	if (!HumComponent)
	{
		return;
	}

	if (CurrentEnvelope > 0.01f)
	{
		if (!HumComponent->IsPlaying())
		{
			HumWave->ResetAudio();
			HumComponent->Play();
		}
		HumComponent->SetVolumeMultiplier(FMath::Sqrt(CurrentEnvelope) * 0.55f);
	}
	else if (HumComponent->IsPlaying())
	{
		HumComponent->Stop();
		HumWave->ResetAudio();
	}
}

void ATriangleProceduralHumSource::FeedAudio()
{
	UpdatePlaybackState(TriangleHum::SynthTickInterval);

	// Note: newer engine versions no longer expose GetSampleRate/GetNumChannels
	// on USoundWaveProcedural; the hum is synthesized at fixed settings anyway.
	const int32 Rate = TriangleHum::FallbackRate;
	const int32 Channels = TriangleHum::FallbackChannels;
	const int32 SamplesPerTick = FMath::Max(64, FMath::RoundToInt(Rate * TriangleHum::SynthTickInterval));

	TArray<int16> Buffer;
	Buffer.Reserve(SamplesPerTick * Channels);

	const double TwoPi = 2.0 * PI;

	// A beat: the two partials slow-dance in and out of phase, a low breathing
	// that the mind functions as "wrong" long before it consciously hears it.
	for (int32 i = 0; i < SamplesPerTick; ++i)
	{
		DetunePhase += TwoPi * 0.09 / Rate;
		const double FreqANow = TriangleHum::FreqA + FMath::Sin(DetunePhase) * 0.55;
		const double FreqBNow = TriangleHum::FreqB + FMath::Sin(DetunePhase + 1.7) * 0.55;

		PhaseA += TwoPi * FreqANow / Rate;
		PhaseB += TwoPi * FreqBNow / Rate;
		SlowLfoPhase += TwoPi * 0.17 / Rate;

		const double Breath = 0.72 + 0.28 * FMath::Sin(SlowLfoPhase);
		const double Sample = (FMath::Sin(PhaseA) + FMath::Sin(PhaseB)) * 0.5 * Breath * CurrentEnvelope * 0.60;

		const int16 Rounded = static_cast<int16>(FMath::Clamp(Sample * 32767.0, -32767.0, 32767.0));
		for (int32 c = 0; c < Channels; ++c)
		{
			Buffer.Add(Rounded);
		}
	}

	if (HumWave)
	{
		HumWave->QueueAudio(reinterpret_cast<uint8*>(Buffer.GetData()), Buffer.Num() * sizeof(int16));
	}
}