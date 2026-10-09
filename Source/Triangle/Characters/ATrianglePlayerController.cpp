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

void ATrianglePlayerController::SetLucidityValue(float Value01)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetLucidityValue(Value01);
	}
}

void ATrianglePlayerController::SetLucidityMeterVisible(bool bVisible)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetLucidityMeterVisible(bVisible);
	}
}

void ATrianglePlayerController::SetRecognitionValue(float Value01)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetRecognitionValue(Value01);
	}
}

void ATrianglePlayerController::SetRecognitionMeterVisible(bool bVisible)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetRecognitionMeterVisible(bVisible);
	}
}

void ATrianglePlayerController::PushSubtitle(const FText& Text, float Duration)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->PushSubtitle(Text, Duration);
	}
}

void ATrianglePlayerController::SetSubtitleScale(float Scale)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetSubtitleScale(Scale);
	}
}

void ATrianglePlayerController::SetSubtitleOpacity(float Opacity)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetSubtitleOpacity(Opacity);
	}
}

void ATrianglePlayerController::StartDreamTransition(float FadeOutDuration, float HoldDuration, float FadeInDuration, FLinearColor FadeColor)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->StartDreamTransition(FadeOutDuration, HoldDuration, FadeInDuration, FadeColor);
	}
}

void ATrianglePlayerController::SetDreamFadeAlpha(float Alpha)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetDreamFadeAlpha(Alpha);
	}
}

void ATrianglePlayerController::StopDreamTransition()
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->StopDreamTransition();
	}
}

void ATrianglePlayerController::SetPauseVisible(bool bVisible)
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->SetPauseVisible(bVisible);
	}
}

void ATrianglePlayerController::BindHUDToCurrentPawn()
{
	if (ATriangleHUD* HUD = GetTriangleHUD())
	{
		HUD->BindToPlayer(GetPawn());
	}
}

ATriangleHUD* ATrianglePlayerController::GetTriangleHUD() const
{
	AHUD* HUD = GetHUD();
	return IsValid(HUD) ? Cast<ATriangleHUD>(HUD) : nullptr;
}
