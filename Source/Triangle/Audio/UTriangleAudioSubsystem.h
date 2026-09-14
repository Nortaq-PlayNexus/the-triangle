#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UTriangleAudioSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class ATriangleProceduralHumSource;

/**
 * Structural audio routing for the dream.
 *
 * Owns a single persistent music/ambience bed, exposes named states with
 * volume tiers and crossfades (fade times are respected; the bed never snaps),
 * and routes one-shots to world locations. Sounds are supplied by the audio
 * content as it lands - if none is bound yet the subsystem degrades to an
 * honest silence instead of pretending to play something.
 */
UCLASS()
class UTriangleAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static const FName StateCalm;
	static const FName StateWatch;
	static const FName StateManifest;
	static const FName StateNearSilence;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Binds the persistent dream bed (loop); 2D, always audible. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void SetMasterMusic(USoundBase* Music);

	/** Crossfades the bed to the given named state volume tier. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void SetMusicState(FName State, float FadeTimeSeconds = 1.f);

	/** Drops everything toward silence - used during Triangle manifestations. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void SetNearSilence(bool bSilence, float FadeTimeSeconds = 1.f);

	/** Plays a one-shot at a world location with the given volume. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void PlayOneShot(USoundBase* Sound, const FVector& Location, float Volume = 1.f);

	/** Drives The Triangle's hum. Target01 is 0..1 presence; fade is honoured. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Audio")
	void SetHumPresence(float Target01, float FadeTimeSeconds = 1.f);

	/** Whether the hum is currently audible. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Audio")
	bool IsHumActive() const;

	/** Current smoothed hum presence (0..1). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Audio")
	float GetHumPresence() const;

private:
	void EnsureMusicHost();
	void EnsureHumSource();
	void FadeTo(float TargetVolume, float FadeTime);

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponent;
	UPROPERTY()
	TObjectPtr<AActor> MusicHost;

	UPROPERTY()
	TObjectPtr<USoundBase> BoundMusic;

	UPROPERTY()
	TObjectPtr<ATriangleProceduralHumSource> HumSource;

	float CurrentVolume = 0.4f;
	float MusicBaseVolume = 1.f;
	bool bSilenced = false;
	bool bWarnedNoMusic = false;
	TMap<FName, float> StateVolume;

	void ApplyStateVolume(FName State, float FadeTime);
};