#include "Dream/UTriangleManifestationDefinition.h"

#include "Triangle.h"

float UTriangleManifestationDefinition::GetRandomDuration() const
{
	return FMath::FRandRange(FMath::Max(0.5f, MinDuration), FMath::Max(0.5f, MaxDuration));
}