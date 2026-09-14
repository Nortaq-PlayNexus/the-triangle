#pragma once

/**
 * Shared states for nightmare entities. Interpreted by each entity's own
 * behaviour logic; loose enough that entities that are "not really chasing"
 * (like The Floating Man) can map states to ambivalent behaviour.
 */
UENUM(BlueprintType)
enum class ETriangleNightmareEntityState : uint8
{
	Idle,
	Follow,
	Observe,
	Disappear,
	Reappear,
	LeadPlayer,
	Wait
};

#include "ENightmareEntityState.generated.h"