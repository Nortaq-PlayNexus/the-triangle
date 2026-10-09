#include "Dream/UTriangleFalseAwakeningManager.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "UI/ATriangleHUD.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

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

void UTriangleFalseAwakeningManager::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PendingDifferenceTimerHandle);
	}
	Super::Deinitialize();
}

bool UTriangleFalseAwakeningManager::TryWake()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	if (World->GetTimeSeconds() < NextWakeAllowedTime)
	{
		return false;
	}
	World->GetTimerManager().ClearTimer(PendingDifferenceTimerHandle);

	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World);

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

	if (LayerIndex >= 32)
	{
		if (PC && PC->GetTriangleHUD())
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("There is no deeper layer to find.")), 3.f);
		}
		return false;
	}

	// Layer 0: the player's first wake is the dream pretending for a single beat.
	const int32 Layer = LayerIndex;
	LayerIndex = FMath::Min(LayerIndex + 1, 32);

	if (PC && PC->GetTriangleHUD())
	{
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You wake up.")), 2.f);
	}

	// Then a small wrongness, deferred long enough to feel like the open eye.
	if (!DifferenceLines.IsEmpty())
	{
		const FText Difference = DifferenceLines.IsValidIndex(Layer) ? DifferenceLines[Layer] : DifferenceLines.Last();
		TWeakObjectPtr<UTriangleFalseAwakeningManager> WeakThis(this);
		TWeakObjectPtr<ATrianglePlayerController> WeakPC(PC);
		World->GetTimerManager().SetTimer(PendingDifferenceTimerHandle, [WeakThis, WeakPC, Difference, Layer]()
		{
			UTriangleFalseAwakeningManager* Manager = WeakThis.Get();
			ATrianglePlayerController* Controller = WeakPC.Get();
			if (!Manager)
			{
				return;
			}
			if (Controller && Controller->GetTriangleHUD())
			{
				Controller->GetTriangleHUD()->PushSubtitle(Difference, 4.f);
			}
			Manager->OnFalseAwakening.Broadcast(Layer);
		}, 2.2f, false);
	}
	else
	{
		OnFalseAwakening.Broadcast(Layer);
	}

	CompletionCount++;
	NextWakeAllowedTime = World->GetTimeSeconds() + 4.f;
	// The release: the dream withdraws, gets its breath back, and starts again.
	OnWakeResetRequested.Broadcast();

	UE_LOG(LogTriangleDream, Log, TEXT("False wake %d ('%s')."), Layer, DescriptorFor(Layer));
	return true;
}

void UTriangleFalseAwakeningManager::RestoreState(const int32 SavedLayer)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PendingDifferenceTimerHandle);
	}
	LayerIndex = FMath::Clamp(SavedLayer, 0, 32);
	CompletionCount = LayerIndex;
	NextWakeAllowedTime = 0.f;
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