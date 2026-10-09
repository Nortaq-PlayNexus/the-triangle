#if WITH_DEV_AUTOMATION_TESTS

#include "Dream/UTriangleManifestationDefinition.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "Misc/AutomationTest.h"
#include "Save/UTriangleSaveGame.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTriangleManifestationDurationTest,
	"TheTriangle.Core.ManifestationDuration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTriangleManifestationDurationTest::RunTest(const FString& Parameters)
{
	UTriangleManifestationDefinition* Definition = NewObject<UTriangleManifestationDefinition>();
	Definition->MinDuration = 4.f;
	Definition->MaxDuration = 7.f;
	for (int32 Index = 0; Index < 100; ++Index)
	{
		const float Duration = Definition->GetRandomDuration();
		TestTrue(TEXT("Duration respects its minimum"), Duration >= Definition->MinDuration);
		TestTrue(TEXT("Duration respects its maximum"), Duration <= Definition->MaxDuration);
	}

	Definition->MinDuration = 9.f;
	Definition->MaxDuration = 2.f;
	TestEqual(TEXT("Inverted duration bounds are normalized"), Definition->GetRandomDuration(), 9.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTriangleRecognitionPersistenceTest,
	"TheTriangle.Core.RecognitionPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTriangleRecognitionPersistenceTest::RunTest(const FString& Parameters)
{
	ULTriangleRecognitionComponent* Recognition = NewObject<ULTriangleRecognitionComponent>();
	const FName FirstPattern(TEXT("CornerPoint"));
	const FName SecondPattern(TEXT("ClockTwelve"));
	Recognition->AddRecognition(10.f, FirstPattern);
	Recognition->AddRecognition(10.f, FirstPattern);
	TestEqual(TEXT("Duplicate sightings are recorded once"), Recognition->GetRecognizedPatternIds().Num(), 1);

	TArray<FName> SavedPatterns;
	SavedPatterns.Add(FirstPattern);
	SavedPatterns.Add(SecondPattern);
	SavedPatterns.Add(FirstPattern);
	SavedPatterns.Add(NAME_None);
	Recognition->RestoreFromSaveWithCatalogue(80.f, SavedPatterns);
	TestEqual(TEXT("Saved recognition is restored"), Recognition->GetRecognition(), 80.f);
	TestEqual(TEXT("Catalogue is restored and deduplicated"), Recognition->GetRecognizedPatternIds().Num(), 2);
	TestTrue(TEXT("Anticipating threshold is restored"), Recognition->CanAttemptWake());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTriangleLucidityPersistenceTest,
	"TheTriangle.Core.LucidityPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTriangleLucidityPersistenceTest::RunTest(const FString& Parameters)
{
	ULTriangleLucidityComponent* Lucidity = NewObject<ULTriangleLucidityComponent>();
	Lucidity->SetLucidity(95.f);
	Lucidity->RestoreFromSave(45.f);
	TestEqual(TEXT("Saved lucidity is clamped and restored"), Lucidity->GetLucidity(), 45.f);
	TestTrue(TEXT("Restored level is derived without progression"), Lucidity->GetLucidityLevel() == ETriangleLucidityLevel::Medium);

	Lucidity->RestoreFromSave(1000.f);
	TestEqual(TEXT("Out-of-range lucidity is clamped"), Lucidity->GetLucidity(), 100.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTriangleSaveSchemaTest,
	"TheTriangle.Core.SaveSchema",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTriangleSaveSchemaTest::RunTest(const FString& Parameters)
{
	UTriangleSaveGame* Save = NewObject<UTriangleSaveGame>();
	TestEqual(TEXT("New save objects use the legacy sentinel until the save boundary"), Save->SchemaVersion, 0);
	TestTrue(TEXT("New save transforms are finite"), !Save->PlayerTransform.ContainsNaN());
	TestTrue(TEXT("New saves defer environment-relative transform authority"), !Save->bHasPlayerTransformInEnvironment);
	TestTrue(TEXT("New relative transforms are finite"), !Save->PlayerTransformInEnvironment.ContainsNaN());
	TestTrue(TEXT("New saves start without a wake layer"), Save->FalseAwakeningLayer == 0);
	return true;
}

#endif
