#include "World/ATriangleNotebook.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATriangleNotebook::ATriangleNotebook()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cover"));
	CoverMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		CoverMesh->SetStaticMesh(CubeMesh.Object);
	}
	CoverMesh->SetRelativeScale3D(FVector(0.24f, 0.18f, 0.035f));

	Interactable = CreateDefaultSubobject<UTriangleInteractableComponent>(TEXT("Interactable"));
	Interactable->InteractionName = FText::FromString(TEXT("Notebook"));
	Interactable->InteractionAnchor = CoverMesh;
	Interactable->MaxInteractionRange = 200.f;
}

void ATriangleNotebook::BeginPlay()
{
	Super::BeginPlay();
	Interactable->OnInteracted.AddDynamic(this, &ATriangleNotebook::OnInteracted);
}

void ATriangleNotebook::OnInteracted(APawn* Interactor)
{
	InteractionCount++;

	ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(GetWorld());
	if (Lucidity)
	{
		Lucidity->AddLucidity(Lucidity->BaseGainPerRealization);
	}

	ShowPage(FMath::Min(InteractionCount - 1, 2));
	UE_LOG(LogTriangleDream, Log, TEXT("Notebook read (page %d/3)."), InteractionCount);
}

void ATriangleNotebook::ShowPage(int32 PageIndex)
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	const FText Pages[] =
	{
		FText::FromString(TEXT("\"Mostly blank. One page at the front has a triangle drawn in pencil.\"")),
		FText::FromString(TEXT("\"This is your handwriting. You don't remember writing it.\"")),
		FText::FromString(TEXT("\"The last note says: wake up.\"")),
	};
	PC->GetTriangleHUD()->PushSubtitle(Pages[FMath::Clamp(PageIndex, 0, 2)], 4.f);
}