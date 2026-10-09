#include "Dream/UTriangleManifestationDefinition.h"

#include "Triangle.h"

float UTriangleManifestationDefinition::GetRandomDuration() const
{
	const float Min = FMath::IsFinite(MinDuration) ? FMath::Max(0.5f, MinDuration) : 0.5f;
	const float Max = FMath::IsFinite(MaxDuration) ? FMath::Max(Min, MaxDuration) : Min;
	return FMath::FRandRange(Min, Max);
}