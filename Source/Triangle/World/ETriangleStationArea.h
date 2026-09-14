#pragma once

#include "CoreMinimal.h"
#include "ETriangleStationArea.generated.h"

UENUM(BlueprintType)
enum class ETriangleStationArea : uint8
{
	/** Open platform where passengers wait. */
	Platform,
	/** The track bed between platforms — dark, narrow. */
	Track,
	/** The concourse / mezzanine level above the platforms. */
	Concourse,
	/** Tunnel portal leading deeper into the underground. */
	Tunnel
};
