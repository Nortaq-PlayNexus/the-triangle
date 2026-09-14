#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ATrianglePlayerController.generated.h"

class ATriangleHUD;

/**
 * Player controller for The Triangle.
 * Bridges the character's interaction focus to the HUD prompt.
 */
UCLASS()
class ATrianglePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATrianglePlayerController();

	/** Sets the text shown in the interaction prompt. Empty text hides the prompt. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetInteractionPrompt(const FText& Text, const FText& ActionName);

	/** Toggles the lucidity meter visibility (used by accessibility/graphics options). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityMeterVisible(bool bVisible);

	ATriangleHUD* GetTriangleHUD() const;
};