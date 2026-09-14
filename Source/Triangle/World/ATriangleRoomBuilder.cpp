#include "World/ATriangleRoomBuilder.h"

#include "Triangle.h"
#include "World/ATriangleDoor.h"
#include "World/ATriangleNotebook.h"
#include "Dream/ATriangleTrianglePoint.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"

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
	BuildNotebook();
	BuildDoor();
	BuildCornerPoint();
	BuildLighting();

	// The player spawns centred near the back wall, facing the door.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (UWorld* World = GetWorld())
	{
		World->SpawnActor<APlayerStart>(FVector(0.f, 0.f, 92.f), FRotator::ZeroRotator, SpawnParams);
	}

	UE_LOG(LogTriangle, Log, TEXT("Childhood bedroom assembled (%s)."), *GetWorld() ? *GetWorld()->GetName() : TEXT("?"));
}

UStaticMesh* ATriangleRoomBuilder::GetCubeMesh()
{
	static TObjectPtr<UStaticMesh> CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	return CubeMesh.Get();
}

UStaticMesh* ATriangleRoomBuilder::GetPlaneMesh()
{
	static TObjectPtr<UStaticMesh> PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	return PlaneMesh.Get();
}

UStaticMeshComponent* ATriangleRoomBuilder::AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	if (!GetWorld())
	{
		return nullptr;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(Rotation, Location));
	if (!MeshActor)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
	MeshComponent->SetStaticMesh(GetCubeMesh());
	MeshComponent->SetRelativeScale3D(Scale);
	MeshComponent->SetMobility(EComponentMobility::Movable);
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

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Moonlight rakes across the room from high behind the window.
	ADirectionalLight* Moon = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FRotator(-35.f, -45.f, 0.f));
	if (Moon)
	{
		Moon->GetLightComponent()->SetIntensity(0.35f);
		Moon->GetLightComponent()->SetLightColor(FLinearColor(0.55f, 0.62f, 0.85f));
		Moon->GetLightComponent()->SetCastShadows(true);
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

void ATriangleRoomBuilder::BuildNotebook()
{
	if (!GetWorld())
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATriangleNotebook* Notebook = GetWorld()->SpawnActor<ATriangleNotebook>(ATriangleNotebook::StaticClass(), FVector(-50.f, 200.f, 91.f), FRotator::ZeroRotator, Params);
	if (Notebook)
	{
		Notebook->Tags.Add(TEXT("DreamMemory"));
	}
}

void ATriangleRoomBuilder::BuildDoor()
{
	if (!GetWorld())
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ATriangleDoor>(ATriangleDoor::StaticClass(), FVector(RoomHalfX - 5.f, 175.f, 105.f), FRotator::ZeroRotator, Params);
}

void ATriangleRoomBuilder::BuildCornerPoint()
{
	if (!GetWorld())
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ATriangleTrianglePoint>(ATriangleTrianglePoint::StaticClass(), TriangleCornerLocation, FRotator::ZeroRotator, Params);
}

void ATriangleRoomBuilder::BuildLighting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Ambient moon wash.
	ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FTransform::Identity);
	if (Sky)
	{
		Sky->GetLightComponent()->SetIntensity(0.12f);
		Sky->GetLightComponent()->SetLightColor(FLinearColor(0.30f, 0.36f, 0.46f));
		Sky->GetLightComponent()->SetCastShadows(false);
	}

	// Ceiling fill - sombre, low, slightly warm against the blue moonlight.
	APointLight* Fill = World->SpawnActor<APointLight>(APointLight::StaticClass(), FVector(0.f, 0.f, 250.f), FRotator::ZeroRotator);
	if (Fill)
	{
		Fill->GetLightComponent()->SetIntensity(9.f);
		Fill->GetLightComponent()->SetAttenuationRadius(850.f);
		Fill->GetLightComponent()->SetLightColor(FLinearColor(0.62f, 0.55f, 0.44f));
		Fill->GetLightComponent()->SetCastShadows(false);
	}

	// Cold glow spilling from the window.
	APointLight* WindowLight = World->SpawnActor<APointLight>(APointLight::StaticClass(), FVector(-RoomHalfX - 18.f, 0.f, 150.f), FRotator::ZeroRotator);
	if (WindowLight)
	{
		WindowLight->GetLightComponent()->SetIntensity(28.f);
		WindowLight->GetLightComponent()->SetAttenuationRadius(1100.f);
		WindowLight->GetLightComponent()->SetLightColor(FLinearColor(0.55f, 0.68f, 0.95f));
		WindowLight->GetLightComponent()->SetCastShadows(false);
	}

	// Fog for corner darkness and depth.
	AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FTransform::Identity);
	if (Fog)
	{
		Fog->GetComponent()->SetFogDensity(0.0045f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.06f, 0.07f, 0.09f));
	}
}