#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "UTriangleHUDWidget.generated.h"

class APawn;
class UCanvasPanel;
class UBorder;
class UProgressBar;
class UTextBlock;

struct FTriangleQueuedSubtitle
{
	FText Text;
	float Duration = 2.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriangleDreamTransitionEvent);

/**
 * The in-game HUD, built entirely in C++ (no content assets required).
 * Shows interaction prompts, optional Lucidity/Recognition meters, subtitles,
 * and a full-screen transition veil for dream/environment changes.
 *
 * The widget binds to the owning pawn's stat components rather than polling
 * them.  This keeps the bars correct after save/load and avoids retaining a
 * destroyed player through a HUD delegate.
 */
UCLASS()
class UTriangleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Binds value and level delegates to the pawn's stat components. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void BindToPlayer(APawn* Player);

	/** Removes all stat delegates. Safe to call more than once. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void UnbindFromPlayer();

	UFUNCTION(BlueprintPure, Category = "Triangle|HUD")
	bool IsBoundToPlayer() const;

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

	/** Queues a subtitle so closely-spaced story beats are not overwritten. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void PushSubtitle(const FText& Text, float Duration);

	/** Runtime accessibility controls for the subtitle text. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Accessibility")
	void SetSubtitleScale(float Scale);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Accessibility")
	void SetSubtitleOpacity(float Opacity);

	/**
	 * Fades the screen to FadeColor, holds at full opacity, then fades back.
	 * The midpoint delegate is the integration point for swapping a dream
	 * environment while the screen is fully covered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void StartDreamTransition(float FadeOutDuration, float HoldDuration, float FadeInDuration, FLinearColor FadeColor);

	/** Allows a caller to drive the veil manually for a bespoke transition. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void SetDreamFadeAlpha(float Alpha);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Transition")
	void StopDreamTransition();

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD|Pause")
	void SetPauseVisible(bool bVisible);

	UFUNCTION(BlueprintPure, Category = "Triangle|HUD|Transition")
	bool IsDreamTransitionActive() const;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|HUD|Transition")
	FTriangleDreamTransitionEvent OnDreamTransitionMidpoint;

	UPROPERTY(BlueprintAssignable, Category = "Triangle|HUD|Transition")
	FTriangleDreamTransitionEvent OnDreamTransitionFinished;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	enum class EDreamTransitionPhase : uint8
	{
		None,
		FadingOut,
		Holding,
		FadingIn
	};

	void EnsureChildWidgets();
	void RefreshLucidityDisplay();
	void RefreshRecognitionDisplay();

	UFUNCTION()
	void HandleLucidityChanged(float Value);

	UFUNCTION()
	void HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel);

	UFUNCTION()
	void HandleRecognitionChanged(float Value);

	UFUNCTION()
	void HandleRecognitionLevelChanged(ETriangleRecognitionLevel NewLevel);

	void ShowNextSubtitle();
	void AdvanceSubtitle(float DeltaSeconds);
	void ClearSubtitleTimer();
	void ApplySubtitleAppearance();

	void AdvanceDreamTransition(float DeltaSeconds);
	void ClearDreamTransitionTimer();
	void ApplyDreamFade(float Alpha);

	UPROPERTY()
	TObjectPtr<UCanvasPanel> HudRoot;

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

	UPROPERTY()
	TObjectPtr<UTextBlock> PauseText;

	UPROPERTY()
	TObjectPtr<UBorder> DreamFadeOverlay;

	/** Font multiplier applied to subtitles without changing their layout anchors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|HUD|Accessibility", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float SubtitleScale = 1.f;

	/** Opacity multiplier applied to subtitles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|HUD|Accessibility", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SubtitleOpacity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|HUD|Accessibility")
	FLinearColor SubtitleColor = FLinearColor(0.92f, 0.92f, 0.96f, 1.f);

	/** Prevents an unbounded burst of events from growing memory forever. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triangle|HUD", meta = (ClampMin = "1", ClampMax = "32"))
	int32 MaxSubtitleQueue = 16;

	TWeakObjectPtr<APawn> BoundPlayer;
	TWeakObjectPtr<ULTriangleLucidityComponent> BoundLucidityComponent;
	TWeakObjectPtr<ULTriangleRecognitionComponent> BoundRecognitionComponent;

	TArray<FTriangleQueuedSubtitle> SubtitleQueue;
	FTimerHandle SubtitleTimerHandle;
	float SubtitleRemaining = 0.f;
	bool bSubtitleActive = false;

	FLinearColor DreamFadeColor = FLinearColor::Black;
	float DreamFadeAlpha = 0.f;
	float DreamFadeOutDuration = 0.f;
	float DreamFadeHoldDuration = 0.f;
	float DreamFadeInDuration = 0.f;
	float DreamTransitionElapsed = 0.f;
	EDreamTransitionPhase DreamTransitionPhase = EDreamTransitionPhase::None;
	FTimerHandle DreamFadeTimerHandle;
};
