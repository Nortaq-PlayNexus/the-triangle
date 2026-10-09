#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ULTriangleRecognitionComponent.generated.h"

UENUM(BlueprintType)
enum class ETriangleRecognitionLevel : uint8
{
	Unknowing,
	Noticing,
	Recognizing,
	Anticipating,
	Master
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleRecognitionLevelChanged, ETriangleRecognitionLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleRecognitionChanged, float, Recognition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleRecognitionSensed, FName, Reason);

/**
 * The hidden Recognition stat - deliberately separate from Lucidity.
 *
 * Lucidity is "I know I am dreaming." Recognition is "I know what is here."
 * The Triangle is an intrusion the player learns to recognise before they can
 * understand it: recognition climbs through observing the same pattern in
 * different bodies - the corner, a gap in the wallpaper, a clock stopped at
 * twelve - until the player can sense its presence before anything is visible.
 *
 *  - Unknowing    (0..25)   : strangeness passes without note.
 *  - Noticing     (25..50)  : the same shape keeps returning; it feels arranged.
 *  - Recognizing  (50..75)  : the pattern is recognised before it completes.
 *  - Anticipating (75..95)  : the presence is felt behind surfaces; the hum has meaning.
 *  - Master       (95..100) : "it's here" - known without seeing.
 *
 * Like Lucidity, Recognition only advances through explicit dream events
 * (witnessed manifestations, recurring motifs, sensed absences) - never by
 * existing.
 */
UCLASS(ClassGroup = (Triangle), meta = (BlueprintSpawnableComponent))
class ULTriangleRecognitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULTriangleRecognitionComponent();

	/** Current recognition, clamped to 0..100. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Recognition", meta = (ClampMin = "0", ClampMax = "100"))
	float Recognition = 0.f;

	/** Recognition gained from witnessing one manifestation resolve. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Recognition", meta = (ClampMin = "0", ClampMax = "30"))
	float BaseGainPerSighting = 6.f;

	/** Recognition gained from a recurring motif (clock, train horn, ...). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Recognition", meta = (ClampMin = "0", ClampMax = "30"))
	float BaseGainPerMotif = 2.f;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Recognition")
	FTriangleRecognitionLevelChanged OnRecognitionLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Recognition")
	FTriangleRecognitionChanged OnRecognitionChanged;

	/** Broadcast when the player senses presence before a visible form ("it's here"). */
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Recognition")
	FTriangleRecognitionSensed OnSensePresence;

	UFUNCTION(BlueprintCallable, Category = "Triangle|Recognition")
	void AddRecognition(float Amount, FName Reason = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Triangle|Recognition")
	void SetRecognition(float NewValue);

	/** Records a witnessed manifestation by id (deduplicates the encounter log). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Recognition")
	void RecordSighting(FName ManifestId);

	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	float GetRecognition() const { return Recognition; }

	/** Returns a defensive copy of the patterns the player has already recognised. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	TArray<FName> GetRecognizedPatternIds() const { return RecognisedIds; }

	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	ETriangleRecognitionLevel GetRecognitionLevel() const;

	/** Player can recognise the Triangle behind a surface before any visual. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	bool CanSensePresenceBeforeVisual() const { return GetRecognitionLevel() >= ETriangleRecognitionLevel::Anticipating; }

	/** Player can know "it's here" with no visible manifestation at all. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	bool HasMasterSense() const { return GetRecognitionLevel() >= ETriangleRecognitionLevel::Master; }

	/** Whether this recognition allows a wake attempt that can resolve. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	bool CanAttemptWake() const { return GetRecognitionLevel() >= ETriangleRecognitionLevel::Anticipating; }

	/** Whether the given motif has already been recognised (recorded as a sighting). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Recognition")
	bool HasRecognised(FName ManifestId) const { return RecognisedIds.Contains(ManifestId); }

	/** Legacy scalar-only restore; retains the current pattern catalogue. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Recognition")
	void RestoreFromSave(float SavedRecognition);

	/**
	 * Restores the scalar and pattern IDs without replaying recognition milestones
	 * or the pre-visual sense. The value-only delegate refreshes UI immediately.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Recognition")
	void RestoreFromSaveWithCatalogue(float SavedRecognition, const TArray<FName>& SavedRecognizedPatternIds);

private:
	static ETriangleRecognitionLevel LevelFor(float Value);

	/** The pattern catalogue: ids the player has already watched resolve. */
	UPROPERTY()
	TArray<FName> RecognisedIds;
};