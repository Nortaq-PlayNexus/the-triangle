#include "Lucidity/ULTriangleRecognitionComponent.h"

#include "Triangle.h"
#include "UObject/Class.h"

ULTriangleRecognitionComponent::ULTriangleRecognitionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ETriangleRecognitionLevel ULTriangleRecognitionComponent::LevelFor(float Value)
{
	if (Value >= 95.f)
	{
		return ETriangleRecognitionLevel::Master;
	}
	if (Value >= 75.f)
	{
		return ETriangleRecognitionLevel::Anticipating;
	}
	if (Value >= 50.f)
	{
		return ETriangleRecognitionLevel::Recognizing;
	}
	if (Value >= 25.f)
	{
		return ETriangleRecognitionLevel::Noticing;
	}
	return ETriangleRecognitionLevel::Unknowing;
}

ETriangleRecognitionLevel ULTriangleRecognitionComponent::GetRecognitionLevel() const
{
	return LevelFor(Recognition);
}

void ULTriangleRecognitionComponent::SetRecognition(float NewValue)
{
	const float Clamped = FMath::Clamp(NewValue, 0.f, 100.f);
	if (FMath::IsNearlyZero(Clamped - Recognition))
	{
		return;
	}

	const ETriangleRecognitionLevel PreviousLevel = GetRecognitionLevel();
	Recognition = Clamped;
	ApplyClamped();

	UE_LOG(LogTriangleDream, Log, TEXT("Recognition changed to %.1f"), Recognition);
	OnRecognitionChanged.Broadcast(Recognition);

	const ETriangleRecognitionLevel NewLevel = GetRecognitionLevel();
	if (NewLevel != PreviousLevel)
	{
		UE_LOG(LogTriangleDream, Log, TEXT("Recognition level -> %s"), *UEnum::GetValueAsString(NewLevel));
		OnRecognitionLevelChanged.Broadcast(NewLevel);

		if (NewLevel >= ETriangleRecognitionLevel::Anticipating)
		{
			// Nearing the top, the sense comes before the sight.
			OnSensePresence.Broadcast(TEXT("SenseHum"));
		}
	}
}

void ULTriangleRecognitionComponent::AddRecognition(float Amount, FName Reason)
{
	if (Amount <= 0.f)
	{
		return;
	}
	SetRecognition(Recognition + Amount);

	if (!Reason.IsNone() && !RecognisedIds.Contains(Reason))
	{
		RecognisedIds.Add(Reason);
		UE_LOG(LogTriangleDream, Log, TEXT("Encounter '%s' recorded in the pattern catalogue."), *Reason.ToString());
	}
}

void ULTriangleRecognitionComponent::RecordSighting(FName ManifestId)
{
	if (ManifestId.IsNone() || RecognisedIds.Contains(ManifestId))
	{
		return;
	}
	RecognisedIds.Add(ManifestId);
	UE_LOG(LogTriangleDream, Log, TEXT("Sighting '%s' recorded."), *ManifestId.ToString());
}

void ULTriangleRecognitionComponent::RestoreFromSave(float SavedRecognition)
{
	Recognition = 0.f;
	if (SavedRecognition > 0.f)
	{
		SetRecognition(SavedRecognition);
	}
}

void ULTriangleRecognitionComponent::ApplyClamped()
{
	Recognition = FMath::Clamp(Recognition, 0.f, 100.f);
}