#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ATriangleHUD.generated.h"

class APawn;
class UTriangleHUDWidget;

/**
 * Thin HUD actor: creates the C++ HUD widget on possession and safely
 * forwards prompt, subtitle, meter, and dream-transition requests to it.
 */
UCLASS()
class ATriangleHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATriangleHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	/** Rebinds stat delegates to the current pawn; safe during possession changes. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void BindToPlayer(APawn* Player);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void UnbindFromPlayer();

	UFUNCTION(BlueprintPure, Category = "Triangle|HUD")
	bool IsBoundToPlayer(APawn* Player) const;

	UFUNCTION(BlueprintPure, Category = "Triangle|HUD")
	UTriangleHUDWidget* GetHUDWidget() const;

private:
	UTriangleHUDWidget* EnsureHUDWidget();
	void TryBindToPlayer();

	UPROPERTY()
	TObjectPtr<UTriangleHUDWidget> HUDWidget;

	TWeakObjectPtr<APawn> BoundPlayer;
};
