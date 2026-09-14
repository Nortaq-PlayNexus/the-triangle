#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ATriangleHUD.generated.h"

class UTriangleHUDWidget;

/**
 * Thin HUD actor: creates the C++ HUD widget on possession and forwards
 * prompt/lucidity updates to it.
 */
UCLASS()
class ATriangleHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATriangleHUD();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetInteractionPrompt(const FText& Text, const FText& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void SetLucidityMeterVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Triangle|HUD")
	void PushSubtitle(const FText& Text, float Duration);

private:
	UPROPERTY()
	TObjectPtr<UTriangleHUDWidget> HUDWidget;
};