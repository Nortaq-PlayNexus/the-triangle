#include "Characters/ATrianglePlayerController.h"

#include "UI/ATriangleHUD.h"

ATrianglePlayerController::ATrianglePlayerController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATrianglePlayerController::SetInteractionPrompt(const FText& Text, const FText& ActionName)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetInteractionPrompt(Text, ActionName);
	}
}

void ATrianglePlayerController::SetLucidityMeterVisible(bool bVisible)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetLucidityMeterVisible(bVisible);
	}
}

void ATrianglePlayerController::SetRecognitionMeterVisible(bool bVisible)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetRecognitionMeterVisible(bVisible);
	}
}

ATriangleHUD* ATrianglePlayerController::GetTriangleHUD() const
{
	return Cast<ATriangleHUD>(GetHUD());
}