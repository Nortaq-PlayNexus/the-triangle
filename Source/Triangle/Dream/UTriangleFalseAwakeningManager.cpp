#include "Dream/UTriangleFalseAwakeningManager.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "UI/ATriangleHUD.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"

void UTriangleFalseAwakeningManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// The first wakes are nearly identical to being awake - only one small
	// wrongness separates them. Every difference line is a "how does the player
	// recognise it" beat, not a jumpscare.
	if (DifferenceLines.Num() == 0)
	{
		DifferenceLines.Add(FText::FromString(TEXT("The clock still says twelve.")));
		DifferenceLines.Add(FText::FromString(TEXT("The moon is on the wrong side of the window.")));
		DifferenceLines.Add(FText::FromString(TEXT("The hallway is longer than it was.")));
		DifferenceLines.Add(FText::FromString(TEXT("The hum is still here.")));
		DifferenceLines.Add(FText::FromString(TEXT("You are still ten years old.")));
	}
}

bool UTriangleFalseAwakeningManager::TryWake()
{
	UWorld* World = GetWorld();
	ATrianglePlayerController* PC = World ? FTriangleGameplayStatics::GetTrianglePlayerController(World) : nullptr;

	float PlayerRecognition = 0.f;
	bool bHasRecognition = false;
	if (PC && PC->GetPawn())
	{
		if (ULTriangleRecognitionComponent* Recog = PC->GetPawn()->FindComponentByClass<ULTriangleRecognitionComponent>())
		{
			PlayerRecognition = Recog->GetRecognition();
			bHasRecognition = true;
		}
	}

	// The dream refuses to let go of players who cannot tell they are in it.
	if (!bHasRecognition || PlayerRecognition < 75.f)
	{
		if (PC && PC->GetTriangleHUD())
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You close your eyes. The dream does not let go.")), 3.f);
		}
		return false;
	}

	// Layer 0: the player's first wake is the dream pretending for a single beat.
	const int32 Layer = LayerIndex;
	LayerIndex++;

	if (PC && PC->GetTriangleHUD())
	{
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You wake up.")), 2.f);

		// Then a small wrongness, deferred long enough to feel like the open eye.
		if (World)
		{
			const FText Difference = DifferenceLines.IsValidIndex(Layer) ? DifferenceLines[Layer] : DifferenceLines.Last();
			FTimerHandle Handle;
			World->GetTimerManager().SetTimer(Handle, [PC, Difference, Layer, this]()
			{
				if (PC && PC->GetTriangleHUD())
				{
					PC->GetTriangleHUD()->PushSubtitle(Difference, 4.f);
				}
				OnFalseAwakening.Broadcast(Layer);
			}, 2.2f, false);
		}
	}

	CompletionCount++;
	// The release: the dream withdraws, gets its breath back, and starts again.
	OnWakeResetRequested.Broadcast();

	UE_LOG(LogTriangleDream, Log, TEXT("False wake %d ('%s')."), Layer, DescriptorFor(Layer));
	return true;
}

const TCHAR* UTriangleFalseAwakeningManager::DescriptorFor(int32 Layer)
{
	switch (Layer)
	{
	case 0: return TEXT("first layer: almost nothing");
	case 1: return TEXT("second layer: one thing wrong");
	case 2: return TEXT("third layer: several things wrong");
	case 3: return TEXT("fourth layer: the love is wrong");
	default: return TEXT("deeper: the shapes are sibling-shaped");
	}
}