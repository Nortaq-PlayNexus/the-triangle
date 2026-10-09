#include "UI/ATriangleHUD.h"

#include "UI/UTriangleHUDWidget.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

ATriangleHUD::ATriangleHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATriangleHUD::BeginPlay()
{
	Super::BeginPlay();

	EnsureHUDWidget();
	TryBindToPlayer();
}

void ATriangleHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(HUDWidget.Get()))
	{
		HUDWidget->UnbindFromPlayer();
		HUDWidget->StopDreamTransition();
		HUDWidget->RemoveFromParent();
	}
	HUDWidget = nullptr;
	BoundPlayer.Reset();

	Super::EndPlay(EndPlayReason);
}

UTriangleHUDWidget* ATriangleHUD::EnsureHUDWidget()
{
	if (!IsValid(HUDWidget.Get()))
	{
		HUDWidget = nullptr;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	if (!IsValid(HUDWidget.Get()))
	{
		HUDWidget = CreateWidget<UTriangleHUDWidget>(PlayerController, UTriangleHUDWidget::StaticClass());
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(10);
		}
	}
	else if (!HUDWidget->IsInViewport())
	{
		HUDWidget->AddToViewport(10);
	}

	return IsValid(HUDWidget.Get()) ? HUDWidget.Get() : nullptr;
}

void ATriangleHUD::TryBindToPlayer()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	APawn* PlayerPawn = IsValid(PlayerController) ? PlayerController->GetPawn() : nullptr;
	if (!IsValid(PlayerPawn))
	{
		if (BoundPlayer.IsValid() || (IsValid(HUDWidget.Get()) && HUDWidget->IsBoundToPlayer()))
		{
			UnbindFromPlayer();
		}
		return;
	}

	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		if (BoundPlayer.Get() != PlayerPawn || !Widget->IsBoundToPlayer())
		{
			Widget->BindToPlayer(PlayerPawn);
			BoundPlayer = PlayerPawn;
		}
	}
}

void ATriangleHUD::SetInteractionPrompt(const FText& Text, const FText& ActionName)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetInteractionPrompt(Text, ActionName);
	}
}

void ATriangleHUD::SetLucidityValue(float Value01)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetLucidityValue(Value01);
	}
}

void ATriangleHUD::SetLucidityMeterVisible(bool bVisible)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetLucidityMeterVisible(bVisible);
	}
}

void ATriangleHUD::SetRecognitionValue(float Value01)
{
	if (UTriangleHUDWidget* SetRecognitionWidget = EnsureHUDWidget())
	{
		SetRecognitionWidget->SetRecognitionValue(Value01);
	}
}

void ATriangleHUD::SetRecognitionMeterVisible(bool bVisible)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetRecognitionMeterVisible(bVisible);
	}
}

void ATriangleHUD::PushSubtitle(const FText& Text, float Duration)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->PushSubtitle(Text, Duration);
	}
}

void ATriangleHUD::SetSubtitleScale(float Scale)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetSubtitleScale(Scale);
	}
}

void ATriangleHUD::SetSubtitleOpacity(float Opacity)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetSubtitleOpacity(Opacity);
	}
}

void ATriangleHUD::StartDreamTransition(float FadeOutDuration, float HoldDuration, float FadeInDuration, FLinearColor FadeColor)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->StartDreamTransition(FadeOutDuration, HoldDuration, FadeInDuration, FadeColor);
	}
}

void ATriangleHUD::SetDreamFadeAlpha(float Alpha)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetDreamFadeAlpha(Alpha);
	}
}

void ATriangleHUD::StopDreamTransition()
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->StopDreamTransition();
	}
}

void ATriangleHUD::SetPauseVisible(bool bVisible)
{
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->SetPauseVisible(bVisible);
	}
}

void ATriangleHUD::BindToPlayer(APawn* Player)
{
	if (!IsValid(Player))
	{
		UnbindFromPlayer();
		return;
	}

	// Detach the previous pawn even if widget creation is delayed. This keeps
	// a failed/recreated HUD from retaining delegates for an old player.
	UnbindFromPlayer();
	if (UTriangleHUDWidget* Widget = EnsureHUDWidget())
	{
		Widget->BindToPlayer(Player);
		BoundPlayer = Player;
	}
}

void ATriangleHUD::UnbindFromPlayer()
{
	if (IsValid(HUDWidget.Get()))
	{
		HUDWidget->UnbindFromPlayer();
	}
	BoundPlayer.Reset();
}

bool ATriangleHUD::IsBoundToPlayer(APawn* Player) const
{
	return IsValid(Player)
		&& BoundPlayer.Get() == Player
		&& IsValid(HUDWidget.Get())
		&& HUDWidget->IsBoundToPlayer();
}

UTriangleHUDWidget* ATriangleHUD::GetHUDWidget() const
{
	return IsValid(HUDWidget.Get()) ? HUDWidget.Get() : nullptr;
}
