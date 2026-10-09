#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ULTriangleLucidityComponent.generated.h"

UENUM(BlueprintType)
enum class ETriangleLucidityLevel : uint8
{
	Low,
	Medium,
	High,
	Maximum
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleLucidityLevelChanged, ETriangleLucidityLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleLucidityChanged, float, Lucidity);

/**
 * The hidden/visible Lucidity meter.
 *
 * Lucidity represents how aware the player is that they are dreaming.
 *
 *  - Low      (0..30)      : the dream is accepted; impossible things pass by.
 *  - Medium   (30..60)     : repeated places get noticed; doors misbehave.
 *  - High     (60..90)     : the player can manipulate dream elements.
 *  - Maximum  (90..100)    : dream rules break; The Triangle draws near.
 *
 * Lucidity is only ever raised through explicit dream events (a watched
 * object, an interaction, a realization) - never by merely existing, which
 * keeps it a progression tool rather than a passive timer.
 */
UCLASS(ClassGroup = (Triangle), meta = (BlueprintSpawnableComponent))
class ULTriangleLucidityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULTriangleLucidityComponent();

	/** Current lucidity, clamped to 0..100. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Lucidity", meta = (ClampMin = "0", ClampMax = "100"))
	float Lucidity = 0.f;

	/** Amount of lucidity a meaningful realization provides. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Lucidity", meta = (ClampMin = "0", ClampMax = "100"))
	float BaseGainPerRealization = 8.f;

	DECLARE_EVENT(ULTriangleLucidityComponent, FTriangleLucidityMaxEvent);
	/** Fired once when lucidity first reaches 100. */
	FTriangleLucidityMaxEvent& OnLucidityMax() { return LucidityMaxEvent; }

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Lucidity")
	FTriangleLucidityLevelChanged OnLucidityLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Lucidity")
	FTriangleLucidityChanged OnLucidityChanged;

	UFUNCTION(BlueprintCallable, Category = "Triangle|Lucidity")
	void AddLucidity(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Triangle|Lucidity")
	void SetLucidity(float NewValue);

	UFUNCTION(BlueprintPure, Category = "Triangle|Lucidity")
	float GetLucidity() const { return Lucidity; }

	UFUNCTION(BlueprintPure, Category = "Triangle|Lucidity")
	ETriangleLucidityLevel GetLucidityLevel() const;

	/** Whether the player is lucid enough to deliberately break dream rules. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Lucidity")
	bool CanManipulateDream() const { return GetLucidityLevel() >= ETriangleLucidityLevel::High; }

	/**
	 * Restores a saved value without replaying level or maximum-lucidity milestones.
	 * The value-only delegate is still broadcast so meters can refresh immediately.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Lucidity")
	void RestoreFromSave(float SavedLucidity);

private:
	FTriangleLucidityMaxEvent LucidityMaxEvent;
	bool bMaxReached = false;

	static ETriangleLucidityLevel LevelFor(float Value);
};