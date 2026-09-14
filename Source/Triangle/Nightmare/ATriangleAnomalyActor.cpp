#include "Nightmare/ATriangleAnomalyActor.h"

#include "Triangle.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/Class.h"
#include "UObject/ConstructorHelpers.h"

FLinearColor ATriangleAnomalyActor::IntrusionColor = FLinearColor(0.02f, 0.03f, 0.045f);

ATriangleAnomalyActor::ATriangleAnomalyActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 0.f;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PrimaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimaryMesh"));
	PrimaryMesh->SetupAttachment(Root);
	PrimaryMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AuxMeshA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AuxMeshA"));
	AuxMeshA->SetupAttachment(Root);
	AuxMeshA->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AuxMeshB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AuxMeshB"));
	AuxMeshB->SetupAttachment(Root);
	AuxMeshB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorHiddenInGame(true);
}

UStaticMesh* ATriangleAnomalyActor::LoadMesh(const TCHAR* Path)
{
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, Path);
	return Mesh;
}

void ATriangleAnomalyActor::Configure(ETriangleAnomalyKind InKind, float InDuration, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	ActiveKind = InKind;
	Lifespan = FMath::Max(1.5f, InDuration);
	AgeSeconds = 0.f;
	bActive = true;

	BuildForKind(InKind);

	// Best-effort tint: the engine basic-shape material is parameterised (Color/
	// Neon). If the named parameter is absent on this engine version, SetVector-
	// ParameterValue is a silent no-op and the intrusion simply reads by shape.
	if (UMaterialInterface* Base = PrimaryMesh ? PrimaryMesh->GetMaterial(0) : nullptr)
	{
		UMaterialInstanceDynamic* Tint = UMaterialInstanceDynamic::Create(Base, this);
		if (Tint)
		{
			Tint->SetVectorParameterValue(TEXT("Color"), IntrusionColor);
			Tint->SetVectorParameterValue(TEXT("Neon"), IntrusionColor);
			Tint->SetScalarParameterValue(TEXT("Opacity"), 1.f);
			for (UStaticMeshComponent* Comp : { PrimaryMesh.Get(), AuxMeshA.Get(), AuxMeshB.Get() })
			{
				if (Comp)
				{
					Comp->SetMaterial(0, Tint);
				}
			}
		}
	}

	SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	SetActorHiddenInGame(false);
	ApplyAnimatedScale(0.001f);

	UE_LOG(LogTriangleDream, Log, TEXT("Anomaly '%s' cut into the dream for %.1fs."),
		*UEnum::GetDisplayValueAsText(ActiveKind).ToString(), Lifespan);
}

void ATriangleAnomalyActor::BuildForKind(ETriangleAnomalyKind Kind)
{
	UStaticMesh* Cone = LoadMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	UStaticMesh* Cube = LoadMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Sphere = LoadMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Hide every part first; each branch reuses the three components.
	PrimaryMesh->SetStaticMesh(Cone);
	PrimaryMesh->SetVisibility(false);
	AuxMeshA->SetVisibility(false);
	AuxMeshB->SetVisibility(false);

	switch (Kind)
	{
	case ETriangleAnomalyKind::TriangularGap:
		// A razor-thin dark wedge standing in the plane of a surface, like the
		// wall has been asked what is behind it and answered with a cut.
		PrimaryMesh->SetStaticMesh(Cone);
		PrimaryMesh->SetRelativeRotation(FRotator(90.f, 0.f, 25.f));
		PrimaryMesh->SetRelativeScale3D(FVector(0.04f, 0.04f, 1.7f));
		PrimaryMesh->SetVisibility(true);
		BuildScale = 1.f;
		break;

	case ETriangleAnomalyKind::CornerSplit:
		// A tall thin seam: the corner holds its breath, slightly apart.
		PrimaryMesh->SetStaticMesh(Cube);
		PrimaryMesh->SetRelativeRotation(FRotator(0.f, 45.f, 0.f));
		PrimaryMesh->SetRelativeScale3D(FVector(0.03f, 0.03f, 1.0f));
		PrimaryMesh->SetVisibility(true);
		BuildScale = 1.f;
		break;

	case ETriangleAnomalyKind::DistantBlackPoint:
		// A tiny black point far away. The oldest shape - a point.
		PrimaryMesh->SetStaticMesh(Sphere);
		PrimaryMesh->SetRelativeRotation(FRotator::ZeroRotator);
		PrimaryMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		PrimaryMesh->SetVisibility(true);
		BuildScale = 0.6f;
		break;

	case ETriangleAnomalyKind::TriangularDoorway:
		// Three dark blades forming a downward triangle against the wall: an
		// impossible door cut into the dream.
		PrimaryMesh->SetStaticMesh(Cone);
		PrimaryMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		PrimaryMesh->SetRelativeScale3D(FVector(2.4f, 0.03f, 0.03f));
		PrimaryMesh->SetVisibility(true);

		AuxMeshA->SetStaticMesh(Cone);
		AuxMeshA->SetRelativeRotation(FRotator(90.f, 0.f, 120.f));
		AuxMeshA->SetRelativeScale3D(FVector(2.4f, 0.03f, 0.03f));
		AuxMeshA->SetVisibility(true);

		AuxMeshB->SetStaticMesh(Cone);
		AuxMeshB->SetRelativeRotation(FRotator(90.f, 0.f, 240.f));
		AuxMeshB->SetRelativeScale3D(FVector(2.4f, 0.03f, 0.03f));
		AuxMeshB->SetVisibility(true);
		BuildScale = 1.f;
		break;

	default:
		break;
	}
}

void ATriangleAnomalyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bActive)
	{
		return;
	}

	AgeSeconds += DeltaSeconds;

	const float InEnd = FadeInTime;
	const float OutStart = Lifespan - FadeOutTime;

	if (AgeSeconds < InEnd)
	{
		ApplyAnimatedScale(FMath::Lerp(0.001f, 1.f, AgeSeconds / InEnd));
	}
	else if (AgeSeconds >= OutStart + 0.001f)
	{
		const float ExitT = FMath::Clamp((Lifespan - AgeSeconds) / FadeOutTime, 0.f, 1.f);
		ApplyAnimatedScale(FMath::Lerp(0.001f, 1.f, ExitT));
		if (ExitT <= 0.005f)
		{
			Finish();
		}
	}
}

void ATriangleAnomalyActor::ApplyAnimatedScale(float ScaleFactor)
{
	Root->SetRelativeScale3D(FVector(ScaleFactor * BuildScale));
}

void ATriangleAnomalyActor::Finish()
{
	bActive = false;
	SetActorHiddenInGame(true);
	UE_LOG(LogTriangleDream, Verbose, TEXT("Anomaly receded."));
	Destroy();
}

void ATriangleAnomalyActor::ShutDownNow()
{
	if (!bActive)
	{
		return;
	}
	bActive = false;
	SetActorHiddenInGame(true);
	Destroy();
}