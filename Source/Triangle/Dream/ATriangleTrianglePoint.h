#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleTrianglePoint.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

/**
 * The point of the dream.
 *
 * Appears as a tiny geometric intrusion in the environment - most often in an
 * upper corner of the bedroom. It is intentionally small, faint and ambiguous.
 * The Dream Director decides when it is present; the player should never be
 * able to walk up to it and "find" what it is.
 */
UCLASS()
class ATriangleTrianglePoint : public AActor
{
	GENERATED_BODY()

public:
	ATriangleTrianglePoint();

	/** Shows the point for the given duration, then hides it again. */
	UFUNCTION(BlueprintCallable, Category = "Triangle|Manifest")
	void Manifest(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Triangle|Manifest")
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "Triangle|Manifest")
	bool IsVisible() const { return bPointVisible; }

	/** Readable name used in encounter tracking. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	FName ManifestId = TEXT("UpperCornerPoint");

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TriangleMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> GlowLight;

	bool bPointVisible = false;
	FTimerHandle ManifestTimerHandle;
};