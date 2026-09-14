#include "World/ATriangleDoor.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "UI/ATriangleHUD.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ATriangleDoor::ATriangleDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}
	// Door panel ~15 cm thick, ~100 cm wide, ~210 cm tall - matching the
	// front-wall gap built by the room builder.
	DoorMesh->SetRelativeScale3D(FVector(0.15f, 1.0f, 2.1f));
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

	Interactable = CreateDefaultSubobject<UTriangleInteractableComponent>(TEXT("Interactable"));
	Interactable->InteractionName = FText::FromString(TEXT("Door"));
	Interactable->InteractionAnchor = DoorMesh;
	Interactable->MaxInteractionRange = 230.f;

	ClosedRotation = GetActorRotation();
	OpenRotation = FRotator(0.f, -95.f, 0.f);
}

void ATriangleDoor::BeginPlay()
{
	Super::BeginPlay();
	Interactable->OnInteracted.AddDynamic(this, &ATriangleDoor::OnInteracted);
}

void ATriangleDoor::OnInteracted(APawn* Interactor)
{
	if (bIsOpen)
	{
		return;
	}

	ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(GetWorld());
	const bool bLucidEnough = Lucidity && Lucidity->CanManipulateDream();

	if (bLucidEnough)
	{
		Open(false);
		UE_LOG(LogTriangleDream, Log, TEXT("The door relented and opened into darkness."));
		if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld()))
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The door opens into darkness.")), 2.5f);
		}
	}
	else
	{
		if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld()))
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("It won't move. The dream is not done with you.")), 3.f);
		}
	}
}

void ATriangleDoor::Open(bool bInstant)
{
	bIsOpen = true;
	Interactable->bEnabled = false;
	Interactable->NotifyFocusEnd();

	if (bInstant)
	{
		SetActorRotation(OpenRotation);
		bSwinging = false;
		return;
	}

	bSwinging = true;
	SwingAlpha = 0.f;
	SetActorTickEnabled(true);
}

void ATriangleDoor::AdvanceSwing(float DeltaTime)
{
	if (!bSwinging)
	{
		return;
	}
	SwingAlpha = FMath::Min(1.f, SwingAlpha + DeltaTime / SwingDuration);
	SetActorRotation(FMath::Lerp(ClosedRotation, OpenRotation, SwingAlpha));

	if (SwingAlpha >= 1.f)
	{
		bSwinging = false;
		SetActorTickEnabled(false);
	}
}

void ATriangleDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AdvanceSwing(DeltaSeconds);
}