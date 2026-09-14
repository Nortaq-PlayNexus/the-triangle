#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UTriangleInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriangleInteractionEvent, APawn*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriangleInteractionFocusEvent);

/**
 * Marks an Actor (or a component of it) as interactable.
 * The player's interaction finder locates these each frame, shows a prompt,
 * and fires the interaction event when the player presses interact.
 *
 * Attach one to a door, note, object, or invisible trigger. The owning actor's
 * transform (or DesiredFocalPoint, if set) is used as the interaction anchor.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class UTriangleInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTriangleInteractableComponent();

	/** Friendly name shown in the HUD prompt, e.g. "Notebook". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction")
	FText InteractionName;

	/** When false the interaction finder skips this component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction")
	bool bEnabled = true;

	/** Alternative anchor point; when set, its location is used for range checks. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction")
	TObjectPtr<USceneComponent> InteractionAnchor;

	/** How far an interactor may be (cm). Leave <= 0 to use the default finder range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|Interaction")
	float MaxInteractionRange = 0.f;

	/** Fired when the player presses interact while this is focused. */
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Interaction")
	FTriangleInteractionEvent OnInteracted;

	/** Fired when the finder starts focusing this interaction. */
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Interaction")
	FTriangleInteractionFocusEvent OnFocusBegin;

	/** Fired when the finder stops focusing this interaction. */
	UPROPERTY(BlueprintAssignable, Category = "Triangle|Interaction")
	FTriangleInteractionFocusEvent OnFocusEnd;

	/** The interactor presses the interact button. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Interaction")
	void Interact(APawn* Interactor);

	// --- Focusing ---------------------------------------------------------
	void NotifyFocusBegin();
	void NotifyFocusEnd();

	UFUNCTION(BlueprintCallable, Category = "Triangle|Interaction")
	FVector GetInteractionWorldLocation() const;
};