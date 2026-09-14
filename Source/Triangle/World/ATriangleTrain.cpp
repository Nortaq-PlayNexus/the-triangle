#include "World/ATriangleTrain.h"

#include "Triangle.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ATriangleTrain::ATriangleTrain()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	SetRootComponent(BodyMesh);

	DoorL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorL"));
	DoorL->SetupAttachment(BodyMesh);

	DoorR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorR"));
	DoorR->SetupAttachment(BodyMesh);

	WindowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Window"));
	WindowMesh->SetupAttachment(BodyMesh);

	InteriorLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("InteriorLight"));
	InteriorLight->SetupAttachment(BodyMesh);

	// Body: a long, low box. The wrong proportions — too tall, too boxy.
	BodyMesh->SetBoxExtent(FVector(200.f, 90.f, 55.f));
	BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	// Doors: thin panels at the sides, slightly inset.
	DoorL->SetBoxExtent(FVector(80.f, 3.f, 70.f));
	DoorL->SetRelativeLocation(FVector(165.f, 86.f, 40.f));
	DoorL->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	DoorR->SetBoxExtent(FVector(80.f, 3.f, 70.f));
	DoorR->SetRelativeLocation(FVector(-165.f, -86.f, 40.f));

	// Windows: a continuous dark strip along the side.
	WindowMesh->SetBoxExtent(FVector(120.f, 2.f, 30.f));
	WindowMesh->SetRelativeLocation(FVector(0.f, 86.f, 60.f));
	WindowMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	// Interior light — a sickly cold glow that never quite illuminates.
	InteriorLight->SetIntensity(60.f);
	InteriorLight->SetLightColor(FLinearColor(0.35f, 0.38f, 0.42f));
	InteriorLight->SetAttenuationRadius(350.f);
	InteriorLight->SetCastShadows(false);
	InteriorLight->SetVisibility(false, true);

	SetActorHiddenInGame(false);
}

void ATriangleTrain::BeginPlay()
{
	Super::BeginPlay();

	// Dark wrong-colour body — charcoal with a faint blue wrongness.
	BodyMesh->SetLinearColor(FLinearColor(0.08f, 0.09f, 0.11f));
	// Slightly lighter doors (read as panels).
	DoorL->SetLinearColor(FLinearColor(0.12f, 0.13f, 0.15f));
	DoorR->SetLinearColor(FLinearColor(0.12f, 0.13f, 0.15f));
	// Windows are darker — voids, not glass.
	WindowMesh->SetLinearColor(FLinearColor(0.01f, 0.01f, 0.02f));

	SetMobility(EComponentMobility::Movable);
	BodyMesh->SetMobility(EComponentMobility::Movable);
	DoorL->SetMobility(EComponentMobility::Movable);
	DoorR->SetMobility(EComponentMobility::Movable);
	WindowMesh->SetMobility(EComponentMobility::Movable);
}

void ATriangleTrain::SetDoorsOpen(bool bOpen)
{
	bDoorsOpen = bOpen;
	if (DoorL && DoorR)
	{
		DoorL->SetVisibility(bOpen, true);
		DoorR->SetVisibility(bOpen, true);
	}
	if (InteriorLight)
	{
		InteriorLight->SetVisibility(bOpen, true);
	}
}
