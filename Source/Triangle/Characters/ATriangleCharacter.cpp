#include "Characters/ATriangleCharacter.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Save/UTriangleSaveGame.h"
#include "Save/UTriangleSaveSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "UObject/UObjectGlobals.h"

ATriangleCharacter::ATriangleCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	GetCharacterMovement()->MaxWalkSpeed = 320.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 110.f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, EyesHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;

	Lucidity = CreateDefaultSubobject<ULTriangleLucidityComponent>(TEXT("Lucidity"));
}

void ATriangleCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnsureInputBindings();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScanTimerHandle, this, &ATriangleCharacter::ScanForInteractable, InteractionScanInterval, true, 0.1f);
	}
}

void ATriangleCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FocusedInteractable && !FocusedInteractable->IsValidLowLevel())
	{
		SetFocusedInteractable(nullptr);
	}
}

void ATriangleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ATriangleCharacter::Move);
		EnhancedInput->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ATriangleCharacter::Look);
		EnhancedInput->BindAction(ActionJump, ETriggerEvent::Started, this, &ATriangleCharacter::StartJump);
		EnhancedInput->BindAction(ActionJump, ETriggerEvent::Completed, this, &ATriangleCharacter::StopJump);
		EnhancedInput->BindAction(ActionInteract, ETriggerEvent::Started, this, &ATriangleCharacter::PressInteract);
		EnhancedInput->BindAction(ActionSave, ETriggerEvent::Started, this, &ATriangleCharacter::QuickSave);
		EnhancedInput->BindAction(ActionLoad, ETriggerEvent::Started, this, &ATriangleCharacter::QuickLoad);
	}
}

void ATriangleCharacter::EnsureInputBindings()
{
	if (InputContext && ActionMove)
	{
		return;
	}

	ActionMove = NewObject<UInputAction>(this);
	ActionMove->ValueType = EInputActionValueType::Axis2D;

	ActionLook = NewObject<UInputAction>(this);
	ActionLook->ValueType = EInputActionValueType::Axis2D;

	ActionJump = NewObject<UInputAction>(this);
	ActionInteract = NewObject<UInputAction>(this);
	ActionSave = NewObject<UInputAction>(this);
	ActionLoad = NewObject<UInputAction>(this);

	InputContext = NewObject<UInputMappingContext>(this);

	auto AddMapping = [this](UInputAction* Action, FKey Key)
	{
		FEnhancedActionKeyMapping& Mapping = InputContext->MapKey(Action, Key);
		Mapping.Context = InputContext;
	};

	AddMapping(ActionMove, EKeys::W);
	AddMapping(ActionMove, EKeys::A);
	AddMapping(ActionMove, EKeys::S);
	AddMapping(ActionMove, EKeys::D);
	AddMapping(ActionLook, EKeys::MouseX);
	AddMapping(ActionLook, EKeys::MouseY);
	AddMapping(ActionJump, EKeys::SpaceBar);
	AddMapping(ActionInteract, EKeys::F);
	AddMapping(ActionSave, EKeys::F5);
	AddMapping(ActionLoad, EKeys::F9);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContext, 0);
		}
	}
}

void ATriangleCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, Input.Y);
		AddMovementInput(Right, Input.X);
	}
}

void ATriangleCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Input.X);
		AddControllerPitchInput(Input.Y);
	}
}

void ATriangleCharacter::StartJump(const FInputActionValue& Value)
{
	Jump();
}

void ATriangleCharacter::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void ATriangleCharacter::PressInteract(const FInputActionValue& Value)
{
	if (FocusedInteractable)
	{
		FocusedInteractable->Interact(this);
	}
}

void ATriangleCharacter::QuickSave(const FInputActionValue& Value)
{
	if (UTriangleSaveSubsystem* Save = FTriangleGameplayStatics::GetSaveSubsystem(GetWorld()))
	{
		Save->SaveGameNow(nullptr);
		if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld()))
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("The dream remembers what you saw.")), 2.f);
		}
	}
}

void ATriangleCharacter::QuickLoad(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UTriangleSaveSubsystem* Save = FTriangleGameplayStatics::GetSaveSubsystem(World);
	UTriangleSaveGame* Snapshot = Save ? Save->LoadGameNow() : nullptr;
	if (!Snapshot)
	{
		if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World))
		{
			PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("There is nothing to wake into yet.")), 2.f);
		}
		return;
	}

	SetActorTransform(Snapshot->PlayerTransform);

	if (ULTriangleLucidityComponent* LucidityComp = FindComponentByClass<ULTriangleLucidityComponent>())
	{
		LucidityComp->RestoreFromSave(Snapshot->Lucidity);
	}

	if (ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(World))
	{
		PC->GetTriangleHUD()->PushSubtitle(FText::FromString(TEXT("You wake where you left the dream.")), 2.f);
	}
}

void ATriangleCharacter::ScanForInteractable()
{
	UWorld* World = GetWorld();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!World || !PC)
	{
		return;
	}

	UTriangleInteractableComponent* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	const FVector CameraLoc = FirstPersonCamera->GetComponentLocation();
	const FVector CameraForward = FirstPersonCamera->GetForwardVector();

	TArray<AActor*> OverlapActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoredActors{ this };

	const float SearchRadius = DefaultInteractionRange + 60.f;
	if (UKismetSystemLibrary::SphereOverlapActors(World, CameraLoc, SearchRadius, ObjectTypes, nullptr, IgnoredActors, OverlapActors))
	{
		for (AActor* Actor : OverlapActors)
		{
			UTriangleInteractableComponent* Comp = Actor ? Actor->FindComponentByClass<UTriangleInteractableComponent>() : nullptr;
			if (!Comp || !Comp->bEnabled)
			{
				continue;
			}

			const float CompRange = Comp->MaxInteractionRange > 0.f ? Comp->MaxInteractionRange : DefaultInteractionRange;

			// Favour targets inside the view cone.
			const FVector ToTarget = Comp->GetInteractionWorldLocation() - CameraLoc;
			const float Distance = ToTarget.Size();
			if (Distance > CompRange)
			{
				continue;
			}

			const float Forwardness = FVector::DotProduct(CameraForward, ToTarget.GetSafeNormal());
			if (Forwardness < 0.35f)
			{
				continue;
			}

			// Scoring = closeness weighted by how much we're facing it.
			const float ForwardBonus = FMath::Max(0.5f, Forwardness);
			const float Score = Distance * (2.f - ForwardBonus);
			if (Score < BestDistance)
			{
				BestDistance = Score;
				Best = Comp;
			}
		}
	}

	if (Best != FocusedInteractable)
	{
		if (FocusedInteractable)
		{
			FocusedInteractable->NotifyFocusEnd();
		}
		FocusedInteractable = Best;
		if (FocusedInteractable)
		{
			FocusedInteractable->NotifyFocusBegin();
		}
		UpdateInteractionPrompt();
	}
}

void ATriangleCharacter::SetFocusedInteractable(UTriangleInteractableComponent* NewFocus)
{
	if (NewFocus == FocusedInteractable)
	{
		return;
	}
	if (FocusedInteractable)
	{
		FocusedInteractable->NotifyFocusEnd();
	}
	FocusedInteractable = NewFocus;
	if (FocusedInteractable)
	{
		FocusedInteractable->NotifyFocusBegin();
	}
	UpdateInteractionPrompt();
}

void ATriangleCharacter::UpdateInteractionPrompt()
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!PC)
	{
		return;
	}

	if (FocusedInteractable)
	{
		PC->SetInteractionPrompt(FocusedInteractable->InteractionName, FText::FromString(TEXT("[F]")));
	}
	else
	{
		PC->SetInteractionPrompt(FText::GetEmpty(), FText::GetEmpty());
	}
}