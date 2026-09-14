#include "Dream/ATriangleDreamDirector.h"

#include "Triangle.h"
#include "Characters/ATriangleCharacter.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Dream/ATriangleTrianglePoint.h"
#include "World/ATriangleRoomBuilder.h"

#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ATriangleDreamDirector::ATriangleDreamDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void ATriangleDreamDirector::InitializeDream()
{
	if (!HasActorBegunPlay())
	{
		// Spawned during GameMode::StartPlay; BeginPlay runs later in the frame.
		UE_LOG(LogTriangleDream, Verbose, TEXT("Dream Director initializing."));
	}
	TimeSinceLastManifest = ManifestInterval; // give the player a quiet first beat
}

void ATriangleDreamDirector::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTriangleDream, Log, TEXT("The dream begins."));
}

ETriangleLucidityLevel ATriangleDreamDirector::GetCurrentLucidityLevel() const
{
	return PlayerLucidity ? PlayerLucidity->GetLucidityLevel() : ETriangleLucidityLevel::Low;
}

bool ATriangleDreamDirector::AcquireReferences()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	if (!PlayerActor)
	{
		PlayerActor = Cast<ATriangleCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
	}

	if (!PlayerLucidity && PlayerActor)
	{
		PlayerLucidity = PlayerActor->FindComponentByClass<ULTriangleLucidityComponent>();

		if (PlayerLucidity && !bBoundLucidityEvents)
		{
			PlayerLucidity->OnLucidityLevelChanged.AddDynamic(this, &ATriangleDreamDirector::HandleLucidityLevelChanged);
			bBoundLucidityEvents = true;
		}
	}

	if (!RoomBuilder)
	{
		RoomBuilder = Cast<ATriangleRoomBuilder>(UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()));
		if (RoomBuilder)
		{
			CornerLocation = RoomBuilder->GetTriangleCornerLocation();
		}
	}

	if (!CornerPoint)
	{
		CornerPoint = Cast<ATriangleTrianglePoint>(UGameplayStatics::GetActorOfClass(World, ATriangleTrianglePoint::StaticClass()));
	}

	return PlayerActor && PlayerLucidity && RoomBuilder && CornerPoint;
}

void ATriangleDreamDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!AcquireReferences())
	{
		return;
	}

	TimeSinceLastManifest += DeltaSeconds;
	WatchCorner(DeltaSeconds);
}

void ATriangleDreamDirector::WatchCorner(float DeltaSeconds)
{
	UCameraComponent* Camera = PlayerActor->GetFirstPersonCamera();
	if (!Camera)
	{
		return;
	}

	const FVector CameraLoc = Camera->GetComponentLocation();
	const FVector ToCorner = CornerLocation - CameraLoc;
	const float Distance = ToCorner.Size();
	const float Forwardness = FVector::DotProduct(Camera->GetForwardVector(), ToCorner.GetSafeNormal());

	const bool bWatching = Distance <= WatchRange && Forwardness >= WatchFovDotThreshold;
	if (bWatching)
	{
		TimeWatching += DeltaSeconds;

		// Recognition builds while the corner is held in view.
		PlayerLucidity->AddLucidity(WatchLucidityPerSecond * DeltaSeconds);

		// The point only appears once the gaze has been held a moment, and at
		// most once per manifest interval.
		if (TimeWatching >= 1.2f && TimeSinceLastManifest >= ManifestInterval && !(CornerPoint && CornerPoint->IsVisible()))
		{
			TriggerTriangleManifest(ManifestDuration);
			TimeSinceLastManifest = 0.f;
			TimeWatching = 0.f;
		}
	}
	else
	{
		TimeWatching = FMath::Max(0.f, TimeWatching - DeltaSeconds);
	}
}

void ATriangleDreamDirector::TriggerTriangleManifest(float Duration)
{
	if (!CornerPoint)
	{
		return;
	}
	CornerPoint->Manifest(Duration);
	UE_LOG(LogTriangleDream, Log, TEXT("Triangle manifest triggered (lucidity=%.1f)."), PlayerLucidity ? PlayerLucidity->GetLucidity() : -1.f);
}

void ATriangleDreamDirector::HandleLucidityLevelChanged(ETriangleLucidityLevel NewLevel)
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	switch (NewLevel)
	{
	case ETriangleLucidityLevel::Medium:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("Something in this room is too familiar.")), 3.f);
		break;

	case ETriangleLucidityLevel::High:
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You remember this place. You have always been here.")), 3.5f);
		break;

	case ETriangleLucidityLevel::Maximum:
		if (!bMaxReachedHandled)
		{
			bMaxReachedHandled = true;
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The dream knows you are awake.")), 4.f);
			// The point stops hiding at full lucidity.
			if (CornerPoint)
			{
				CornerPoint->Manifest(9999.f);
			}
		}
		break;

	default:
		break;
	}
}