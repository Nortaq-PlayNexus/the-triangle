#include "World/ATriangleStationBuilder.h"

#include "World/ATriangleTrain.h"
#include "Dream/ATriangleTrianglePoint.h"
#include "Triangle.h"

#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ATriangleStationBuilder::ATriangleStationBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
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
	SpawnedDreamStart = Cast<APlayerStart>(SpawnTrackedActor(
		APlayerStart::StaticClass(),
		FTransform(FRotator(0.f, -90.f, 0.f), FVector(0.f, 250.f, PlatformHeight + 100.f))));
	if (SpawnedDreamStart)
	{
		SpawnedDreamStart->Tags.Add(TEXT("DreamStart"));
	}

	UE_LOG(LogTriangle, Log, TEXT("Underground station assembled."));
}

void ATriangleStationBuilder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// The station is assembled at runtime. Destroy every piece explicitly so
	// replacing the environment cannot leave orphaned lights, props, or starts.
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

FVector ATriangleStationBuilder::GetPlatform12Location() const
{
	return GetActorTransform().TransformPosition(Platform12Location + FVector(0.f, 0.f, StationElevation));
}

AActor* ATriangleStationBuilder::SpawnTrackedActor(UClass* ActorClass, const FTransform& Transform)
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
	const FVector DesignLocation = Transform.GetLocation() + FVector(0.f, 0.f, StationElevation);
	const FTransform WorldTransform(
		BuilderTransform.GetRotation() * Transform.GetRotation(),
		BuilderTransform.TransformPosition(DesignLocation),
		BuilderTransform.GetScale3D() * Transform.GetScale3D());
	AActor* Actor = World->SpawnActor<AActor>(ActorClass, WorldTransform, SpawnParameters);
	if (Actor)
	{
		SpawnedActors.Add(Actor);
	}
	return Actor;
}

UStaticMesh* ATriangleStationBuilder::GetCubeMesh()
{
	static TObjectPtr<UStaticMesh> CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	return CubeMesh.Get();
}

UStaticMeshComponent* ATriangleStationBuilder::AddBox(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(SpawnTrackedActor(
		AStaticMeshActor::StaticClass(), FTransform(Rotation, Location)));
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
		if (APointLight* FL = Cast<APointLight>(SpawnTrackedActor(
			APointLight::StaticClass(),
			FTransform(FRotator::ZeroRotator, FVector(Pos.X, Pos.Y, Pos.Z - 15.f)))))
		{
			if (UPointLightComponent* LightComponent = Cast<UPointLightComponent>(FL->GetLightComponent()))
			{
				LightComponent->SetIntensity(2200.f);
				LightComponent->SetLightColor(FLinearColor(0.85f, 0.88f, 0.92f));
				LightComponent->SetAttenuationRadius(800.f);
				LightComponent->SetCastShadows(false);
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
	// Four broad amber pools are enough to keep the platform edge readable.
	// The previous 26 tiny point lights looked richer in theory but were a poor
	// trade on a GTX 1080's forward-lit SM5 path.
	constexpr float LightPositions[] = { -350.f, 350.f };
	for (const float X : LightPositions)
	{
		if (APointLight* LeftEdge = Cast<APointLight>(SpawnTrackedActor(
			APointLight::StaticClass(),
			FTransform(FRotator::ZeroRotator, FVector(X, -148.f, PlatformHeight + 8.f)))))
		{
			if (UPointLightComponent* LightComponent = Cast<UPointLightComponent>(LeftEdge->GetLightComponent()))
			{
				LightComponent->SetIntensity(55.f);
				LightComponent->SetLightColor(FLinearColor(0.6f, 0.35f, 0.05f));
				LightComponent->SetAttenuationRadius(180.f);
				LightComponent->SetCastShadows(false);
			}
		}

		if (APointLight* RightEdge = Cast<APointLight>(SpawnTrackedActor(
			APointLight::StaticClass(),
			FTransform(FRotator::ZeroRotator, FVector(X, 148.f, PlatformHeight + 8.f)))))
		{
			if (UPointLightComponent* LightComponent = Cast<UPointLightComponent>(RightEdge->GetLightComponent()))
			{
				LightComponent->SetIntensity(55.f);
				LightComponent->SetLightColor(FLinearColor(0.6f, 0.35f, 0.05f));
				LightComponent->SetAttenuationRadius(180.f);
				LightComponent->SetCastShadows(false);
			}
		}
	}
}

void ATriangleStationBuilder::BuildPlatform12Marker()
{
	// Platform 12 sign post.
	AddBox(FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 90.f),
		FVector(0.08f, 0.08f, 1.8f));

	// The sign itself: thin across X, wide across the platform, and tall in Z.
	UStaticMeshComponent* Sign = AddBox(FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 140.f),
		FVector(0.06f, 1.2f, 0.6f));
	if (Sign)
	{
		Sign->SetLinearColor(FLinearColor(0.9f, 0.85f, 0.15f));
	}

	// BasicShapes cannot show the number 12, so use Unreal's built-in text
	// renderer. The actor faces back into the station rather than into the wall.
	if (ATextRenderActor* SignText = Cast<ATextRenderActor>(SpawnTrackedActor(
		ATextRenderActor::StaticClass(),
		FTransform(FRotator(0.f, 180.f, 0.f), FVector(StationHalfX - 66.f, -140.f, PlatformHeight + 140.f)))))
	{
		if (UTextRenderComponent* TextComponent = SignText->GetTextRender())
		{
			TextComponent->SetText(FText::FromString(TEXT("12")));
			TextComponent->SetHorizontalAlignment(EHTA_Center);
			TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
			TextComponent->SetWorldSize(52.f);
			TextComponent->SetTextRenderColor(FColor(230, 216, 38));
		}
	}

	Platform12Location = FVector(StationHalfX - 60.f, -140.f, PlatformHeight + 100.f);
}

void ATriangleStationBuilder::BuildTrain()
{
	ATriangleTrain* Train = Cast<ATriangleTrain>(SpawnTrackedActor(
		ATriangleTrain::StaticClass(),
		FTransform(FRotator::ZeroRotator, FVector(0.f, 60.f, 0.f))));
	if (Train)
	{
		Train->Tags.Add(TEXT("WrongTrain"));
	}
}

void ATriangleStationBuilder::BuildCornerPoint()
{
	// Upper corner of the far platform — where the Triangle first appears.
	const FVector CornerLoc = FVector(-StationHalfX * 0.85f, 360.f, PlatformHeight + 10.f);
	if (ATriangleTrianglePoint* Point = Cast<ATriangleTrianglePoint>(SpawnTrackedActor(
		ATriangleTrianglePoint::StaticClass(),
		FTransform(FRotator::ZeroRotator, CornerLoc))))
	{
		Point->ManifestId = FName(TEXT("StationCorner"));
	}
}

void ATriangleStationBuilder::BuildLighting()
{
	// Cold ambient wash from above.
	if (ASkyLight* Sky = Cast<ASkyLight>(SpawnTrackedActor(ASkyLight::StaticClass(), FTransform::Identity)))
	{
		if (ULightComponent* LightComponent = Sky->GetLightComponent())
		{
			LightComponent->SetIntensity(0.25f);
			LightComponent->SetLightColor(FLinearColor(0.25f, 0.28f, 0.32f));
			LightComponent->SetCastShadows(false);
		}
	}

	// Near-total darkness over the tracks.
	if (APointLight* TrackFill = Cast<APointLight>(SpawnTrackedActor(
		APointLight::StaticClass(), FTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, 100.f)))))
	{
		if (UPointLightComponent* LightComponent = Cast<UPointLightComponent>(TrackFill->GetLightComponent()))
		{
			LightComponent->SetIntensity(20.f);
			LightComponent->SetLightColor(FLinearColor(0.05f, 0.05f, 0.07f));
			LightComponent->SetAttenuationRadius(600.f);
			LightComponent->SetCastShadows(false);
		}
	}

	// Fog — thicker than the bedroom; underground, no breeze.
	if (AExponentialHeightFog* Fog = Cast<AExponentialHeightFog>(SpawnTrackedActor(
		AExponentialHeightFog::StaticClass(), FTransform::Identity)))
	{
		if (UExponentialHeightFogComponent* FogComponent = Cast<UExponentialHeightFogComponent>(Fog->GetComponent()))
		{
			FogComponent->SetFogDensity(0.008f);
			FogComponent->SetFogInscatteringColor(FLinearColor(0.03f, 0.035f, 0.045f));
		}
	}
}
