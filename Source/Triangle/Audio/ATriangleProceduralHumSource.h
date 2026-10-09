#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleProceduralHumSource.generated.h"

class UAudioComponent;
class USoundWaveProcedural;

/**
 * The Hum - the Triangle's signature sound.
 *
 * A persistent, quietly wrong low drone generated entirely at runtime (two
 * detuned sine partials breathing against each other), so the signature sound
 * works with zero audio content. It begins imperceptibly, swells when The
 * Triangle is present, and occasionally vanishes for a beat before returning.
 *
 * The Dream Director (through the audio subsystem) drives only a 0..1 presence
 * target; the attack/release of the envelope does the rest.
 */
UCLASS()
class ATriangleProceduralHumSource : public AActor
{
	GENERATED_BODY()

public:
	ATriangleProceduralHumSource();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Sets the presence target (0..1). Fading honours FadeTimeSeconds. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void SetPresence(float Target01, float FadeTimeSeconds = 1.f);

	/** Whether the hum is currently audible enough to matter. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Audio")
	bool IsHumming() const { return CurrentEnvelope > 0.05f; }

	/** Current smoothed presence (0..1). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Audio")
	float GetHumPresence() const { return CurrentEnvelope; }

private:
	/** Synthesizes one block of int16 PCM and queues it into the procedural wave. */
	void FeedAudio();

	/** Tells the audio component to be playing or silent. */
	void UpdatePlaybackState(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> HumComponent;

	UPROPERTY()
	TObjectPtr<USoundWaveProcedural> HumWave;

	float TargetPresence = 0.f;
	float CurrentEnvelope = 0.f;
	float AttackCooldown = 0.f;
	float FadeSeconds = 1.f;

	double PhaseA = 0.0;
	double PhaseB = 0.0;
	double SlowLfoPhase = 0.0;
	double DetunePhase = 0.0;

	FTimerHandle FeedTimerHandle;

	/** Reused PCM staging buffer; QueueAudio copies before the timer returns. */
	TArray<int16> SynthBuffer;
};