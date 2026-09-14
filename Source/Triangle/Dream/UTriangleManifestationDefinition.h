#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Dream/ETriangleManifestationKind.h"
#include "UTriangleManifestationDefinition.generated.h"

/**
 * Data asset describing one possible manifestation of The Triangle.
 *
 * The Dream Director holds a pool of these and picks from them as recognition
 * rises. Each definition states which body The Triangle takes, whether the
 * hum accompanies it and how loud, how long it lasts, and how much recognition
 * a witnessed appearance is worth. See the design notes: sometimes the hum
 * starts and nothing is ever visible - that is a valid manifestation too.
 */
UCLASS(BlueprintType)
class UTriangleManifestationDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	FName ManifestId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	ETriangleManifestationKind Kind = ETriangleManifestationKind::Point;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	ETriangleAnomalyKind AnomalyKind = ETriangleAnomalyKind::TriangularGap;

	/** Whether the hum is part of this manifestation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	bool bUseHum = true;

	/** Hum presence (0..1) while this manifestation is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest", meta = (ClampMin = "0", ClampMax = "1"))
	float HumPresence = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest", meta = (ClampMin = "0.5", ClampMax = "60"))
	float MinDuration = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest", meta = (ClampMin = "0.5", ClampMax = "60"))
	float MaxDuration = 8.f;

	/** Recognition gained when this appearance resolves while being witnessed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest", meta = (ClampMin = "0", ClampMax = "30"))
	float RecognitionGainOnSight = 6.f;

	/** Minimum gap between consecutive appearances of this body. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest", meta = (ClampMin = "3", ClampMax = "120"))
	float CooldownSeconds = 18.f;

	/** Design note for the authoring team (never shown in game). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Triangle|Manifest")
	FText ObserverNote;

	/** A random duration within the configured window. */
	UFUNCTION(BlueprintPure, Category = "Triangle|Manifest")
	float GetRandomDuration() const;
};