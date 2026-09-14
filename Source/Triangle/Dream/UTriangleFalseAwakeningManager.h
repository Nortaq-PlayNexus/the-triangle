#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UTriangleFalseAwakeningManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleFalseAwakeningEvent, int32, LayerIndex);
DECLARE_MULTICAST_DELEGATE(FTriangleWakeResetRequested);

/**
 * The wake - and why it never resolves.
 *
 * When the player attempts to wake (eyes closed, breath held), the dream
 * releases them for a moment... then keeps going. Each layer of waking is a
 * few differences deep: the clock stopped at twelve, the moon on the wrong
 * side, the hallway a little too long, the hum still there.
 *
 * This manager owns the layer counter and the lines of difference, tells the
 * Director when a wake has "resolved", and refuses wakes before the player's
 * recognition is high enough to deserve them.
 */
UCLASS()
class UTriangleFalseAwakeningManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Attempt a wake. Returns true if the dream let the player down (false = refuse). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Wake")
	bool TryWake();

	/** The current wake layer (0 within a dream: never woken). */
	UFUNCTION(BlueprintPure, Category = "Triangle|Wake")
	int32 GetCurrentLayer() const { return LayerIndex; }

	/** Lines of difference the wake produces, one per completed wake: first wakes are almost identical. */
	UPROPERTY(EditDefaultsOnly, Category = "Triangle|Wake")
	TArray<FText> DifferenceLines;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|Wake")
	FTriangleFalseAwakeningEvent OnFalseAwakening;

	/** Broadcast when a completed wake asks the Director to withdraw presence. */
	FTriangleWakeResetRequested OnWakeResetRequested;

private:
	int32 LayerIndex = 0;
	int32 CompletionCount = 0;

	static const TCHAR* DescriptorFor(int32 Layer);
};