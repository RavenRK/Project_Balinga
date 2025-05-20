#include "BalingaControllerBase.h"
#include "BalingaBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

void ABalingaControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn); // idk what this does

	// Get a reference to the player's pawn
	PlayerCharacter = Cast<ABalingaBase>(aPawn);
	checkf(PlayerCharacter, TEXT("ABalingaController derived classes should only possess ABalinga derived pawns."));

	// Get a reference to the EnhancedInputComponent
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent."));

	// Get a reference to the local player subsystem
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem"));

	// Add InputMappingContext within InputSubsystem
	checkf(InputMappingContext, TEXT("InputMappingContent was not specified."));
	InputSubsystem->ClearAllMappings(); // Shouldn't do this if you have multiple mapping contexts active at the same time
	InputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Bind actions to bindings
	// (Sets up the EnhancedInputComponent to check for the bindings and execute the action when the check is successful)
	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ABalingaControllerBase::HandleMove);
	}

	if (ActionLook)
	{
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ABalingaControllerBase::HandleLook);
	}

	if (ActionJump)
	{
		EnhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &ABalingaControllerBase::HandleJump);
	}
}

void ABalingaControllerBase::OnUnPossess()
{
	// Unbinds actions, so the component won't check for them or execute those actions anymore
	EnhancedInputComponent->ClearActionBindings();

	Super::OnUnPossess(); // idk what this does 
}

void ABalingaControllerBase::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), MovementVector.X);
	}
}

void ABalingaControllerBase::HandleLook(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ABalingaControllerBase::HandleJump()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UnCrouch();
		PlayerCharacter->Jump();
	}
}

void ABalingaControllerBase::HandleToggleSprint()
{
}
