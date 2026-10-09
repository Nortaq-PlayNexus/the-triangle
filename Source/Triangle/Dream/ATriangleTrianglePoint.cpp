#include "Dream/ATriangleTrianglePoint.h"

#include "Triangle.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATriangleTrianglePoint::ATriangleTrianglePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TriangleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriangleMesh"));
	TriangleMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		TriangleMesh->SetStaticMesh(ConeMesh.Object);
	}
	TriangleMesh->SetRelativeScale3D(FVector(0.16f, 0.16f, 0.16f));
	TriangleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// A faint teal glow - wrong-coloured for the room, quiet enough to miss.
	GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
	GlowLight->SetupAttachment(RootComponent);
	GlowLight->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
	GlowLight->SetIntensity(6.f);
	GlowLight->SetLightColor(FLinearColor(0.25f, 0.45f, 0.50f));
	GlowLight->SetAttenuationRadius(350.f);
	GlowLight->SetCastShadows(false);

	SetActorHiddenInGame(true);
	TriangleMesh->SetVisibility(false, true);
	GlowLight->SetVisibility(false);
}

void ATriangleTrianglePoint::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(!bPointVisible);
	TriangleMesh->SetVisibility(bPointVisible, true);
	GlowLight->SetVisibility(bPointVisible);
}

void ATriangleTrianglePoint::Manifest(float Duration)
{
	bPointVisible = true;
	SetActorHiddenInGame(false);
	TriangleMesh->SetVisibility(true, true);
	GlowLight->SetVisibility(true);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Triangle point '%s' manifesting for %.1fs."), *ManifestId.ToString(), Duration);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	World->GetTimerManager().ClearTimer(ManifestTimerHandle);
	World->GetTimerManager().SetTimer(ManifestTimerHandle, this, &ATriangleTrianglePoint::Hide, Duration, false);
}

void ATriangleTrianglePoint::Hide()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ManifestTimerHandle);
	}
	bPointVisible = false;
	SetActorHiddenInGame(true);
	TriangleMesh->SetVisibility(false, true);
	GlowLight->SetVisibility(false);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Triangle point '%s' receded."), *ManifestId.ToString());
}