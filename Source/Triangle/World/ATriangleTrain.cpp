#include "World/ATriangleTrain.h"

#include "Triangle.h"
#include "Interaction/UTriangleInteractableComponent.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATriangleTrain::ATriangleTrain()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	BodyMesh->SetMobility(EComponentMobility::Movable);
	SetRootComponent(BodyMesh);

	DoorL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorL"));
	DoorL->SetMobility(EComponentMobility::Movable);
	DoorL->SetupAttachment(BodyMesh);

	DoorR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorR"));
	DoorR->SetMobility(EComponentMobility::Movable);
	DoorR->SetupAttachment(BodyMesh);

	WindowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Window"));
	WindowMesh->SetMobility(EComponentMobility::Movable);
	WindowMesh->SetupAttachment(BodyMesh);

	InteriorAperture = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteriorAperture"));
	InteriorAperture->SetMobility(EComponentMobility::Movable);
	InteriorAperture->SetupAttachment(BodyMesh);

	InteriorLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("InteriorLight"));
	InteriorLight->SetMobility(EComponentMobility::Movable);
	InteriorLight->SetupAttachment(BodyMesh);

	// Every visible part of the wrong train uses the engine cube. The body is
	// deliberately overlong and boxy rather than using a bespoke train asset.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		DoorL->SetStaticMesh(CubeMesh.Object);
		DoorR->SetStaticMesh(CubeMesh.Object);
		WindowMesh->SetStaticMesh(CubeMesh.Object);
		InteriorAperture->SetStaticMesh(CubeMesh.Object);
	}

	// Body: a long, low box. The wrong proportions — too tall, too boxy.
	BodyMesh->SetRelativeScale3D(FVector(4.f, 1.8f, 1.1f));
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));

	// Doors: thin panels just proud of the body sides.
	DoorL->SetRelativeScale3D(FVector(1.6f, 0.06f, 1.4f));
	DoorL->SetRelativeLocation(FVector(165.f, 93.f, 40.f));
	DoorL->SetCollisionProfileName(TEXT("BlockAll"));

	DoorR->SetRelativeScale3D(FVector(1.6f, 0.06f, 1.4f));
	DoorR->SetRelativeLocation(FVector(-165.f, -93.f, 40.f));
	DoorR->SetCollisionProfileName(TEXT("BlockAll"));

	// Windows: a continuous dark strip just proud of the side.
	WindowMesh->SetRelativeScale3D(FVector(2.4f, 0.04f, 0.6f));
	WindowMesh->SetRelativeLocation(FVector(0.f, 93.f, 60.f));
	WindowMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// The body is a solid BasicShape, so a black inset supplies the visual
	// depth of an open carriage without requiring a bespoke train mesh.
	InteriorAperture->SetRelativeScale3D(FVector(1.6f, 0.02f, 1.4f));
	InteriorAperture->SetRelativeLocation(FVector(165.f, 91.5f, 40.f));
	InteriorAperture->SetCollisionProfileName(TEXT("NoCollision"));
	InteriorAperture->SetVisibility(false, true);

	// Interior light — a sickly cold glow that never quite illuminates.
	InteriorLight->SetRelativeLocation(FVector(165.f, 105.f, 45.f));
	InteriorLight->SetIntensity(60.f);
	InteriorLight->SetLightColor(FLinearColor(0.35f, 0.38f, 0.42f));
	InteriorLight->SetAttenuationRadius(350.f);
	InteriorLight->SetCastShadows(false);
	InteriorLight->SetVisibility(false, true);

	Interactable = CreateDefaultSubobject<UTriangleInteractableComponent>(TEXT("Interactable"));
	Interactable->InteractionName = FText::FromString(TEXT("Wrong Train"));
	Interactable->InteractionAnchor = BodyMesh;
	Interactable->MaxInteractionRange = 500.f;
}

void ATriangleTrain::BeginPlay()
{
	Super::BeginPlay();

	// Dark wrong-colour body — charcoal with a faint blue wrongness.
	if (BodyMesh)
	{
		BodyMesh->SetLinearColor(FLinearColor(0.08f, 0.09f, 0.11f));
	}
	// Slightly lighter doors (read as panels).
	if (DoorL)
	{
		DoorL->SetLinearColor(FLinearColor(0.12f, 0.13f, 0.15f));
	}
	if (DoorR)
	{
		DoorR->SetLinearColor(FLinearColor(0.12f, 0.13f, 0.15f));
	}
	// Windows are darker — voids, not glass.
	if (WindowMesh)
	{
		WindowMesh->SetLinearColor(FLinearColor(0.01f, 0.01f, 0.02f));
	}
	if (InteriorAperture)
	{
		InteriorAperture->SetLinearColor(FLinearColor(0.002f, 0.002f, 0.004f));
	}

	if (Interactable)
	{
		Interactable->OnInteracted.AddDynamic(this, &ATriangleTrain::OnInteracted);
	}

	// Apply the authored closed state even if the actor was streamed or loaded
	// with a different component visibility state.
	SetDoorsOpen(bDoorsOpen);
}

void ATriangleTrain::OnInteracted(APawn*)
{
	ToggleDoors();
	UE_LOG(LogTriangle, Log, TEXT("Wrong train doors toggled (open=%s)."), bDoorsOpen ? TEXT("true") : TEXT("false"));
}

void ATriangleTrain::ToggleDoors()
{
	SetDoorsOpen(!bDoorsOpen);
}

void ATriangleTrain::SetDoorsOpen(bool bOpen)
{
	bDoorsOpen = bOpen;

	// Closed doors remain visible and block the opening. Opening removes both
	// the visual panels and their collision, revealing the dark interior.
	const bool bDoorPanelsVisible = !bOpen;
	if (DoorL)
	{
		DoorL->SetVisibility(bDoorPanelsVisible, true);
		DoorL->SetCollisionEnabled(bDoorPanelsVisible
			? ECollisionEnabled::QueryAndPhysics
			: ECollisionEnabled::NoCollision);
	}
	if (DoorR)
	{
		DoorR->SetVisibility(bDoorPanelsVisible, true);
		DoorR->SetCollisionEnabled(bDoorPanelsVisible
			? ECollisionEnabled::QueryAndPhysics
			: ECollisionEnabled::NoCollision);
	}
	if (InteriorAperture)
	{
		InteriorAperture->SetVisibility(bOpen, true);
	}
	if (InteriorLight)
	{
		InteriorLight->SetVisibility(bOpen, true);
	}
	if (Interactable)
	{
		Interactable->InteractionName = bOpen
			? FText::FromString(TEXT("Close Wrong Train"))
			: FText::FromString(TEXT("Open Wrong Train"));
	}
}
