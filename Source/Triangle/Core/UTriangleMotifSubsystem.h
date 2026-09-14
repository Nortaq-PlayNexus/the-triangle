#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UTriangleMotifSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleMotifBroadcast, FName, MotifId);

/**
 * Recurring motif scheduler for the dream.
 *
 * A motif is a symbol The Triangle keeps repeating (a clock stopped at twelve,
 * a platform sign reading 12, a distant train horn, the hum resolving into a
 * question mark shape). This subsystem owns the timing: it registers motifs,
 * handles cooldowns, picks which one comes next and fires a broadcast the
 * Director maps to subtitles / hum / anomaly events.
 */
USTRUCT(BlueprintType)
struct FTriangleMotifConfig
{
	GENERATED_BODY()

	/** Minimum cooldown between occurrences of this motif (seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Motif")
	float MinCooldown = 25.f;

	/** Maximum cooldown: the next scheduled appearance lands within this window. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Motif")
	float MaxCooldown = 55.f;

	/** Minimum player recognition required for this motif to schedule. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Motif")
	float RecognitionRequirement = 0.f;
};

UCLASS()
class UTriangleMotifSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Registers a motif type with its scheduling config. */
	void RegisterMotif(FName Id, const FTriangleMotifConfig& Config);

	/** Turns on/off automatic scheduling for a motif (director calls this). */
	void SetMotifEnabled(FName Id, bool bEnabled);

	/** Enables the background clock that drives automatic scheduling. */
	void EnableAutomaticScheduling(bool bOn);

	/** Immediately forces this motif (bypasses cooldown). Returns false if not registered. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Motif")
	bool RequestMotif(FName Id);

	/** Schedules the next occurrence of this motif within its cooldown window. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Motif")
	void QueueAutomaticMotif(FName Id);

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Motif")
	FTriangleMotifBroadcast OnMotif;

	/** The dream's accumulated runtime clock (seconds since schedule was enabled). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Motif")
	float GetDreamClock() const { return DreamClock; }

private:
	FTriangleMotifConfig DefaultConfig;
	float DreamClock = 0.f;
	bool bSchedulingEnabled = false;

	struct FMotifEntry
	{
		FTriangleMotifConfig Config;
		float NextScheduledTime = 0.f;
		bool bEnabled = true;
	};
	TMap<FName, FMotifEntry> MotifTable;

	void ClockTick();
	FTimerHandle ClockTimerHandle;
};