#include "Characters/ATriangleCharacter.h"

#include "Triangle.h"
#include "Characters/ATrianglePlayerController.h"
#include "Core/FTriangleGameplayStatics.h"
#include "Core/UTriangleGameFlowSubsystem.h"
#include "Dream/UTriangleFalseAwakeningManager.h"
#include "Interaction/UTriangleInteractableComponent.h"
#include "Lucidity/ULTriangleLucidityComponent.h"
#include "Lucidity/ULTriangleRecognitionComponent.h"
#include "Save/UTriangleSaveGame.h"
#include "Save/UTriangleSaveSubsystem.h"
#include "UI/ATriangleHUD.h"
#include "World/ATriangleRoomBuilder.h"
#include "World/ATriangleStationBuilder.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedActionKeyMapping.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/UObjectGlobals.h"

ATriangleCharacter::ATriangleCharacter()
{
	// Interaction focus is timer-driven; the character itself does not need a
	// per-frame tick just to validate a stored interaction reference.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	GetCharacterMovement()->MaxWalkSpeed = 320.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 110.f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, EyesHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;

	Lucidity = CreateDefaultSubobject<ULTriangleLucidityComponent>(TEXT("Lucidity"));
	Recognition = CreateDefaultSubobject<ULTriangleRecognitionComponent>(TEXT("Recognition"));
}

void ATriangleCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnsureInputBindings();
	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(GetController()); IsValid(PC))
	{
		PC->BindHUDToCurrentPawn();
	}

	StartInteractionScan();
}

void ATriangleCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Possession can happen before or after BeginPlay, and the local-player
	// subsystem can be created a frame later. EnsureInputBindings is safe to
	// call in both cases and only adds a context once per subsystem.
	EnsureInputBindings();
	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(NewController); IsValid(PC))
	{
		PC->BindHUDToCurrentPawn();
	}
	StartInteractionScan();

	// Guarantee the pawn lands inside the dream room. The game mode may pick a
	// map-native start when the room's DreamStart has not spawned yet, and that
	// scatters the player across a template landscape. Once possessed, snap to
	// the DreamStart, or fall back to the room centre facing the moonlit window.
	FTransform HomeTransform(FRotator(0.f, 180.f, 0.f), FVector(0.f, 0.f, 92.f + 5000.f));
	if (UWorld* World = GetWorld())
	{
		APlayerStart* ScopedStart = nullptr;
		if (ATriangleRoomBuilder* Room = UGameplayStatics::GetActorOfClass(World, ATriangleRoomBuilder::StaticClass()); IsValid(Room))
		{
			ScopedStart = Room->GetDreamStart();
		}
		if (!IsValid(ScopedStart))
		{
			if (ATriangleStationBuilder* Station = UGameplayStatics::GetActorOfClass(World, ATriangleStationBuilder::StaticClass()); IsValid(Station))
			{
				ScopedStart = Station->GetDreamStart();
			}
		}
		if (!IsValid(ScopedStart))
		{
			TArray<AActor*> DreamStarts;
			UGameplayStatics::GetAllActorsOfClassWithTag(World, APlayerStart::StaticClass(), TEXT("DreamStart"), DreamStarts);
			for (AActor* Candidate : DreamStarts)
			{
				if (APlayerStart* CandidateStart = Cast<APlayerStart>(Candidate); IsValid(CandidateStart))
				{
					ScopedStart = CandidateStart;
					break;
				}
			}
		}
		if (IsValid(ScopedStart))
		{
			HomeTransform = ScopedStart->GetActorTransform();
		}
	}
	SetActorLocationAndRotation(HomeTransform.GetLocation(), HomeTransform.GetRotation());
}

void ATriangleCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	EnsureInputBindings();
	StartInteractionScan();
}

void ATriangleCharacter::UnPossessed()
{
	RemoveInputContext();
	BoundInputComponent = nullptr;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScanTimerHandle);
	}

	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(GetController()); IsValid(PC))
	{
		if (ATriangleHUD* HUD = PC->GetTriangleHUD(); HUD && HUD->IsBoundToPlayer(this))
		{
			HUD->UnbindFromPlayer();
		}
	}
	if (IsValid(FocusedInteractable.Get()))
	{
		SetFocusedInteractable(nullptr);
	}
	else
	{
		FocusedInteractable = nullptr;
		UpdateInteractionPrompt();
	}

	Super::UnPossessed();
}

void ATriangleCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveInputContext();
	BoundInputComponent = nullptr;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScanTimerHandle);
	}

	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(GetController()); IsValid(PC) && PC->GetPawn() == this)
	{
		if (ATriangleHUD* HUD = PC->GetTriangleHUD(); HUD && HUD->IsBoundToPlayer(this))
		{
			HUD->UnbindFromPlayer();
		}
	}

	if (IsValid(FocusedInteractable.Get()) && IsValid(FocusedInteractable->GetOwner()))
	{
		FocusedInteractable->NotifyFocusEnd();
	}
	FocusedInteractable = nullptr;

	Super::EndPlay(EndPlayReason);
}

void ATriangleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!IsValid(PlayerInputComponent))
	{
		return;
	}

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Pause is handled by an Enhanced Input action whose TriggerWhenPaused flag
	// is set below; the legacy input-component pause flag is not used in UE 5.8.
	// Setup can run again after a seamless travel or a controller change.
	// Recreate assets if needed, but never bind the same component twice.
	EnsureInputBindings();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput || (BoundInputComponent.Get() == PlayerInputComponent && IsValid(BoundInputComponent.Get())))
	{
		return;
	}

	if (!ActionMoveForward || !ActionMoveRight || !ActionLookHorizontal || !ActionLookVertical || !ActionJump || !ActionInteract || !ActionWake || !ActionSave || !ActionLoad || !ActionPause)
	{
		BoundInputComponent = nullptr;
		return;
	}

	BoundInputComponent = PlayerInputComponent;

	EnhancedInput->BindAction(ActionMoveForward, ETriggerEvent::Triggered, this, &ATriangleCharacter::MoveForward);
	EnhancedInput->BindAction(ActionMoveRight, ETriggerEvent::Triggered, this, &ATriangleCharacter::MoveRight);
	EnhancedInput->BindAction(ActionLookHorizontal, ETriggerEvent::Triggered, this, &ATriangleCharacter::LookHorizontal);
	EnhancedInput->BindAction(ActionLookVertical, ETriggerEvent::Triggered, this, &ATriangleCharacter::LookVertical);
	EnhancedInput->BindAction(ActionJump, ETriggerEvent::Started, this, &ATriangleCharacter::StartJump);
	EnhancedInput->BindAction(ActionJump, ETriggerEvent::Completed, this, &ATriangleCharacter::StopJump);
	EnhancedInput->BindAction(ActionInteract, ETriggerEvent::Started, this, &ATriangleCharacter::PressInteract);
	EnhancedInput->BindAction(ActionWake, ETriggerEvent::Started, this, &ATriangleCharacter::PressWake);
	EnhancedInput->BindAction(ActionSave, ETriggerEvent::Started, this, &ATriangleCharacter::QuickSave);
	EnhancedInput->BindAction(ActionLoad, ETriggerEvent::Started, this, &ATriangleCharacter::QuickLoad);
	EnhancedInput->BindAction(ActionPause, ETriggerEvent::Started, this, &ATriangleCharacter::PressPause);
}

void ATriangleCharacter::EnsureInputBindings()
{
	// Actions are owned by this pawn, so they remain valid for the lifetime of
	// the character and are not saved with a map.
	if (!ActionMoveForward)
	{
		ActionMoveForward = NewObject<UInputAction>(this);
		ActionMoveForward->ValueType = EInputActionValueType::Axis1D;
	}
	if (!ActionMoveRight)
	{
		ActionMoveRight = NewObject<UInputAction>(this);
		ActionMoveRight->ValueType = EInputActionValueType::Axis1D;
	}
	if (!ActionLookHorizontal)
	{
		ActionLookHorizontal = NewObject<UInputAction>(this);
		ActionLookHorizontal->ValueType = EInputActionValueType::Axis1D;
	}
	if (!ActionLookVertical)
	{
		ActionLookVertical = NewObject<UInputAction>(this);
		ActionLookVertical->ValueType = EInputActionValueType::Axis1D;
	}
	if (!ActionJump)
	{
		ActionJump = NewObject<UInputAction>(this);
		ActionJump->ValueType = EInputActionValueType::Boolean;
	}
	if (!ActionInteract)
	{
		ActionInteract = NewObject<UInputAction>(this);
		ActionInteract->ValueType = EInputActionValueType::Boolean;
	}
	if (!ActionWake)
	{
		ActionWake = NewObject<UInputAction>(this);
		ActionWake->ValueType = EInputActionValueType::Boolean;
	}
	if (!ActionSave)
	{
		ActionSave = NewObject<UInputAction>(this);
		ActionSave->ValueType = EInputActionValueType::Boolean;
	}
	if (!ActionLoad)
	{
		ActionLoad = NewObject<UInputAction>(this);
		ActionLoad->ValueType = EInputActionValueType::Boolean;
	}
	if (!ActionPause)
	{
		ActionPause = NewObject<UInputAction>(this);
		ActionPause->ValueType = EInputActionValueType::Boolean;
	}
	if (IsValid(ActionPause.Get()))
	{
		ActionPause->bTriggerWhenPaused = true;
	}

	if (!InputContext)
	{
		InputContext = NewObject<UInputMappingContext>(this);
	}

	if (InputContext && !bInputMappingsInitialized)
	{
		auto AddMapping = [this](UInputAction* Action, const FKey Key, const bool bNegate = false, const float Scale = 1.f)
		{
			if (InputContext && Action)
			{
				FEnhancedActionKeyMapping& Mapping = InputContext->MapKey(Action, Key);
				if (bNegate)
				{
					Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(InputContext));
				}
				if (!FMath::IsNearlyEqual(Scale, 1.f))
				{
					UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(InputContext);
					Scalar->Scalar = FVector(Scale, Scale, Scale);
					Mapping.Modifiers.Add(Scalar);
				}
			}
		};

		// Keyboard movement/look use separate Axis1D actions. Digital keys are
		// explicitly negated where the player expects the opposite direction;
		// no implicit Axis2D component merging is involved.
		AddMapping(ActionMoveForward, EKeys::W);
		AddMapping(ActionMoveForward, EKeys::S, true);
		AddMapping(ActionMoveRight, EKeys::D);
		AddMapping(ActionMoveRight, EKeys::A, true);
		AddMapping(ActionLookHorizontal, EKeys::MouseX, false, 0.07f);
		// UE's standard mouse convention reports downward motion as positive
		// Y; negate it so moving the mouse down looks down.
		AddMapping(ActionLookVertical, EKeys::MouseY, true, 0.07f);
		AddMapping(ActionJump, EKeys::SpaceBar);
		AddMapping(ActionInteract, EKeys::F);
		AddMapping(ActionWake, EKeys::Q);
		AddMapping(ActionSave, EKeys::F5);
		AddMapping(ActionLoad, EKeys::F9);
		AddMapping(ActionPause, EKeys::Escape);

		// Equivalent gamepad bindings. The left/right sticks are already signed
		// axes, so only keyboard digital directions need a negate modifier.
		AddMapping(ActionMoveRight, EKeys::Gamepad_LeftX);
		AddMapping(ActionMoveForward, EKeys::Gamepad_LeftY);
		AddMapping(ActionLookHorizontal, EKeys::Gamepad_RightX);
		AddMapping(ActionLookVertical, EKeys::Gamepad_RightY);
		AddMapping(ActionJump, EKeys::Gamepad_FaceButton_Bottom);
		AddMapping(ActionInteract, EKeys::Gamepad_FaceButton_Left);
		AddMapping(ActionWake, EKeys::Gamepad_FaceButton_Right);
		AddMapping(ActionSave, EKeys::Gamepad_LeftShoulder);
		AddMapping(ActionLoad, EKeys::Gamepad_RightShoulder);
		AddMapping(ActionPause, EKeys::Gamepad_Start);
		bInputMappingsInitialized = true;
	}

	const bool bAdded = TryAddInputContext();
	if (!bAdded)
	{
		// LocalPlayer subsystems are occasionally initialized after the pawn.
		// Retry on a timer rather than assuming BeginPlay order.
		APlayerController* PC = Cast<APlayerController>(GetController());
		const bool bHasLocalPlayer = IsValid(PC) && (PC->IsLocalController() || PC->GetLocalPlayer() != nullptr);
		if (UWorld* World = GetWorld(); World && World->GetNetMode() != NM_DedicatedServer && bHasLocalPlayer)
		{
			World->GetTimerManager().SetTimer(
				InputContextRetryTimerHandle,
				this,
				&ATriangleCharacter::RetryAddInputContext,
				0.25f,
				true,
				0.25f);
		}
	}
}

bool ATriangleCharacter::TryAddInputContext()
{
	if (!InputContext)
	{
		return false;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!IsValid(PC) || !PC->IsLocalController())
	{
		return false;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return false;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!IsValid(Subsystem))
	{
		return false;
	}

	if (ActiveInputSubsystem.Get() != Subsystem)
	{
		RemoveInputContext();
		ActiveInputSubsystem = Subsystem;
	}

	// HasMappingContext is checked even after our local flag is set. This
	// handles a local-player subsystem being rebuilt or another system adding
	// the same context between possession calls.
	if (!Subsystem->HasMappingContext(InputContext))
	{
		Subsystem->AddMappingContext(InputContext, 0);
		bInputContextAddedByCharacter = true;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InputContextRetryTimerHandle);
	}
	return true;
}

void ATriangleCharacter::RetryAddInputContext()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	const bool bHasLocalPlayer = IsValid(PC) && (PC->IsLocalController() || PC->GetLocalPlayer() != nullptr);
	if (!TryAddInputContext() && !bHasLocalPlayer)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(InputContextRetryTimerHandle);
		}
	}
}

void ATriangleCharacter::RemoveInputContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ActiveInputSubsystem.Get())
	{
		if (bInputContextAddedByCharacter && InputContext && Subsystem->HasMappingContext(InputContext))
		{
			Subsystem->RemoveMappingContext(InputContext);
		}
	}

	ActiveInputSubsystem.Reset();
	bInputContextAddedByCharacter = false;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InputContextRetryTimerHandle);
	}
}

void ATriangleCharacter::MoveForward(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}
	const float Input = Value.Get<float>();
	if (FMath::IsNearlyZero(Input) || !FMath::IsFinite(Input) || !IsValid(Controller))
	{
		return;
	}

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Input);
}

void ATriangleCharacter::MoveRight(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}
	const float Input = Value.Get<float>();
	if (FMath::IsNearlyZero(Input) || !FMath::IsFinite(Input) || !IsValid(Controller))
	{
		return;
	}

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Input);
}

void ATriangleCharacter::LookHorizontal(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}
	const float Input = Value.Get<float>();
	if (FMath::IsNearlyZero(Input) || !FMath::IsFinite(Input) || !IsValid(Controller))
	{
		return;
	}
	AddControllerYawInput(Input);
}

void ATriangleCharacter::LookVertical(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}
	const float Input = Value.Get<float>();
	if (FMath::IsNearlyZero(Input) || !FMath::IsFinite(Input) || !IsValid(Controller))
	{
		return;
	}
	AddControllerPitchInput(Input);
}

void ATriangleCharacter::StartJump(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}
	Jump();
}

void ATriangleCharacter::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void ATriangleCharacter::PressInteract(const FInputActionValue& Value)
{
	if (const UTriangleGameFlowSubsystem* Flow = GetWorld() ? GetWorld()->GetSubsystem<UTriangleGameFlowSubsystem>() : nullptr; Flow && Flow->IsTransitioning())
	{
		return;
	}

	UTriangleInteractableComponent* Interactable = FocusedInteractable.Get();
	if (IsValid(Interactable) && Interactable->bEnabled && IsValid(Interactable->GetOwner()))
	{
		Interactable->Interact(this);
	}
}

void ATriangleCharacter::PressWake(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>(); Flow && Flow->IsTransitioning())
	{
		return;
	}
	if (UTriangleFalseAwakeningManager* Wake = World->GetSubsystem<UTriangleFalseAwakeningManager>())
	{
		Wake->TryWake();
	}
}

void ATriangleCharacter::PressPause(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	UWorld* World = IsValid(PC) ? PC->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}
	if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>(); Flow && Flow->IsTransitioning())
	{
		return;
	}

	const bool bPaused = !World->IsPaused();
	World->SetPause(bPaused);
	if (ATrianglePlayerController* TrianglePC = Cast<ATrianglePlayerController>(PC); IsValid(TrianglePC))
	{
		TrianglePC->SetPauseVisible(bPaused);
		TrianglePC->SetShowMouseCursor(bPaused);
	}
}

void ATriangleCharacter::PushHudSubtitle(const FText& Text, float Duration)
{
	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(GetController()); IsValid(PC))
	{
		PC->PushSubtitle(Text, Duration);
	}
}

void ATriangleCharacter::QuickSave(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>(); Flow && Flow->IsTransitioning())
	{
		PushHudSubtitle(FText::FromString(TEXT("The dream is still changing.")), 1.5f);
		return;
	}

	UTriangleSaveSubsystem* Save = FTriangleGameplayStatics::GetSaveSubsystem(World);
	if (!Save)
	{
		return;
	}

	const bool bSaved = Save->SaveGameNow(nullptr);
	PushHudSubtitle(
		bSaved
			? FText::FromString(TEXT("The dream remembers what you saw."))
			: FText::FromString(TEXT("The dream cannot hold that memory yet.")),
		2.f);
}

void ATriangleCharacter::QuickLoad(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	if (const UTriangleGameFlowSubsystem* Flow = World->GetSubsystem<UTriangleGameFlowSubsystem>(); Flow && Flow->IsTransitioning())
	{
		PushHudSubtitle(FText::FromString(TEXT("Wait for the dream to settle.")), 1.5f);
		return;
	}

	UTriangleSaveSubsystem* Save = FTriangleGameplayStatics::GetSaveSubsystem(World);
	UTriangleSaveGame* Snapshot = Save ? Save->LoadGameNow() : nullptr;
	if (!Snapshot)
	{
		PushHudSubtitle(FText::FromString(TEXT("There is nothing to wake into yet.")), 2.f);
		return;
	}

	// The save subsystem owns environment restoration as well as meters. A
	// different environment is queued until the procedural swap is complete.
	const ETriangleSaveApplyResult ApplyResult = Save->ApplySnapshot(Snapshot);
	if (ApplyResult == ETriangleSaveApplyResult::Queued)
	{
		PushHudSubtitle(FText::FromString(TEXT("The memory folds the room around you...")), 2.f);
		return;
	}
	if (ApplyResult != ETriangleSaveApplyResult::Applied)
	{
		PushHudSubtitle(FText::FromString(TEXT("The dream refuses that memory.")), 2.f);
		return;
	}

	if (ATrianglePlayerController* PC = Cast<ATrianglePlayerController>(GetController()); IsValid(PC))
	{
		PC->BindHUDToCurrentPawn();
	}
	PushHudSubtitle(FText::FromString(TEXT("You wake where you left the dream.")), 2.f);
}

void ATriangleCharacter::StartInteractionScan()
{
	UWorld* World = GetWorld();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!World || !IsValid(PC) || !PC->IsLocalController())
	{
		if (World)
		{
			World->GetTimerManager().ClearTimer(ScanTimerHandle);
		}
		return;
	}

	const float SafeConfiguredInterval = FMath::IsFinite(InteractionScanInterval) ? InteractionScanInterval : 0.08f;
	const float ScanInterval = FMath::Clamp(SafeConfiguredInterval, 0.05f, 1.f);
	World->GetTimerManager().ClearTimer(ScanTimerHandle);
	World->GetTimerManager().SetTimer(
		ScanTimerHandle,
		this,
		&ATriangleCharacter::ScanForInteractable,
		ScanInterval,
		true,
		ScanInterval);
}

void ATriangleCharacter::ScanForInteractable()
{
	// The timer is the validity boundary for focus.  This replaces the old
	// per-frame Tick check and also clears a disabled component before input
	// can reach it.
	if (!IsValid(FocusedInteractable.Get()))
	{
		// A component can be destroyed while its owner is being torn down.
		// Clear the prompt as well as the reference so a dead actor cannot
		// leave a stale interaction hint behind, without touching the HUD on
		// every empty scan.
		const bool bHadFocusReference = FocusedInteractable.Get() != nullptr;
		FocusedInteractable = nullptr;
		if (bHadFocusReference)
		{
			UpdateInteractionPrompt();
		}
	}
	else if (!FocusedInteractable->bEnabled || !IsValid(FocusedInteractable->GetOwner()))
	{
		SetFocusedInteractable(nullptr);
	}

	UWorld* World = GetWorld();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!World || !IsValid(PC) || !PC->IsLocalController() || !IsValid(FirstPersonCamera.Get()))
	{
		if (IsValid(FocusedInteractable.Get()))
		{
			SetFocusedInteractable(nullptr);
		}
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
	const float SafeDefaultRange = FMath::IsFinite(DefaultInteractionRange) ? FMath::Max(40.f, DefaultInteractionRange) : 240.f;
	// Keep the scan broad enough for authored long-range props (the train
	// intentionally has a 520 cm interaction range) while the per-component
	// distance test below remains the real gameplay gate.
	const float SearchRadius = FMath::Max(SafeDefaultRange + 60.f, 700.f);
	if (UKismetSystemLibrary::SphereOverlapActors(World, CameraLoc, SearchRadius, ObjectTypes, nullptr, IgnoredActors, OverlapActors))
	{
		for (AActor* Actor : OverlapActors)
		{
			UTriangleInteractableComponent* Comp = IsValid(Actor) ? Actor->FindComponentByClass<UTriangleInteractableComponent>() : nullptr;
			if (!IsValid(Comp) || !Comp->bEnabled || !IsValid(Comp->GetOwner()))
			{
				continue;
			}

			const float CompRange = FMath::IsFinite(Comp->MaxInteractionRange) && Comp->MaxInteractionRange > 0.f
				? Comp->MaxInteractionRange
				: SafeDefaultRange;
			const FVector ToTarget = Comp->GetInteractionWorldLocation() - CameraLoc;
			const float Distance = ToTarget.Size();
			if (Distance > CompRange)
			{
				continue;
			}

			// Favour targets inside the view cone.
			const float Forwardness = FVector::DotProduct(CameraForward, ToTarget.GetSafeNormal());
			if (Forwardness < 0.35f)
			{
				continue;
			}

			// Do not focus through a wall. Stop just before the target so its
			// own collision does not count as an occluder.
			FHitResult Hit;
			FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TriangleInteractionFocus), false);
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(Actor);
			const FVector TraceEnd = CameraLoc + ToTarget - ToTarget.GetSafeNormal() * 8.f;
			if (World->LineTraceSingleByChannel(Hit, CameraLoc, TraceEnd, ECC_Visibility, QueryParams)
				&& Hit.bBlockingHit && Hit.Distance < Distance - 16.f)
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

	if (Best != FocusedInteractable.Get())
	{
		SetFocusedInteractable(Best);
	}
}

void ATriangleCharacter::SetFocusedInteractable(UTriangleInteractableComponent* NewFocus)
{
	UTriangleInteractableComponent* ValidFocus = NewFocus;
	if (ValidFocus && (!IsValid(ValidFocus) || !ValidFocus->bEnabled || !IsValid(ValidFocus->GetOwner())))
	{
		ValidFocus = nullptr;
	}

	UTriangleInteractableComponent* CurrentFocus = FocusedInteractable.Get();
	if (ValidFocus == CurrentFocus)
	{
		return;
	}

	if (IsValid(CurrentFocus) && IsValid(CurrentFocus->GetOwner()))
	{
		CurrentFocus->NotifyFocusEnd();
	}

	FocusedInteractable = ValidFocus;
	if (IsValid(FocusedInteractable.Get()))
	{
		FocusedInteractable->NotifyFocusBegin();
	}
	UpdateInteractionPrompt();
}

void ATriangleCharacter::UpdateInteractionPrompt()
{
	ATrianglePlayerController* PC = FTriangleGameplayStatics::GetTrianglePlayerController(GetWorld());
	if (!IsValid(PC))
	{
		return;
	}

	UTriangleInteractableComponent* CurrentFocus = FocusedInteractable.Get();
	if (IsValid(CurrentFocus) && CurrentFocus->bEnabled && IsValid(CurrentFocus->GetOwner()))
	{
		PC->SetInteractionPrompt(CurrentFocus->InteractionName, FText::FromString(TEXT("[F / X]")));
	}
	else
	{
		PC->SetInteractionPrompt(FText::GetEmpty(), FText::GetEmpty());
	}
}
