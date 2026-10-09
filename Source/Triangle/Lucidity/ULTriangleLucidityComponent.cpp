#include "Lucidity/ULTriangleLucidityComponent.h"

#include "Triangle.h"
#include "UObject/Class.h"

ULTriangleLucidityComponent::ULTriangleLucidityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ETriangleLucidityLevel ULTriangleLucidityComponent::LevelFor(float Value)
{
	if (Value >= 90.f)
	{
		return ETriangleLucidityLevel::Maximum;
	}
	if (Value >= 60.f)
	{
		return ETriangleLucidityLevel::High;
	}
	if (Value >= 30.f)
	{
		return ETriangleLucidityLevel::Medium;
	}
	return ETriangleLucidityLevel::Low;
}

ETriangleLucidityLevel ULTriangleLucidityComponent::GetLucidityLevel() const
{
	return LevelFor(Lucidity);
}

void ULTriangleLucidityComponent::SetLucidity(float NewValue)
{
	if (!FMath::IsFinite(NewValue))
	{
		UE_LOG(LogTriangleDream, Warning, TEXT("Ignoring non-finite lucidity value."));
		return;
	}
	const float Clamped = FMath::Clamp(NewValue, 0.f, 100.f);
	if (FMath::IsNearlyZero(Clamped - Lucidity))
	{
		return;
	}

	const ETriangleLucidityLevel PreviousLevel = GetLucidityLevel();
	Lucidity = Clamped;

	UE_LOG(LogTriangleDream, Log, TEXT("Lucidity changed to %.1f"), Lucidity);
	OnLucidityChanged.Broadcast(Lucidity);

	const ETriangleLucidityLevel NewLevel = GetLucidityLevel();
	if (NewLevel != PreviousLevel)
	{
		UE_LOG(LogTriangleDream, Log, TEXT("Lucidity level -> %s"), *UEnum::GetValueAsString(NewLevel));
		OnLucidityLevelChanged.Broadcast(NewLevel);
	}

	if (!bMaxReached && Lucidity >= 100.f)
	{
		bMaxReached = true;
		LucidityMaxEvent.Broadcast();
	}
}

void ULTriangleLucidityComponent::AddLucidity(float Amount)
{
	if (FMath::IsFinite(Amount))
	{
		SetLucidity(Lucidity + Amount);
	}
}

void ULTriangleLucidityComponent::RestoreFromSave(float SavedLucidity)
{
	// Loading is state synchronisation, not progression. Assign directly so the
	// level and max delegates cannot replay story beats or awards.
	Lucidity = FMath::IsFinite(SavedLucidity) ? FMath::Clamp(SavedLucidity, 0.f, 100.f) : 0.f;
	bMaxReached = Lucidity >= 100.f;

	UE_LOG(LogTriangleDream, Log, TEXT("Lucidity restored to %.1f."), Lucidity);
	OnLucidityChanged.Broadcast(Lucidity);
}