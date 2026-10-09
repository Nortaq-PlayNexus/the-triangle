#include "Audio/ATriangleProceduralHumSource.h"

#include "Triangle.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Sound/SoundWaveProcedural.h"

namespace TriangleHum
{
	constexpr float SynthTickInterval = 0.10f;
	constexpr float QueuedAudioSeconds = 0.35f;
	constexpr float FreqA = 43.2f;   // a sleeping-room vibration, not quite a note
	constexpr float FreqB = 46.7f;
	constexpr int32 SampleRate = 48000;
	constexpr int32 Channels = 1;
}

ATriangleProceduralHumSource::ATriangleProceduralHumSource()
{
	PrimaryActorTick.bCanEverTick = false;

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
	HumWave->NumChannels = TriangleHum::Channels;
	HumWave->SampleByteSize = sizeof(int16) * TriangleHum::Channels;
	HumWave->SetSampleRate(TriangleHum::SampleRate);
	HumWave->SetNumFrames(TriangleHum::SampleRate * 300);
	HumWave->Duration = 300.f;
	SynthBuffer.Reserve(TriangleHum::SampleRate * TriangleHum::QueuedAudioSeconds * TriangleHum::Channels);

	HumComponent = NewObject<UAudioComponent>(this);
	HumComponent->SetSound(HumWave);
	HumComponent->bIsUISound = true; // undirectional: the hum is inside the ears, not in the room
	HumComponent->bAutoActivate = false;
	HumComponent->bAutoDestroy = false;
	HumComponent->SetVolumeMultiplier(0.f);
	HumComponent->SetupAttachment(RootComponent);
	AddInstanceComponent(HumComponent);
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
	SynthBuffer.Reset();
	Super::EndPlay(EndPlayReason);
}

void ATriangleProceduralHumSource::SetPresence(float Target01, float FadeTimeSeconds)
{
	TargetPresence = FMath::IsFinite(Target01) ? FMath::Clamp(Target01, 0.f, 1.f) : 0.f;
	FadeSeconds = FMath::IsFinite(FadeTimeSeconds) ? FMath::Max(0.05f, FadeTimeSeconds) : 1.f;
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

	const int32 Rate = TriangleHum::SampleRate;
	const int32 Channels = TriangleHum::Channels;
	const int32 BytesPerSample = sizeof(int16) * Channels;
	const int32 LowWaterBytes = FMath::RoundToInt(Rate * QueuedAudioSeconds * 0.5f * BytesPerSample);
	if (CurrentEnvelope <= 0.001f || (HumWave && HumWave->GetAvailableAudioByteCount() >= LowWaterBytes))
	{
		return;
	}

	const int32 SamplesPerBlock = FMath::Max(256, FMath::RoundToInt(Rate * TriangleHum::QueuedAudioSeconds));
	SynthBuffer.Reset();
	SynthBuffer.Reserve(SamplesPerBlock * Channels);

	const double TwoPi = 2.0 * PI;

	// A beat: the two partials slow-dance in and out of phase, a low breathing
	// that the mind functions as "wrong" long before it consciously hears it.
	for (int32 i = 0; i < SamplesPerBlock; ++i)
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
			SynthBuffer.Add(Rounded);
		}
	}

	if (HumWave)
	{
		HumWave->QueueAudio(reinterpret_cast<uint8*>(SynthBuffer.GetData()), SynthBuffer.Num() * sizeof(int16));
	}
}