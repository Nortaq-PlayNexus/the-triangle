#include "Save/UTriangleSaveGame.h"

#include "Triangle.h"

const int32 UTriangleSaveGame::CurrentSchemaVersion = 2;

UTriangleSaveGame::UTriangleSaveGame()
{
	// Leave new objects at the legacy sentinel. The save boundary stamps the
	// current schema, allowing old serialized objects to be detected on load.
	SchemaVersion = 0;
	PlayerTransform = FTransform::Identity;
	PlayerTransformInEnvironment = FTransform::Identity;
	bHasPlayerTransformInEnvironment = false;
}