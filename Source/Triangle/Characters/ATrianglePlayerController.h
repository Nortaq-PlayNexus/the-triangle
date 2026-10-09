#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ATrianglePlayerController.generated.h"

class ATriangleHUD;

/**
 * Player controller for The Triangle.
 * Provides null-safe forwarding to the HUD so gameplay systems do not need to
 * dereference a HUD actor just to display a line of dialogue.
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

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityValue(float Value01);

	/** Toggles the lucidity meter visibility (used by accessibility/graphics options). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityMeterVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetRecognitionValue(float Value01);

	/** Toggles the recognition meter visibility (usually hidden). */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetRecognitionMeterVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void PushSubtitle(const FText& Text, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Accessibility")
	void SetSubtitleScale(float Scale);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Accessibility")
	void SetSubtitleOpacity(float Opacity);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void StartDreamTransition(float FadeOutDuration, float HoldDuration, float FadeInDuration, FLinearColor FadeColor);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void SetDreamFadeAlpha(float Alpha);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void StopDreamTransition();

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Pause")
	void SetPauseVisible(bool bVisible);

	/** Rebinds the HUD after possession or a local-player change. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void BindHUDToCurrentPawn();

	ATriangleHUD* GetTriangleHUD() const;
};
