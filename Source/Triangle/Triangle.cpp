#include "Triangle.h"

#include "Modules/ModuleManager.h"

#include "Characters/ATriangleCharacter.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/ATriangleGameMode.h"
#include "Dream/ATriangleDreamDirector.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleRoomBuilder.h"

DEFINE_LOG_CATEGORY(LogTriangle);
DEFINE_LOG_CATEGORY(LogTriangleDream);
DEFINE_LOG_CATEGORY(LogTriangleSave);
DEFINE_LOG_CATEGORY(LogTriangleInteraction);

class FTriangleGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FTriangleGameModule::StartupModule()
{
	// Materialise the core native classes at module start. UE 5.8.2 registers
	// classes by their prefix-stripped object name (e.g. "TriangleGameMode"),
	// so string lookups in config (GlobalDefaultGameMode) and elsewhere rely on
	// these being created before the engine resolves them.
	ATriangleGameMode::StaticClass();
	ATriangleCharacter::StaticClass();
	ATrianglePlayerController::StaticClass();
	ATriangleHUD::StaticClass();
	ATriangleRoomBuilder::StaticClass();
	ATriangleDreamDirector::StaticClass();
}

void FTriangleGameModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FTriangleGameModule, Triangle, "The Triangle");