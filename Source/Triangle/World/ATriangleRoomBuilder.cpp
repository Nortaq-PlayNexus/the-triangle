#include "World/ATriangleRoomBuilder.h"

#include "Triangle.h"
#include "World/ATriangleDoor.h"
#include "World/ATriangleNotebook.h"
#include "Dream/ATriangleTrianglePoint.h"

#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ATriangleRoomBuilder::ATriangleRoomBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
	TriangleCornerLocation = FVector(-RoomHalfX * 0.95f, RoomHalfY * 0.97f, RoomHeight * 0.97f);
}

void ATriangleRoomBuilder::BeginPlay()
{
	Super::BeginPlay();

	BuildShell();
	BuildWindow();
	BuildBed();
	BuildDesk();
	BuildClock();
	BuildNotebook();
	BuildDoor();
	BuildCornerPoint();
	BuildLighting();

	// The player spawns centred in the room, facing the window-lit back wall
	// so the first thing on screen reads as a moonlit bedroom.
	SpawnedDreamStart = Cast<APlayerStart>(SpawnTrackedActor(
		APlayerStart::StaticClass(),
		FTransform(FRotator(0.f, 180.f, 0.f), FVector(0.f, 0.f, 92.f + RoomElevation))));
	if (SpawnedDreamStart)
	{
		SpawnedDreamStart->Tags.Add(TEXT("DreamStart"));
	}

	UE_LOG(LogTriangle, Log, TEXT("Childhood bedroom assembled (%s)."), GetWorld() ? *GetWorld()->GetName() : TEXT("?"));
}

void ATriangleRoomBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// These actors are runtime pieces of the procedural environment rather
	// than level assets. Tear them down explicitly when the builder is
	// replaced or the world is unloaded.
	for (int32 Index = SpawnedActors.Num() - 1; Index >= 0; --Index)
	{
		AActor* Actor = SpawnedActors[Index].Get();
		if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Reset();
	SpawnedDreamStart = nullptr;

	Super::EndPlay(EndPlayReason);
}

FVector ATriangleRoomBuilder::GetTriangleCornerLocation() const
{
	// TriangleCornerLocation is design-space. The room is intentionally built
	// at an elevated world altitude, and the builder may itself be transformed.
	const FVector LocalCorner = TriangleCornerLocation + FVector(0.f, 0.f, RoomElevation);
	return GetActorTransform().TransformPosition(LocalCorner);
}

AActor* ATriangleRoomBuilder::SpawnTrackedActor(UClass* ActorClass, const FTransform& Transform)
{
	UWorld* World = GetWorld();
	if (!World || !ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FTransform& BuilderTransform = GetActorTransform();
	const FTransform WorldTransform(
		BuilderTransform.GetRotation() * Transform.GetRotation(),
		BuilderTransform.TransformPosition(Transform.GetLocation()),
		BuilderTransform.GetScale3D() * Transform.GetScale3D());
	AActor* Actor = World->SpawnActor<AActor>(ActorClass, WorldTransform, SpawnParameters);
	if (Actor)
	{
		SpawnedActors.Add(Actor);
	}
	return Actor;
}

UStaticMesh* ATriangleRoomBuilder::GetCubeMesh()
{
	static TObjectPtr<UStaticMesh> CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	return CubeMesh.Get();
}

UStaticMesh* ATriangleRoomBuilder::GetCylinderMesh()
{
	static TObjectPtr<UStaticMesh> CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	return CylinderMesh.Get();
}

UStaticMeshComponent* ATriangleRoomBuilder::AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(SpawnTrackedActor(
		AStaticMeshActor::StaticClass(),
		FTransform(Rotation, Location + FVector(0.f, 0.f, RoomElevation))));
	if (!MeshActor)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
	if (!MeshComponent)
	{
		return nullptr;
	}
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetStaticMesh(GetCubeMesh());
	MeshComponent->SetRelativeScale3D(Scale);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	return MeshComponent;
}

void ATriangleRoomBuilder::BuildShell()
{
	const float HalfWallMin = 0.2f; // cube of 100cm scaled 0.2 = 20cm thick

	// Floor / ceiling.
	AddBox(FVector(0.f, 0.f, -10.f), FVector(RoomHalfX * 2.f / 100.f, RoomHalfY * 2.f / 100.f, HalfWallMin));
	AddBox(FVector(0.f, 0.f, RoomHeight + 10.f), FVector(RoomHalfX * 2.f / 100.f, RoomHalfY * 2.f / 100.f, 0.15f));

	// Back wall (X negative) - has the window, built as thin full wall plus the
	// window light source in BuildWindow().
	AddBox(FVector(-RoomHalfX, 0.f, RoomHeight * 0.5f), FVector(HalfWallMin, RoomHalfY * 2.f / 100.f, RoomHeight / 100.f));

	// Left / right walls.
	AddBox(FVector(0.f, -RoomHalfY, RoomHeight * 0.5f), FVector(RoomHalfX * 2.f / 100.f, HalfWallMin, RoomHeight / 100.f));
	AddBox(FVector(0.f,  RoomHalfY, RoomHeight * 0.5f), FVector(RoomHalfX * 2.f / 100.f, HalfWallMin, RoomHeight / 100.f));

	// Front wall (X positive) with the door gap at y in [120, 230].
	const float GapYMin = 120.f;
	const float GapYMax = 230.f;
	const float GapHeight = 220.f;

	// Left segment (y -310..120).
	AddBox(FVector(RoomHalfX, (GapYMin - RoomHalfY) * 0.5f, RoomHeight * 0.5f),
		FVector(HalfWallMin, (GapYMin + RoomHalfY) / 100.f, RoomHeight / 100.f));
	// Right segment (y 230..310).
	AddBox(FVector(RoomHalfX, (GapYMax + RoomHalfY) * 0.5f, RoomHeight * 0.5f),
		FVector(HalfWallMin, (RoomHalfY - GapYMax) / 100.f, RoomHeight / 100.f));
	// Lintel above the gap.
	AddBox(FVector(RoomHalfX, (GapYMin + GapYMax) * 0.5f, (RoomHeight + GapHeight) * 0.5f),
		FVector(HalfWallMin, (GapYMax - GapYMin) / 100.f, (RoomHeight - GapHeight) / 100.f));
}

void ATriangleRoomBuilder::BuildWindow()
{
	// A pale panel on the back wall that reads as moonlight.
	UStaticMeshComponent* WindowMesh = AddBox(FVector(-RoomHalfX - 2.f, 0.f, 150.f), FVector(0.06f, 1.2f, 1.4f));
	if (WindowMesh)
	{
		WindowMesh->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void ATriangleRoomBuilder::BuildBed()
{
	// Frame, mattress and pillow reads as a bed in the dark.
	AddBox(FVector(150.f, -220.f, 15.f), FVector(2.0f, 1.0f, 0.3f));
	AddBox(FVector(150.f, -220.f, 42.f), FVector(1.9f, 0.95f, 0.12f));
	AddBox(FVector(60.f, -232.f, 53.f), FVector(0.45f, 0.7f, 0.10f));
	// Headboard.
	AddBox(FVector(252.f, -220.f, 55.f), FVector(0.12f, 1.0f, 0.75f));
}

void ATriangleRoomBuilder::BuildDesk()
{
	AddBox(FVector(-80.f, 200.f, 55.f), FVector(1.2f, 0.6f, 0.7f));
}

void ATriangleRoomBuilder::BuildClock()
{
	// A deliberately ordinary bedroom clock, stopped at twelve. The motif can
	// now refer to something the player has actually seen in the room.
	const FVector ClockLocation(-80.f, RoomHalfY - 5.f, 205.f + RoomElevation);
	if (AStaticMeshActor* ClockActor = Cast<AStaticMeshActor>(SpawnTrackedActor(
		AStaticMeshActor::StaticClass(),
		FTransform(FRotator(90.f, 0.f, 0.f), ClockLocation))))
	{
		if (UStaticMeshComponent* ClockMesh = ClockActor->GetStaticMeshComponent())
		{
			ClockMesh->SetMobility(EComponentMobility::Movable);
			ClockMesh->SetStaticMesh(GetCylinderMesh());
			ClockMesh->SetRelativeScale3D(FVector(0.68f, 0.04f, 0.68f));
			ClockMesh->SetCollisionProfileName(TEXT("NoCollision"));
			ClockMesh->SetLinearColor(FLinearColor(0.28f, 0.24f, 0.18f));
		}
	}

	// Both hands point to twelve; their different lengths remain readable at a
	// distance without requiring a texture or imported clock mesh.
	UStaticMeshComponent* LongHand = AddBox(
		FVector(-80.f, RoomHalfY - 10.f, 231.f),
		FVector(0.035f, 0.025f, 0.52f));
	UStaticMeshComponent* ShortHand = AddBox(
		FVector(-80.f, RoomHalfY - 10.f, 219.f),
		FVector(0.045f, 0.025f, 0.36f));
	if (LongHand)
	{
		LongHand->SetCollisionProfileName(TEXT("NoCollision"));
		LongHand->SetLinearColor(FLinearColor(0.72f, 0.68f, 0.58f));
	}
	if (ShortHand)
	{
		ShortHand->SetCollisionProfileName(TEXT("NoCollision"));
		ShortHand->SetLinearColor(FLinearColor(0.72f, 0.68f, 0.58f));
	}
}

void ATriangleRoomBuilder::BuildNotebook()
{
	ATriangleNotebook* Notebook = Cast<ATriangleNotebook>(SpawnTrackedActor(
		ATriangleNotebook::StaticClass(),
		FTransform(FRotator::ZeroRotator, FVector(-50.f, 200.f, 91.f + RoomElevation))));
	if (Notebook)
	{
		Notebook->Tags.Add(TEXT("DreamMemory"));
	}
}

void ATriangleRoomBuilder::BuildDoor()
{
	SpawnTrackedActor(
		ATriangleDoor::StaticClass(),
		FTransform(FRotator::ZeroRotator, FVector(RoomHalfX - 5.f, 175.f, 105.f + RoomElevation)));
}

void ATriangleRoomBuilder::BuildCornerPoint()
{
	if (ATriangleTrianglePoint* Point = Cast<ATriangleTrianglePoint>(SpawnTrackedActor(
		ATriangleTrianglePoint::StaticClass(),
		FTransform(FRotator::ZeroRotator, TriangleCornerLocation + FVector(0.f, 0.f, RoomElevation)))))
	{
		Point->ManifestId = FName(TEXT("UpperCornerPoint"));
	}
}

void ATriangleRoomBuilder::BuildLighting()
{
	// Ambient moon wash.
	if (ASkyLight* Sky = Cast<ASkyLight>(SpawnTrackedActor(ASkyLight::StaticClass(), FTransform::Identity)))
	{
		if (ULightComponent* LightComponent = Sky->GetLightComponent())
		{
			LightComponent->SetIntensity(0.45f);
			LightComponent->SetLightColor(FLinearColor(0.30f, 0.36f, 0.46f));
			LightComponent->SetCastShadows(false);
		}
	}

	// Ceiling fill - sombre, low, slightly warm against the blue moonlight.
	if (APointLight* Fill = Cast<APointLight>(SpawnTrackedActor(
		APointLight::StaticClass(),
		FTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, 250.f + RoomElevation)))))
	{
		if (UPointLightComponent* FillLight = Cast<UPointLightComponent>(Fill->GetLightComponent()))
		{
			FillLight->SetIntensity(140.f);
			FillLight->SetAttenuationRadius(900.f);
			FillLight->SetLightColor(FLinearColor(0.62f, 0.55f, 0.44f));
			FillLight->SetCastShadows(false);
		}
	}

	// Cold glow spilling from the window.
	if (APointLight* WindowLight = Cast<APointLight>(SpawnTrackedActor(
		APointLight::StaticClass(),
		FTransform(FRotator::ZeroRotator, FVector(-RoomHalfX - 18.f, 0.f, 150.f + RoomElevation)))))
	{
		if (UPointLightComponent* WindowLightComponent = Cast<UPointLightComponent>(WindowLight->GetLightComponent()))
		{
			WindowLightComponent->SetIntensity(340.f);
			WindowLightComponent->SetAttenuationRadius(1300.f);
			WindowLightComponent->SetLightColor(FLinearColor(0.55f, 0.68f, 0.95f));
			WindowLightComponent->SetCastShadows(false);
		}
	}

	// Fog for corner darkness and depth.
	if (AExponentialHeightFog* Fog = Cast<AExponentialHeightFog>(SpawnTrackedActor(
		AExponentialHeightFog::StaticClass(), FTransform::Identity)))
	{
		if (UExponentialHeightFogComponent* FogComponent = Cast<UExponentialHeightFogComponent>(Fog->GetComponent()))
		{
			FogComponent->SetFogDensity(0.0045f);
			FogComponent->SetFogInscatteringColor(FLinearColor(0.06f, 0.07f, 0.09f));
		}
	}
}
