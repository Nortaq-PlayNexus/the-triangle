#include "World/ATriangleStationBuilder.h"

#include "World/ATriangleTrain.h"
#include "Dream/ATriangleTrianglePoint.h"
#include "Triangle.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerStart.h"

ATriangleStationBuilder::ATriangleStationBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
}

UStaticMesh* ATriangleStationBuilder::GetCubeMesh()
{
	static TObjectPtr<UStaticMesh> CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	return CubeMesh.Get();
}

void ATriangleStationBuilder::BeginPlay()
{
	Super::BeginPlay();

	BuildShell();
	BuildPlatforms();
	BuildTracks();
	BuildFluorescentLights();
	BuildVentilation();
	BuildBenches();
	BuildSafetyLights();
	BuildPlatform12Marker();
	BuildTrain();
	BuildCornerPoint();
	BuildLighting();

	// Player spawn on the right-hand platform, facing the tracks.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (UWorld* World = GetWorld())
	{
		APlayerStart* DreamStart = World->SpawnActor<APlayerStart>(
			FVector(0.f, 250.f, PlatformHeight + 40.f),
			FRotator(0.f, -90.f, 0.f),
			SpawnParams);
		if (DreamStart)
		{
			DreamStart->Tags.Add(TEXT("DreamStart"));
		}
	}

	UE_LOG(LogTriangle, Log, TEXT("Underground station assembled."));
}

UStaticMeshComponent* ATriangleStationBuilder::AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	if (!GetWorld())
	{
		return nullptr;
	}

	AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), FTransform(Rotation, Location));
	if (!MeshActor)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetStaticMesh(GetCubeMesh());
	MeshComponent->SetRelativeScale3D(Scale);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	return MeshComponent;
}

void ATriangleStationBuilder::BuildShell()
{
	// Floor — concrete-grey, slightly warm.
	AddBox(FVector(0.f, 0.f, -10.f), FVector(StationHalfX * 2.f / 100.f, StationHalfY * 2.f / 100.f, 0.2f));

	// Ceiling — low, oppressive.
	AddBox(FVector(0.f, 0.f, StationHeight + 10.f), FVector(StationHalfX * 2.f / 100.f, StationHalfY * 2.f / 100.f, 0.15f));

	// Side walls.
	AddBox(FVector(0.f, -StationHalfY, StationHeight * 0.5f),
		FVector(StationHalfX * 2.f / 100.f, 0.2f, StationHeight / 100.f));
	AddBox(FVector(0.f, StationHalfY, StationHeight * 0.5f),
		FVector(StationHalfX * 2.f / 100.f, 0.2f, StationHeight / 100.f));

	// End wall (far end — dark, no exit).
	AddBox(FVector(-StationHalfX, 0.f, StationHeight * 0.5f),
		FVector(0.2f, StationHalfY * 2.f / 100.f, StationHeight / 100.f));
}

void ATriangleStationBuilder::BuildPlatforms()
{
	// Left platform — raised block under the left half of the station.
	AddBox(FVector(0.f, -(StationHalfY + 150.f) * 0.5f, PlatformHeight * 0.5f),
		FVector(StationHalfX * 2.f / 100.f, (StationHalfY - 150.f) / 100.f, PlatformHeight / 100.f));

	// Right platform.
	AddBox(FVector(0.f, (StationHalfY + 150.f) * 0.5f, PlatformHeight * 0.5f),
		FVector(StationHalfX * 2.f / 100.f, (StationHalfY - 150.f) / 100.f, PlatformHeight / 100.f));

	// Platform surface markings — thin lighter strips near the edge.
	AddBox(FVector(-StationHalfX * 0.6f, -152.f, PlatformHeight + 1.f),
		FVector(0.05f, 4.f, 0.1f));
	AddBox(FVector(StationHalfX * 0.6f, 152.f, PlatformHeight + 1.f),
		FVector(0.05f, 4.f, 0.1f));
}

void ATriangleStationBuilder::BuildTracks()
{
	// Two dark strips for the track bed between platforms.
	AddBox(FVector(-150.f, 0.f, 2.f), FVector(3.f, StationHalfY * 2.f / 100.f, 0.1f));
	AddBox(FVector(150.f, 0.f, 2.f), FVector(3.f, StationHalfY * 2.f / 100.f, 0.1f));

	// Sleepers — thin dark boxes under the tracks.
	for (float x = -StationHalfX + 80.f; x < StationHalfX - 80.f; x += 70.f)
	{
		AddBox(FVector(x, 0.f, 1.f), FVector(0.4f, StationHalfY * 2.f / 100.f, 0.08f));
	}
}

void ATriangleStationBuilder::BuildFluorescentLights()
{
	// Four long fluorescent strips along the ceiling — cold, clinical.
	const TArray<FVector> LightPositions =
	{
		FVector(-StationHalfX * 0.375f, 0.f, StationHeight - 20.f),
		FVector(-StationHalfX * 0.125f, 0.f, StationHeight - 20.f),
		FVector(StationHalfX * 0.125f, 0.f, StationHeight - 20.f),
		FVector(StationHalfX * 0.375f, 0.f, StationHeight - 20.f),
	};

	for (const FVector& Pos : LightPositions)
	{
		// The fixture — a long thin white box.
		AddBox(Pos, FVector(4.f, 0.15f, 0.15f));

		// The actual light.
		if (UWorld* World = GetWorld())
		{
			APointLight* FL = World->SpawnActor<APointLight>(
				APointLight::StaticClass(),
				FTransform(FRotator(0.f, 0.f, 0.f), FVector(Pos.X, Pos.Y, Pos.Z - 15.f)));
			if (FL)
			{
				FL->GetLightComponent()->SetIntensity(2200.f);
				FL->GetLightComponent()->SetLightColor(FLinearColor(0.85f, 0.88f, 0.92f));
				FL->GetLightComponent()->SetAttenuationRadius(800.f);
				FL->GetLightComponent()->SetCastShadows(false);
			}
		}
	}
}

void ATriangleStationBuilder::BuildVentilation()
{
	// Two ducts running along the ceiling between the lights.
	AddBox(FVector(-StationHalfX * 0.25f, 0.f, StationHeight - 8.f),
		FVector(StationHalfX * 0.5f / 100.f, 0.25f, 0.25f));
	AddBox(FVector(StationHalfX * 0.25f, 0.f, StationHeight - 8.f),
		FVector(StationHalfX * 0.5f / 100.f, 0.25f, 0.25f));
}

void ATriangleStationBuilder::BuildBenches()
{
	// One bench on each platform, near the middle.
	AddBox(FVector(-StationHalfX * 0.3f, -230.f, PlatformHeight + 20.f),
		FVector(1.2f, 0.35f, 0.4f));
	AddBox(FVector(-StationHalfX * 0.3f, -230.f, PlatformHeight + 40.f),
		FVector(1.2f, 0.15f, 0.4f));

	AddBox(FVector(StationHalfX * 0.3f, 230.f, PlatformHeight + 20.f),
		FVector(1.2f, 0.35f, 0.4f));
	AddBox(FVector(StationHalfX * 0.3f, 230.f, PlatformHeight + 40.f),
		FVector(1.2f, 0.15f, 0.4f));
}

void ATriangleStationBuilder::BuildSafetyLights()
{
	// Amber safety strips along platform edges.
	for (float x = -StationHalfX + 60.f; x < StationHalfX - 60.f; x += 50.f)
	{
		if (UWorld* World = GetWorld())
		{
			APointLight* LeftEdge = World->SpawnActor<APointLight>(
				APointLight::StaticClass(),
				FTransform(FRotator(0.f, 0.f, 0.f), FVector(x, -148.f, PlatformHeight + 5.f)));
			if (LeftEdge)
			{
				LeftEdge->GetLightComponent()->SetIntensity(30.f);
				LeftEdge->GetLightComponent()->SetLightColor(FLinearColor(0.6f, 0.35f, 0.05f));
				LeftEdge->GetLightComponent()->SetAttenuationRadius(80.f);
				LeftEdge->GetLightComponent()->SetCastShadows(false);
			}

			APointLight* RightEdge = World->SpawnActor<APointLight>(
				APointLight::StaticClass(),
				FTransform(FRotator(0.f, 0.f, 0.f), FVector(x, 148.f, PlatformHeight + 5.f)));
			if (RightEdge)
			{
				RightEdge->GetLightComponent()->SetIntensity(30.f);
				RightEdge->GetLightComponent()->SetLightColor(FLinearColor(0.6f, 0.35f, 0.05f));
				RightEdge->GetLightComponent()->SetAttenuationRadius(80.f);
				RightEdge->GetLightComponent()->SetCastShadows(false);
			}
		}
	}
}

void ATriangleStationBuilder::BuildPlatform12Marker()
{
	if (!GetWorld())
	{
		return;
	}

	// Platform 12 sign post.
	AddBox(FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 90.f),
		FVector(0.08f, 0.08f, 1.8f));

	// The sign itself.
	UStaticMeshComponent* Sign = AddBox(FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 140.f),
		FVector(1.2f, 0.6f, 0.06f));
	if (Sign)
	{
		Sign->SetLinearColor(FLinearColor(0.9f, 0.85f, 0.15f));
	}

	Platform12Location = FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 100.f);
}

void ATriangleStationBuilder::BuildTrain()
{
	if (!GetWorld())
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATriangleTrain* Train = GetWorld()->SpawnActor<ATriangleTrain>(
		ATriangleTrain::StaticClass(),
		FVector(0.f, -50.f, 0.f),
		FRotator(0.f, 0.f, 0.f),
		Params);
	if (Train)
	{
		Train->Tags.Add(TEXT("WrongTrain"));
	}
}

void ATriangleStationBuilder::BuildCornerPoint()
{
	if (!GetWorld())
	{
		return;
	}

	// Upper corner of the far platform — where the Triangle first appears.
	FVector CornerLoc = FVector(-StationHalfX * 0.85f, 360.f, PlatformHeight + 10.f);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATriangleTrianglePoint* Point = GetWorld()->SpawnActor<ATriangleTrianglePoint>(
		ATriangleTrianglePoint::StaticClass(),
		CornerLoc,
		FRotator::ZeroRotator,
		Params);
	if (Point)
	{
		Point->ManifestId = FName(TEXT("StationCorner"));
	}
}

void ATriangleStationBuilder::BuildLighting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Cold ambient wash from above.
	ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FTransform::Identity);
	if (Sky)
	{
		Sky->GetLightComponent()->SetIntensity(0.25f);
		Sky->GetLightComponent()->SetLightColor(FLinearColor(0.25f, 0.28f, 0.32f));
		Sky->GetLightComponent()->SetCastShadows(false);
	}

	// Near-total darkness over the tracks.
	if (UPointLight* TrackFill = World->SpawnActor<APointLight>(
		APointLight::StaticClass(), FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator))
	{
		TrackFill->GetLightComponent()->SetIntensity(20.f);
		TrackFill->GetLightComponent()->SetLightColor(FLinearColor(0.05f, 0.05f, 0.07f));
		TrackFill->GetLightComponent()->SetAttenuationRadius(600.f);
		TrackFill->GetLightComponent()->SetCastShadows(false);
	}

	// Fog — thicker than the bedroom; underground, no breeze.
	AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(
		AExponentialHeightFog::StaticClass(), FTransform::Identity);
	if (Fog)
	{
		Fog->GetComponent()->SetFogDensity(0.008f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.03f, 0.035f, 0.045f));
	}
}
