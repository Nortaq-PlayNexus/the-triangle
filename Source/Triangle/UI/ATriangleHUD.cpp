#include "UI/ATriangleHUD.h"

#include "UI/UTriangleHUDWidget.h"
#include "Blueprint/UserWidget.h"

ATriangleHUD::ATriangleHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATriangleHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidget == nullptr)
	{
		HUDWidget = CreateWidget<UTriangleHUDWidget>(GetOwningPlayerController(), UTriangleHUDWidget::StaticClass());
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(10);
		}
	}
}

void ATriangleHUD::SetInteractionPrompt(const FText& Text, const FText& ActionName)
{
	if (HUDWidget)
	{
		HUDWidget->SetInteractionPrompt(Text, ActionName);
	}
}

void ATriangleHUD::SetLucidityMeterVisible(bool bVisible)
{
	if (HUDWidget)
	{
		HUDWidget->SetLucidityMeterVisible(bVisible);
	}
}

void ATriangleHUD::SetRecognitionMeterVisible(bool bVisible)
{
	if (HUDWidget)
	{
		HUDWidget->SetRecognitionMeterVisible(bVisible);
	}
}

void ATriangleHUD::PushSubtitle(const FText& Text, float Duration)
{
	if (HUDWidget)
	{
		HUDWidget->PushSubtitle(Text, Duration);
	}
}