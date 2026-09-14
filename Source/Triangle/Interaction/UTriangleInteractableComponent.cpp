#include "Interaction/UTriangleInteractableComponent.h"

#include "Triangle.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UTriangleInteractableComponent::UTriangleInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTriangleInteractableComponent::Interact(APawn* Interactor)
{
	if (!bEnabled)
	{
		return;
	}

	UE_LOG(LogTriangleInteraction, Log, TEXT("Interact: %s interacted with {%s}."), Interactor ? *Interactor->GetName() : TEXT("unknown"), *InteractionName.ToString());
	OnInteracted.Broadcast(Interactor);
}

void UTriangleInteractableComponent::NotifyFocusBegin()
{
	if (bEnabled)
	{
		OnFocusBegin.Broadcast();
	}
}

void UTriangleInteractableComponent::NotifyFocusEnd()
{
	OnFocusEnd.Broadcast();
}

FVector UTriangleInteractableComponent::GetInteractionWorldLocation() const
{
	if (InteractionAnchor)
	{
		return InteractionAnchor->GetComponentLocation();
	}
	if (GetOwner())
	{
		return GetOwner()->GetActorLocation();
	}
	return FVector::ZeroVector;
}