#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTriangleHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * The in-game HUD, built entirely in C++ (no content assets required).
 * Shows interaction prompts, the lucidity meter and subtitle text.
 *
 * Subclasses can override this in Blueprints to restyle without touching logic.
 */
UCLASS()
class UTriangleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetInteractionPrompt(const FText& Text, const FText& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityValue(float Value01);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityMeterVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetRecognitionValue(float Value01);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetRecognitionMeterVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void PushSubtitle(const FText& Text, float Duration);

protected:
	virtual void NativeConstruct() override;

private:
	void EnsureChildWidgets();

	UPROPERTY()
	TObjectPtr<UTextBlock> InteractionName;
	UPROPERTY()
	TObjectPtr<UTextBlock> InteractionAction;
	UPROPERTY()
	TObjectPtr<UProgressBar> LucidityBar;
	UPROPERTY()
	TObjectPtr<UTextBlock> LucidityLabel;
	UPROPERTY()
	TObjectPtr<UProgressBar> RecognitionBar;
	UPROPERTY()
	TObjectPtr<UTextBlock> RecognitionLabel;
	UPROPERTY()
	TObjectPtr<UTextBlock> SubtitleText;

	/** Timer that clears the subtitle after its duration. */
	FTimerHandle SubtitleTimerHandle;
	float SubtitleRemaining = 0.f;
};