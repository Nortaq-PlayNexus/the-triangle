#include "UI/UTriangleHUDWidget.h"

#include "Triangle.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Misc/Paths.h"
#include "TimerManager.h"

void UTriangleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureChildWidgets();
}

void UTriangleHUDWidget::EnsureChildWidgets()
{
	if (InteractionAction || !WidgetTree)
	{
		return;
	}

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("HudRoot"));
	WidgetTree->RootWidget = Root;

	auto MakeText = [this](const FName& Name, const FLinearColor& Color, float Size)
	{
		UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		Block->SetText(FText::GetEmpty());
		Block->SetColorAndOpacity(FSlateColor(Color));
		Block->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansMono.ttf"), Size));
		return Block;
	};

	InteractionAction = MakeText(TEXT("InteractionAction"), FLinearColor(0.92f, 0.92f, 0.96f), 20.f);
	InteractionName = MakeText(TEXT("InteractionName"), FLinearColor(0.55f, 0.55f, 0.60f), 12.f);
	LucidityLabel = MakeText(TEXT("LucidityLabel"), FLinearColor(0.42f, 0.10f, 0.22f), 10.f);
	SubtitleText = MakeText(TEXT("SubtitleText"), FLinearColor(0.92f, 0.92f, 0.96f), 16.f);

	LucidityBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("LucidityBar"));
	LucidityBar->SetFillColorAndOpacity(FLinearColor(0.42f, 0.10f, 0.22f));
	LucidityBar->SetPercent(0.f);
	LucidityBar->SetVisibility(ESlateVisibility::Collapsed);

	auto SlotAt = [Root](UWidget* Widget, const FVector2D& Position)
	{
		UCanvasPanelSlot* Slot = Root->AddChildToCanvas(Widget);
		Slot->SetPosition(Position);
		Slot->SetAutoSize(true);
		return Slot;
	};

	// Interaction prompt (bottom-centre).
	SlotAt(InteractionAction, FVector2D(10.f, 640.f));
	SlotAt(InteractionName, FVector2D(10.f, 668.f));

	// Subtitle above the prompt.
	SlotAt(SubtitleText, FVector2D(10.f, 610.f));

	// Lucidity meter (top-left corner, unobtrusive).
	SlotAt(LucidityLabel, FVector2D(20.f, 30.f));
	SlotAt(LucidityBar, FVector2D(20.f, 48.f));

	// The meter widths are set when caption+bar are shown.
	LucidityBar->SetRenderScale(FVector2D(3.2f, 0.7f));

	InteractionAction->SetVisibility(ESlateVisibility::Collapsed);
	InteractionName->SetVisibility(ESlateVisibility::Collapsed);
	SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
	LucidityLabel->SetVisibility(ESlateVisibility::Collapsed);
	LucidityBar->SetVisibility(ESlateVisibility::Collapsed);
}

void UTriangleHUDWidget::SetInteractionPrompt(const FText& Text, const FText& ActionName)
{
	if (!InteractionAction)
	{
		EnsureChildWidgets();
	}

	if (Text.IsEmpty())
	{
		InteractionAction->SetVisibility(ESlateVisibility::Collapsed);
		InteractionName->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	InteractionAction->SetText(Text);
	InteractionName->SetText(ActionName);
	InteractionAction->SetVisibility(ESlateVisibility::HitTestInvisible);
	InteractionName->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UTriangleHUDWidget::SetLucidityValue(float Value01)
{
	if (!LucidityBar)
	{
		EnsureChildWidgets();
	}
	LucidityBar->SetPercent(FMath::Clamp(Value01, 0.f, 1.f));
}

void UTriangleHUDWidget::SetLucidityMeterVisible(bool bVisible)
{
	if (!LucidityBar)
	{
		EnsureChildWidgets();
	}
	const ESlateVisibility Vis = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
	LucidityBar->SetVisibility(Vis);
	LucidityLabel->SetVisibility(Vis);
}

void UTriangleHUDWidget::PushSubtitle(const FText& Text, float Duration)
{
	if (!SubtitleText)
	{
		EnsureChildWidgets();
	}

	SubtitleText->SetText(Text);
	SubtitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
	SubtitleRemaining = Duration;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	World->GetTimerManager().ClearTimer(SubtitleTimerHandle);
	World->GetTimerManager().SetTimer(SubtitleTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this, World]()
	{
		if (SubtitleRemaining > 0.f && SubtitleText)
		{
			SubtitleRemaining -= 0.1f;
			if (SubtitleRemaining <= 0.f)
			{
				SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
				World->GetTimerManager().ClearTimer(SubtitleTimerHandle);
			}
		}
	}), 0.1f, true);
}