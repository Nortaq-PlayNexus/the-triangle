#pragma once

#include "CoreMinimal.h"
#include "ETriangleDreamEnvironment.generated.h"

/** The two environments included in the current playable vertical slice. */
UENUM(BlueprintType)
enum class ETriangleDreamEnvironment : uint8
{
	ChildhoodBedroom,
	UndergroundStation
};
