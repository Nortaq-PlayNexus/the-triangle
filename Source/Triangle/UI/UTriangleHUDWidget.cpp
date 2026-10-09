#include "UI/UTriangleHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "TimerManager.h"

namespace TriangleHUD
{
	constexpr float SubtitleTickSeconds = 0.1f;
	constexpr float DreamFadeTickSeconds = 0.05f;
	constexpr float MinimumSubtitleDuration = 0.05f;

	const TCHAR* LucidityLevelName(const ETriangleLucidityLevel Level)
	{
		switch (Level)
		{
		case ETriangleLucidityLevel::Low:
			return TEXT("LOW");
		case ETriangleLucidityLevel::Medium:
			return TEXT("MEDIUM");
		case ETriangleLucidityLevel::High:
			return TEXT("HIGH");
		case ETriangleLucidityLevel::Maximum:
			return TEXT("MAXIMUM");
		default:
			return TEXT("LOW");
		}
	}

	const TCHAR* RecognitionLevelName(const ETriangleRecognitionLevel Level)
	{
		switch (Level)
		{
		case ETriangleRecognitionLevel::Unknowing:
			return TEXT("UNKNOWING");
		case ETriangleRecognitionLevel::Noticing:
			return TEXT("NOTICING");
		case ETriangleRecognitionLevel::Recognizing:
			return TEXT("RECOGNIZING");
		case ETriangleRecognitionLevel::Anticipating:
			return TEXT("ANTICIPATING");
		case ETriangleRecognitionLevel::Master:
			return TEXT("MASTER");
		default:
			return TEXT("UNKNOWING");
		}
	}

	FText MakeMeterLabel(const TCHAR* MeterName, const TCHAR* LevelName)
	{
		return FText::FromString(FString::Printf(TEXT("%s  %s"), MeterName, LevelName));
	}
}

void UTriangleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnsureChildWidgets();
	ApplySubtitleAppearance();

	// A widget can be constructed before possession completes.  HUD also
	// retries the bind when the pawn changes, but doing it here covers the
	// normal possession path without requiring a HUD tick.
	if (APlayerController* OwningController = GetOwningPlayer(); IsValid(OwningController))
	{
		if (APawn* OwningPawn = OwningController->GetPawn())
		{
			if (BoundPlayer.Get() != OwningPawn)
			{
				BindToPlayer(OwningPawn);
			}
		}
	}

	if (!bSubtitleActive && !SubtitleQueue.IsEmpty())
	{
		ShowNextSubtitle();
	}
}

void UTriangleHUDWidget::NativeDestruct()
{
	ClearSubtitleTimer();
	SubtitleQueue.Reset();
	SubtitleRemaining = 0.f;
	bSubtitleActive = false;
	if (SubtitleText)
	{
		SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
	}
	UnbindFromPlayer();

	ClearDreamTransitionTimer();
	DreamTransitionPhase = EDreamTransitionPhase::None;
	if (DreamFadeOverlay)
	{
		DreamFadeOverlay->SetBrushColor(FLinearColor::Transparent);
		DreamFadeOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnDreamTransitionMidpoint.Clear();
	OnDreamTransitionFinished.Clear();

	Super::NativeDestruct();
}

void UTriangleHUDWidget::EnsureChildWidgets()
{
	if (!WidgetTree)
	{
		return;
	}

	if (!HudRoot)
	{
		if (!WidgetTree->RootWidget)
		{
			HudRoot = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("HudRoot"));
			WidgetTree->RootWidget = HudRoot;
		}
		else
		{
			HudRoot = Cast<UCanvasPanel>(WidgetTree->RootWidget);
		}
	}

	if (!HudRoot)
	{
		return;
	}
	HudRoot->SetVisibility(ESlateVisibility::HitTestInvisible);

	// Native construction is intentionally all-or-nothing.  A Blueprint
	// subclass may provide its own root; in that case its child widgets remain
	// authoritative rather than being injected into an incompatible tree.
	if (InteractionAction || InteractionName || SubtitleText || PauseText || LucidityBar || RecognitionBar || DreamFadeOverlay)
	{
		ApplySubtitleAppearance();
		return;
	}

	auto MakeText = [this](const FName& Name, const FLinearColor& Color, float Size, const ETextJustify::Type Justification)
	{
		UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		if (!Block)
		{
			return static_cast<UTextBlock*>(nullptr);
		}

		Block->SetText(FText::GetEmpty());
		Block->SetColorAndOpacity(FSlateColor(Color));
		Block->SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Size));
		Block->SetJustification(Justification);
		Block->SetVisibility(ESlateVisibility::Collapsed);
		return Block;
	};

	InteractionAction = MakeText(TEXT("InteractionAction"), FLinearColor(0.92f, 0.92f, 0.96f, 1.f), 20.f, ETextJustify::Center);
	InteractionName = MakeText(TEXT("InteractionName"), FLinearColor(0.55f, 0.55f, 0.60f, 1.f), 12.f, ETextJustify::Center);
	LucidityLabel = MakeText(TEXT("LucidityLabel"), FLinearColor(0.42f, 0.10f, 0.22f, 1.f), 10.f, ETextJustify::Left);
	RecognitionLabel = MakeText(TEXT("RecognitionLabel"), FLinearColor(0.10f, 0.22f, 0.30f, 1.f), 10.f, ETextJustify::Left);
	SubtitleText = MakeText(TEXT("SubtitleText"), FLinearColor(0.92f, 0.92f, 0.96f, 1.f), 16.f, ETextJustify::Center);
	PauseText = MakeText(TEXT("PauseText"), FLinearColor(0.92f, 0.92f, 0.96f, 1.f), 22.f, ETextJustify::Center);
	if (PauseText)
	{
		PauseText->SetText(FText::FromString(TEXT("PAUSED\nESC / START - RESUME")));
	}

	LucidityBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("LucidityBar"));
	if (LucidityBar)
	{
		LucidityBar->SetFillColorAndOpacity(FLinearColor(0.42f, 0.10f, 0.22f, 1.f));
		LucidityBar->SetPercent(0.f);
		LucidityBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	RecognitionBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("RecognitionBar"));
	if (RecognitionBar)
	{
		RecognitionBar->SetFillColorAndOpacity(FLinearColor(0.10f, 0.22f, 0.30f, 1.f));
		RecognitionBar->SetPercent(0.f);
		RecognitionBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!InteractionAction || !InteractionName || !LucidityLabel || !RecognitionLabel || !SubtitleText || !PauseText || !LucidityBar || !RecognitionBar)
	{
		return;
	}

	// Canvas anchors are viewport-relative; the offsets below are only small
	// safe-area style margins.  The center prompt therefore remains centered
	// on both 16:9 and ultrawide displays instead of relying on fixed pixel
	// coordinates.
	auto AddTextSlot = [HudRoot](UWidget* Widget, const FAnchors& Anchors, const FVector2D& Alignment, const FVector2D& Offset)
	{
		UCanvasPanelSlot* Slot = HudRoot->AddChildToCanvas(Widget);
		if (Slot)
		{
			Slot->SetAnchors(Anchors);
			Slot->SetAlignment(Alignment);
			Slot->SetAutoSize(true);
			Slot->SetPosition(Offset);
		}
		return Slot;
	};

	// Interaction prompt and subtitle share a bottom-center anchor.
	AddTextSlot(InteractionAction, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(0.f, -92.f));
	AddTextSlot(InteractionName, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(0.f, -66.f));
	AddTextSlot(SubtitleText, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(0.f, -132.f));
	AddTextSlot(PauseText, FAnchors(0.5f, 0.5f, 0.5f, 0.5f), FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);

	// Optional meters stay tucked into the top-left safe area on every aspect
	// ratio.  Their bar size is explicit, but their anchor is not viewport-fixed.
	AddTextSlot(LucidityLabel, FAnchors(0.f, 0.f, 0.f, 0.f), FVector2D::ZeroVector, FVector2D(32.f, 28.f));
	AddTextSlot(RecognitionLabel, FAnchors(0.f, 0.f, 0.f, 0.f), FVector2D::ZeroVector, FVector2D(32.f, 76.f));

	auto AddMeterSlot = [HudRoot](UProgressBar* Bar, const FVector2D& Offset)
	{
		UCanvasPanelSlot* Slot = HudRoot->AddChildToCanvas(Bar);
		if (Slot)
		{
			Slot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
			Slot->SetAlignment(FVector2D::ZeroVector);
			Slot->SetAutoSize(false);
			Slot->SetPosition(Offset);
			Slot->SetSize(FVector2D(180.f, 8.f));
		}
		return Slot;
	};

	AddMeterSlot(LucidityBar, FVector2D(32.f, 50.f));
	AddMeterSlot(RecognitionBar, FVector2D(32.f, 98.f));
	// Keep the bars compact in the safe area; the Canvas slot already owns
	// their responsive size, so an extra render scale would make them balloon
	// on high-DPI displays.
	LucidityBar->SetRenderScale(FVector2D(1.f, 1.f));
	RecognitionBar->SetRenderScale(FVector2D(1.f, 1.f));

	// Added last so it covers every other HUD element while active.
	DreamFadeOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DreamFadeOverlay"));
	if (DreamFadeOverlay)
	{
		FSlateBrush FadeBrush;
		FadeBrush.DrawAs = ESlateBrushDrawType::Box;
		FadeBrush.ImageType = ESlateBrushImageType::NoImage;
		FadeBrush.ImageSize = FVector2D(1.f, 1.f);
		FadeBrush.TintColor = FSlateColor(FLinearColor::White);
		DreamFadeOverlay->SetBrush(FadeBrush);
		DreamFadeOverlay->SetPadding(FMargin(0.f));
		DreamFadeOverlay->SetBrushColor(FLinearColor::Transparent);
		DreamFadeOverlay->SetVisibility(ESlateVisibility::Collapsed);
		UCanvasPanelSlot* FadeSlot = HudRoot->AddChildToCanvas(DreamFadeOverlay);
		if (FadeSlot)
		{
			FadeSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			FadeSlot->SetAlignment(FVector2D::ZeroVector);
			FadeSlot->SetAutoSize(false);
			FadeSlot->SetOffsets(FMargin(0.f));
			FadeSlot->SetZOrder(1000);
		}
	}

	ApplySubtitleAppearance();
	RefreshLucidityDisplay();
	RefreshRecognitionDisplay();
}

void UTriangleHUDWidget::BindToPlayer(APawn* Player)
{
	UnbindFromPlayer();

	if (!IsValid(Player))
	{
		return;
	}

	BoundPlayer = Player;

	ULTriangleLucidityComponent* LucidityComponent = Player->FindComponentByClass<ULTriangleLucidityComponent>();
	if (IsValid(LucidityComponent))
	{
		BoundLucidityComponent = LucidityComponent;
		LucidityComponent->OnLucidityChanged.AddUniqueDynamic(this, &UTriangleHUDWidget::HandleLucidityChanged);
		LucidityComponent->OnLucidityLevelChanged.AddUniqueDynamic(this, &UTriangleHUDWidget::HandleLucidityLevelChanged);
	}

	ULTriangleRecognitionComponent* RecognitionComponent = Player->FindComponentByClass<ULTriangleRecognitionComponent>();
	if (IsValid(RecognitionComponent))
	{
		BoundRecognitionComponent = RecognitionComponent;
		RecognitionComponent->OnRecognitionChanged.AddUniqueDynamic(this, &UTriangleHUDWidget::HandleRecognitionChanged);
		RecognitionComponent->OnRecognitionLevelChanged.AddUniqueDynamic(this, &UTriangleHUDWidget::HandleRecognitionLevelChanged);
	}

	RefreshLucidityDisplay();
	RefreshRecognitionDisplay();
}

void UTriangleHUDWidget::UnbindFromPlayer()
{
	if (ULTriangleLucidityComponent* LucidityComponent = BoundLucidityComponent.Get())
	{
		if (IsValid(LucidityComponent))
		{
			LucidityComponent->OnLucidityChanged.RemoveDynamic(this, &UTriangleHUDWidget::HandleLucidityChanged);
			LucidityComponent->OnLucidityLevelChanged.RemoveDynamic(this, &UTriangleHUDWidget::HandleLucidityLevelChanged);
		}
	}

	if (ULTriangleRecognitionComponent* RecognitionComponent = BoundRecognitionComponent.Get())
	{
		if (IsValid(RecognitionComponent))
		{
			RecognitionComponent->OnRecognitionChanged.RemoveDynamic(this, &UTriangleHUDWidget::HandleRecognitionChanged);
			RecognitionComponent->OnRecognitionLevelChanged.RemoveDynamic(this, &UTriangleHUDWidget::HandleRecognitionLevelChanged);
		}
	}

	BoundLucidityComponent.Reset();
	BoundRecognitionComponent.Reset();
	BoundPlayer.Reset();
}

bool UTriangleHUDWidget::IsBoundToPlayer() const
{
	return BoundPlayer.IsValid();
}

void UTriangleHUDWidget::SetInteractionPrompt(const FText& Text, const FText& ActionName)
{
	if (!InteractionAction || !InteractionName)
	{
		EnsureChildWidgets();
	}

	if (!InteractionAction || !InteractionName)
	{
		return;
	}

	if (Text.IsEmpty())
	{
		InteractionAction->SetText(FText::GetEmpty());
		InteractionName->SetText(FText::GetEmpty());
		InteractionAction->SetVisibility(ESlateVisibility::Collapsed);
		InteractionName->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	InteractionAction->SetText(Text);
	InteractionAction->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (ActionName.IsEmpty())
	{
		InteractionName->SetText(FText::GetEmpty());
		InteractionName->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		InteractionName->SetText(ActionName);
		InteractionName->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UTriangleHUDWidget::SetPauseVisible(const bool bVisible)
{
	if (!PauseText)
	{
		EnsureChildWidgets();
	}
	if (PauseText)
	{
		PauseText->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UTriangleHUDWidget::SetLucidityValue(float Value01)
{
	if (!LucidityBar)
	{
		EnsureChildWidgets();
	}
	if (LucidityBar)
	{
		const float SafeValue = FMath::IsFinite(Value01) ? Value01 : 0.f;
		LucidityBar->SetPercent(FMath::Clamp(SafeValue, 0.f, 1.f));
	}
}

void UTriangleHUDWidget::SetLucidityMeterVisible(bool bVisible)
{
	if (!LucidityBar || !LucidityLabel)
	{
		EnsureChildWidgets();
	}
	if (LucidityBar && LucidityLabel)
	{
		const ESlateVisibility Visibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		LucidityBar->SetVisibility(Visibility);
		LucidityLabel->SetVisibility(Visibility);
	}
}

void UTriangleHUDWidget::SetRecognitionValue(float Value01)
{
	if (!RecognitionBar)
	{
		EnsureChildWidgets();
	}
	if (RecognitionBar)
	{
		const float SafeValue = FMath::IsFinite(Value01) ? Value01 : 0.f;
		RecognitionBar->SetPercent(FMath::Clamp(SafeValue, 0.f, 1.f));
	}
}

void UTriangleHUDWidget::SetRecognitionMeterVisible(bool bVisible)
{
	if (!RecognitionBar || !RecognitionLabel)
	{
		EnsureChildWidgets();
	}
	if (RecognitionBar && RecognitionLabel)
	{
		const ESlateVisibility Visibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		RecognitionBar->SetVisibility(Visibility);
		RecognitionLabel->SetVisibility(Visibility);
	}
}

void UTriangleHUDWidget::RefreshLucidityDisplay()
{
	ULTriangleLucidityComponent* LucidityComponent = BoundLucidityComponent.Get();
	if (IsValid(LucidityComponent))
	{
		SetLucidityValue(LucidityComponent->GetLucidity() / 100.f);
		if (LucidityLabel)
		{
			LucidityLabel->SetText(TriangleHUD::MakeMeterLabel(TEXT("LUCIDITY"), TriangleHUD::LucidityLevelName(LucidityComponent->GetLucidityLevel())));
		}
		return;
	}

	SetLucidityValue(0.f);
	if (LucidityLabel)
	{
		LucidityLabel->SetText(TriangleHUD::MakeMeterLabel(TEXT("LUCIDITY"), TEXT("LOW")));
	}
}

void UTriangleHUDWidget::RefreshRecognitionDisplay()
{
	ULTriangleRecognitionComponent* RecognitionComponent = BoundRecognitionComponent.Get();
	if (IsValid(RecognitionComponent))
	{
		SetRecognitionValue(RecognitionComponent->GetRecognition() / 100.f);
		if (RecognitionLabel)
		{
			RecognitionLabel->SetText(TriangleHUD::MakeMeterLabel(TEXT("RECOGNITION"), TriangleHUD::RecognitionLevelName(RecognitionComponent->GetRecognitionLevel())));
		}
		return;
	}

	SetRecognitionValue(0.f);
	if (RecognitionLabel)
	{
		RecognitionLabel->SetText(TriangleHUD::MakeMeterLabel(TEXT("RECOGNITION"), TEXT("UNKNOWING")));
	}
}

void UTriangleHUDWidget::HandleLucidityChanged(float Value)
{
	// Save restoration broadcasts the value delegate without replaying the
	// progression-level event. Refresh both pieces from the live component so
	// the label cannot remain at its pre-load level.
	if (BoundLucidityComponent.IsValid())
	{
		SetLucidityValue(Value / 100.f);
		RefreshLucidityDisplay();
	}
}

void UTriangleHUDWidget::HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel)
{
	// Refresh both pieces: the value event normally arrives first, but a
	// level event is also a useful synchronization point after a save load.
	(void)NewLevel;
	RefreshLucidityDisplay();
}

void UTriangleHUDWidget::HandleRecognitionChanged(float Value)
{
	// A loaded scalar is intentionally quiet on the progression-level delegate.
	// Derive the current level as well as the bar value from the component.
	if (BoundRecognitionComponent.IsValid())
	{
		SetRecognitionValue(Value / 100.f);
		RefreshRecognitionDisplay();
	}
}

void UTriangleHUDWidget::HandleRecognitionLevelChanged(ETriangleRecognitionLevel NewLevel)
{
	(void)NewLevel;
	RefreshRecognitionDisplay();
}

void UTriangleHUDWidget::PushSubtitle(const FText& Text, float Duration)
{
	if (Text.IsEmpty())
	{
		return;
	}

	if (!SubtitleText)
	{
		EnsureChildWidgets();
	}

	FTriangleQueuedSubtitle Entry;
	Entry.Text = Text;
	Entry.Duration = FMath::Max(TriangleHUD::MinimumSubtitleDuration, FMath::IsFinite(Duration) ? Duration : 2.f);

	const int32 QueueLimit = FMath::Clamp(MaxSubtitleQueue, 1, 32);
	while (SubtitleQueue.Num() >= QueueLimit)
	{
		SubtitleQueue.RemoveAt(0);
	}
	SubtitleQueue.Add(Entry);

	if (!bSubtitleActive)
	{
		ShowNextSubtitle();
	}
}

void UTriangleHUDWidget::ShowNextSubtitle()
{
	if (bSubtitleActive || SubtitleQueue.IsEmpty() || !SubtitleText)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FTriangleQueuedSubtitle Entry = SubtitleQueue[0];
	SubtitleQueue.RemoveAt(0);
	SubtitleText->SetText(Entry.Text);
	ApplySubtitleAppearance();
	SubtitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
	SubtitleRemaining = FMath::Max(TriangleHUD::MinimumSubtitleDuration, Entry.Duration);
	bSubtitleActive = true;

	ClearSubtitleTimer();
	World->GetTimerManager().SetTimer(
		SubtitleTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			AdvanceSubtitle(TriangleHUD::SubtitleTickSeconds);
		}),
		TriangleHUD::SubtitleTickSeconds,
		true);
}

void UTriangleHUDWidget::AdvanceSubtitle(float DeltaSeconds)
{
	if (!bSubtitleActive)
	{
		ClearSubtitleTimer();
		return;
	}

	SubtitleRemaining -= FMath::Max(0.f, DeltaSeconds);
	if (SubtitleRemaining > 0.f)
	{
		return;
	}

	SubtitleRemaining = 0.f;
	bSubtitleActive = false;
	if (SubtitleText)
	{
		SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!SubtitleQueue.IsEmpty())
	{
		ShowNextSubtitle();
	}
	else
	{
		ClearSubtitleTimer();
	}
}

void UTriangleHUDWidget::ClearSubtitleTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SubtitleTimerHandle);
	}
}

void UTriangleHUDWidget::SetSubtitleScale(float Scale)
{
	SubtitleScale = FMath::Clamp(FMath::IsFinite(Scale) ? Scale : 1.f, 0.5f, 3.f);
	ApplySubtitleAppearance();
}

void UTriangleHUDWidget::SetSubtitleOpacity(float Opacity)
{
	SubtitleOpacity = FMath::Clamp(FMath::IsFinite(Opacity) ? Opacity : 1.f, 0.f, 1.f);
	ApplySubtitleAppearance();
}

void UTriangleHUDWidget::ApplySubtitleAppearance()
{
	if (!SubtitleText)
	{
		return;
	}

	const float SafeScale = FMath::Clamp(FMath::IsFinite(SubtitleScale) ? SubtitleScale : 1.f, 0.5f, 3.f);
	SubtitleText->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	SubtitleText->SetRenderScale(FVector2D(SafeScale, SafeScale));

	FLinearColor Color = SubtitleColor;
	const float SafeOpacity = FMath::IsFinite(SubtitleOpacity) ? FMath::Clamp(SubtitleOpacity, 0.f, 1.f) : 1.f;
	Color.A = FMath::Clamp(Color.A * SafeOpacity, 0.f, 1.f);
	SubtitleText->SetColorAndOpacity(FSlateColor(Color));
}

void UTriangleHUDWidget::StartDreamTransition(float FadeOutDuration, float HoldDuration, float FadeInDuration, FLinearColor FadeColor)
{
	if (!DreamFadeOverlay)
	{
		EnsureChildWidgets();
	}
	if (!DreamFadeOverlay)
	{
		return;
	}

	ClearDreamTransitionTimer();
	DreamFadeColor = FadeColor;
	DreamFadeColor.A = FMath::Clamp(FMath::IsFinite(DreamFadeColor.A) ? DreamFadeColor.A : 1.f, 0.f, 1.f);
	DreamFadeOutDuration = FMath::Max(0.f, FMath::IsFinite(FadeOutDuration) ? FadeOutDuration : 0.f);
	DreamFadeHoldDuration = FMath::Max(0.f, FMath::IsFinite(HoldDuration) ? HoldDuration : 0.f);
	DreamFadeInDuration = FMath::Max(0.f, FMath::IsFinite(FadeInDuration) ? FadeInDuration : 0.f);
	DreamTransitionElapsed = 0.f;
	DreamFadeAlpha = 0.f;
	DreamTransitionPhase = EDreamTransitionPhase::None;
	ApplyDreamFade(0.f);
	DreamFadeOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);

	UWorld* World = GetWorld();
	if (!World)
	{
		StopDreamTransition();
		return;
	}

	if (DreamFadeOutDuration <= KINDA_SMALL_NUMBER && DreamFadeHoldDuration <= KINDA_SMALL_NUMBER && DreamFadeInDuration <= KINDA_SMALL_NUMBER)
	{
		StopDreamTransition();
		OnDreamTransitionFinished.Broadcast();
		return;
	}

	bool bBroadcastMidpointImmediately = false;
	if (DreamFadeOutDuration <= KINDA_SMALL_NUMBER)
	{
		DreamTransitionPhase = DreamFadeHoldDuration > KINDA_SMALL_NUMBER ? EDreamTransitionPhase::Holding : EDreamTransitionPhase::FadingIn;
		DreamTransitionElapsed = 0.f;
		ApplyDreamFade(1.f);
		bBroadcastMidpointImmediately = true;
	}
	else
	{
		DreamTransitionPhase = EDreamTransitionPhase::FadingOut;
	}

	World->GetTimerManager().SetTimer(
		DreamFadeTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			AdvanceDreamTransition(TriangleHUD::DreamFadeTickSeconds);
		}),
		TriangleHUD::DreamFadeTickSeconds,
		true);

	if (bBroadcastMidpointImmediately)
	{
		OnDreamTransitionMidpoint.Broadcast();
	}
}

void UTriangleHUDWidget::SetDreamFadeAlpha(float Alpha)
{
	if (!DreamFadeOverlay)
	{
		EnsureChildWidgets();
	}
	if (DreamFadeOverlay)
	{
		const float SafeAlpha = FMath::Clamp(FMath::IsFinite(Alpha) ? Alpha : 0.f, 0.f, 1.f);
		ApplyDreamFade(SafeAlpha);
		DreamFadeOverlay->SetVisibility((SafeAlpha > KINDA_SMALL_NUMBER || DreamTransitionPhase != EDreamTransitionPhase::None) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UTriangleHUDWidget::StopDreamTransition()
{
	ClearDreamTransitionTimer();
	DreamTransitionPhase = EDreamTransitionPhase::None;
	DreamTransitionElapsed = 0.f;
	ApplyDreamFade(0.f);
	if (DreamFadeOverlay)
	{
		DreamFadeOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UTriangleHUDWidget::IsDreamTransitionActive() const
{
	return DreamTransitionPhase != EDreamTransitionPhase::None;
}

void UTriangleHUDWidget::AdvanceDreamTransition(float DeltaSeconds)
{
	if (DreamTransitionPhase == EDreamTransitionPhase::None)
	{
		ClearDreamTransitionTimer();
		return;
	}

	DreamTransitionElapsed += FMath::Max(0.f, DeltaSeconds);
	switch (DreamTransitionPhase)
	{
	case EDreamTransitionPhase::FadingOut:
	{
		const float Progress = DreamFadeOutDuration > KINDA_SMALL_NUMBER
			? FMath::Clamp(DreamTransitionElapsed / DreamFadeOutDuration, 0.f, 1.f)
			: 1.f;
		ApplyDreamFade(Progress);
		if (Progress >= 1.f)
		{
			DreamTransitionElapsed = 0.f;
			if (DreamFadeHoldDuration > KINDA_SMALL_NUMBER)
			{
				DreamTransitionPhase = EDreamTransitionPhase::Holding;
			}
			else
			{
				DreamTransitionPhase = EDreamTransitionPhase::FadingIn;
			}
			OnDreamTransitionMidpoint.Broadcast();
		}
		break;
	}

	case EDreamTransitionPhase::Holding:
		if (DreamTransitionElapsed >= DreamFadeHoldDuration)
		{
			DreamTransitionElapsed = 0.f;
			DreamTransitionPhase = EDreamTransitionPhase::FadingIn;
		}
		break;

	case EDreamTransitionPhase::FadingIn:
	{
		const float Progress = DreamFadeInDuration > KINDA_SMALL_NUMBER
			? FMath::Clamp(DreamTransitionElapsed / DreamFadeInDuration, 0.f, 1.f)
			: 1.f;
		ApplyDreamFade(1.f - Progress);
		if (Progress >= 1.f)
		{
			StopDreamTransition();
			OnDreamTransitionFinished.Broadcast();
		}
		break;
	}

	default:
		break;
	}
}

void UTriangleHUDWidget::ClearDreamTransitionTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DreamFadeTimerHandle);
	}
}

void UTriangleHUDWidget::ApplyDreamFade(float Alpha)
{
	DreamFadeAlpha = FMath::Clamp(Alpha, 0.f, 1.f);
	if (DreamFadeOverlay)
	{
		FLinearColor Color = DreamFadeColor;
		Color.A = FMath::Clamp(Color.A * DreamFadeAlpha, 0.f, 1.f);
		DreamFadeOverlay->SetBrushColor(Color);
	}
}
