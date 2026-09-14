#include "Nightmare/UTriangleAnomalySubsystem.h"

#include "Triangle.h"
#include "Nightmare/ATriangleAnomalyActor.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

void UTriangleAnomalySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

ATriangleAnomalyActor* UTriangleAnomalySubsystem::SpawnAnomaly(ETriangleAnomalyKind Kind, float Duration, const FVector& AroundLocation)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	if (Kind == ETriangleAnomalyKind::None)
	{
		return nullptr;
	}

	PruneGhosts();

	// Enforce the concurrent budget before spawning.
	while (ActiveAnomalies.Num() >= MaxConcurrent)
	{
		ATriangleAnomalyActor* Oldest = ActiveAnomalies[0];
		ActiveAnomalies.RemoveAt(0);
		if (Oldest)
		{
			Oldest->ShutDownNow();
		}
	}

	const FVector RenderPoint = ChooseRenderPoint(World, Kind, AroundLocation);
	const FRotator RenderRotation = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATriangleAnomalyActor* Anomaly = World->SpawnActor<ATriangleAnomalyActor>(ATriangleAnomalyActor::StaticClass(), FTransform::Identity, Params);
	if (!Anomaly)
	{
		UE_LOG(LogTriangle, Error, TEXT("Anomaly subsystem failed to spawn an anomaly actor."));
		return nullptr;
	}

	Anomaly->Configure(Kind, Duration, RenderPoint, RenderRotation);
	ActiveAnomalies.Add(Anomaly);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Anomaly spawned (alive=%d)."), ActiveAnomalies.Num());
	return Anomaly;
}

void UTriangleAnomalySubsystem::PruneGhosts()
{
	for (int32 i = ActiveAnomalies.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(ActiveAnomalies[i]))
		{
			ActiveAnomalies.RemoveAtSwap(i);
		}
	}
}

int32 UTriangleAnomalySubsystem::GetActiveAnomalyCount() const
{
	return ActiveAnomalies.Num();
}

void UTriangleAnomalySubsystem::DespawnAll()
{
	for (TObjectPtr<ATriangleAnomalyActor> Anomaly : ActiveAnomalies)
	{
		if (Anomaly)
		{
			Anomaly->ShutDownNow();
		}
	}
	ActiveAnomalies.Reset();
	UE_LOG(LogTriangleDream, Verbose, TEXT("All anomalies withdrawn."));
}

FVector UTriangleAnomalySubsystem::ChooseRenderPoint(const UWorld* World, ETriangleAnomalyKind Kind, const FVector& Around) const
{
	// Respect the camera's facing so intrusions land where the player is likely
	// to be looking without ever appearing on top of them.
	FVector Facing = FVector::ForwardVector;
	if (const APawn* Pawn = World->GetFirstPlayerController() ? World->GetFirstPlayerController()->GetPawn() : nullptr)
	{
		if (const UCameraComponent* Camera = Pawn->FindComponentByClass<UCameraComponent>())
		{
			Facing = Camera->GetForwardVector();
		}
	}
	const FVector FacingFlat = FVector(Facing.X, Facing.Y, 0.f).GetSafeNormal();

	if (Kind == ETriangleAnomalyKind::DistantBlackPoint)
	{
		// Far, faint, unapproachable: a point of the dream hovering past reach.
		const float Distance = FMath::FRandRange(1800.f, 3200.f);
		const FVector Dir = (FacingFlat * FMath::FRandRange(0.55f, 1.f) + FVector(FMath::FRandRange(-0.4f, 0.4f), FMath::FRandRange(-0.4f, 0.4f), 0.f)).GetSafeNormal();
		return Around + Dir * Distance + FVector(0.f, 0.f, FMath::FRandRange(20.f, 180.f));
	}

	// Wall-ish sampling: a short step sideways from the player.
	const float Fraction = FMath::FRandRange(0.4f, 0.95f);
	const float Side = FMath::FRandRange(-1.f, 1.f);
	const FVector Dir = (FacingFlat * (Fraction * FMath::Sign(FMath::FRandRange(-1.f, 1.f))) + FVector(-FacingFlat.Y, FacingFlat.X, 0.f) * Side).GetSafeNormal();
	const FVector Point = Around + Dir * FMath::FRandRange(70.f, 230.f);
	return FVector(Point.X, Point.Y, FMath::Clamp(Around.Z + FMath::FRandRange(-40.f, 160.f), 60.f, 260.f));
}