#include "World/ATriangleDoor.h"

#include "Triangle.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "UI/ATriangleHUD.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATriangleDoor::ATriangleDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Movable);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetMobility(EComponentMobility::Movable);
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

	OpenRotation = FRotator(0.f, -95.f, 0.f);
}

void ATriangleDoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedRotation = GetActorRotation();
	OpenRotation = ClosedRotation + FRotator(0.f, -95.f, 0.f);
	SetActorTickEnabled(false);

	if (Interactable)
	{
		Interactable->OnInteracted.AddDynamic(this, &ATriangleDoor::OnInteracted);
	}
}

void ATriangleDoor::OnInteracted(APawn*)
{
	if (bSwinging)
	{
		return;
	}

	if (bIsOpen)
	{
		// Keep the component enabled after opening. A second deliberate press
		// is the seam where the environment flow requests the station cut.
		RequestTransition();
		return;
	}

	ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(GetWorld());
	const bool bLucidEnough = Lucidity && Lucidity->CanManipulateDream();

	if (bLucidEnough)
	{
		Open(false);
		UE_LOG(LogTriangleDream, Log, TEXT("The door relented and opened into darkness."));
		if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(GetWorld()))
		{
			HUD->PushSubtitle(FText::FromString(TEXT("The door opens into darkness.")), 2.5f);
		}
	}
	else if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(GetWorld()))
	{
		HUD->PushSubtitle(FText::FromString(TEXT("It won't move. The dream is not done with you.")), 3.f);
	}
}

void ATriangleDoor::Open(bool bInstant)
{
	if (bIsOpen)
	{
		if (bInstant)
		{
			SetActorRotation(OpenRotation);
			bSwinging = false;
			SetActorTickEnabled(false);
		}
		return;
	}

	bIsOpen = true;
	const float ConfiguredDuration = FMath::IsFinite(SwingDuration) ? SwingDuration : 0.f;
	if (bInstant || ConfiguredDuration <= KINDA_SMALL_NUMBER)
	{
		SetActorRotation(OpenRotation);
		bSwinging = false;
		SetActorTickEnabled(false);
		return;
	}

	bSwinging = true;
	SwingAlpha = 0.f;
	SetActorTickEnabled(true);
}

void ATriangleDoor::Close(const bool bInstant)
{
	(void)bInstant;
	bIsOpen = false;
	bSwinging = false;
	SetActorRotation(ClosedRotation);
	SetActorTickEnabled(false);
}

void ATriangleDoor::RequestTransition()
{
	if (!bIsOpen || bSwinging)
	{
		return;
	}

	if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(GetWorld()))
	{
		HUD->PushSubtitle(FText::FromString(TEXT("The darkness waits beyond.")), 2.5f);
	}
	const FVector BeyondLocation = GetBeyondLocation();
	UE_LOG(LogTriangleDream, Log, TEXT("Door transition requested; beyond location is %s."), *BeyondLocation.ToCompactString());
	OnTransitionRequested.Broadcast(this);
}

FVector ATriangleDoor::GetBeyondLocation() const
{
	// Use the closed orientation as the stable reference. The door rotates
	// around its own origin, so this point remains a useful destination after
	// the swing completes.
	const FRotator ReferenceRotation = HasActorBegunPlay() ? ClosedRotation : GetActorRotation();
	return GetActorLocation() + ReferenceRotation.RotateVector(BeyondOffset);
}

void ATriangleDoor::AdvanceSwing(float DeltaTime)
{
	if (!bSwinging)
	{
		SetActorTickEnabled(false);
		return;
	}

	const float SafeDuration = FMath::Max(FMath::IsFinite(SwingDuration) ? SwingDuration : 0.f, KINDA_SMALL_NUMBER);
	SwingAlpha = FMath::Min(1.f, SwingAlpha + DeltaTime / SafeDuration);
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
