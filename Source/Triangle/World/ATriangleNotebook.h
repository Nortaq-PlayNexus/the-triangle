#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATriangleNotebook.generated.h"

class UStaticMeshComponent;
class UTriangleInteractableComponent;

/**
 * The notebook. It collects dream observations across the whole game.
 *
 * In the vertical slice it already proves the memory/interaction loop: page
 * text accumulates across interactions, lucidity grows with each recognition,
 * and the front page hides a triangle the player cannot fully explain.
 */
UCLASS()
class ATriangleNotebook : public AActor
{
	GENERATED_BODY()

public:
	ATriangleNotebook();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnInteracted(APawn* Interactor);

	void ShowPage(int32 PageIndex);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CoverMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTriangleInteractableComponent> Interactable;

	int32 InteractionCount = 0;
};