#include "World/ATriangleNotebook.h"

#include "Triangle.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "UI/ATriangleHUD.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

namespace TriangleNotebookPrivate
{
	constexpr int32 PageCount = 3;
}

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
	CoverMesh->SetCollisionProfileName(TEXT("BlockAll"));

	Interactable = CreateDefaultSubobject<UTriangleInteractableComponent>(TEXT("Interactable"));
	Interactable->InteractionName = FText::FromString(TEXT("Notebook"));
	Interactable->InteractionAnchor = CoverMesh;
	Interactable->MaxInteractionRange = 200.f;
}

void ATriangleNotebook::BeginPlay()
{
	Super::BeginPlay();
	if (Interactable)
	{
		Interactable->OnInteracted.AddDynamic(this, &ATriangleNotebook::OnInteracted);
	}
}

void ATriangleNotebook::OnInteracted(APawn*)
{
	InteractionCount = FMath::Max(0, InteractionCount);
	if (InteractionCount < MAX_int32)
	{
		++InteractionCount;
	}

	// Each read can reveal the next page, but reading an already-discovered
	// page must not grant the realization reward again.
	const bool bNewPage = InteractionCount > 0 && InteractionCount <= TriangleNotebookPrivate::PageCount;
	if (bNewPage)
	{
		if (ULTriangleLucidityComponent* Lucidity = FTriangleGameplayStatics::GetPlayerLucidity(GetWorld()))
		{
			Lucidity->AddLucidity(Lucidity->BaseGainPerRealization);
		}
	}

	const int32 PageIndex = FMath::Clamp(InteractionCount - 1, 0, TriangleNotebookPrivate::PageCount - 1);
	ShowPage(PageIndex);
	UE_LOG(LogTriangleDream, Log, TEXT("Notebook read (page %d/%d, new=%s)."),
		PageIndex + 1,
		TriangleNotebookPrivate::PageCount,
		bNewPage ? TEXT("true") : TEXT("false"));
}

void ATriangleNotebook::SetInteractionCount(int32 NewInteractionCount)
{
	InteractionCount = FMath::Max(0, NewInteractionCount);
}

void ATriangleNotebook::ResetInteractionCount()
{
	InteractionCount = 0;
}

void ATriangleNotebook::ShowPage(int32 PageIndex)
{
	const FText Pages[] =
	{
		FText::FromString(TEXT("\"Mostly blank. One page at the front has a triangle drawn in pencil.\"")),
		FText::FromString(TEXT("\"This is your handwriting. You don't remember writing it.\"")),
		FText::FromString(TEXT("\"The last note says: wake up.\"")),
	};

	if (ATriangleHUD* HUD = FTriangleGameplayStatics::GetTriangleHUD(GetWorld()))
	{
		HUD->PushSubtitle(Pages[FMath::Clamp(PageIndex, 0, TriangleNotebookPrivate::PageCount - 1)], 4.f);
	}
}
