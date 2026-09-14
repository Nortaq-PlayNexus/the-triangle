#pragma once

#include "CoreMinimal.h"
#include "ETriangleManifestationKind.generated.h"

/**
 * The possible bodies The Triangle chooses to appear in.
 *
 * The Triangle has no single body - it manifests as intrusions into the dream:
 * a point in a corner, a razor gap cut into a wall, a corner set slightly
 * apart from itself, a distant black point, an impossible doorway - or
 * nothing at all, only the hum. The player's recognition of the underlying
 * pattern is what makes these read as the same thing.
 */
UENUM(BlueprintType)
enum class ETriangleManifestationKind : uint8
{
	/** Existing corner point - the first learned shape. */
	Point,
	/** Presence with no visible form at all (the hum, an absence). */
	HumOnly,
	/** Razor-thin triangular gap cut into a surface. */
	TriangularGap,
	/** A vertical corner pushed slightly apart from itself. */
	CornerSplit,
	/** A tiny black point hanging in the distance. */
	DistantBlackPoint,
	/** An impossible triangular doorway cut into a wall. */
	TriangularDoorway,
	/** Reserved for the human form (mother) - not spawned by the slice. */
	HumanForm
};

/** Which visual the anomaly system should raise for a manifestation. */
UENUM(BlueprintType)
enum class ETriangleAnomalyKind : uint8
{
	None,
	TriangularGap,
	CornerSplit,
	DistantBlackPoint,
	TriangularDoorway
};