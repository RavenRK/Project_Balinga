#include "BalingaControllerBase.h"
#include "BalingaBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"


void ABalingaControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	#pragma region GetReferences
		//ref to player's pawn
		PlayerCharacter = Cast<ABalingaBase>(aPawn);
		checkf(PlayerCharacter, TEXT("ABalingaController derived classes should only possess ABalinga derived pawns."));

		//ref EnhInputComponent
		EnhInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		checkf(EnhInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent."));

		//ref local player subsystem
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem"));

		// Add InputMappingContext within InputSubsystem
		checkf(GroundInputMapping, TEXT("InputMappingContent was not specified."));
	#pragma endregion

	InputSubsystem->ClearAllMappings(); // Shouldn't do this if you have multiple mapping contexts active at the same time
	InputSubsystem->AddMappingContext(GroundInputMapping, 0);

	// Bind actions to bindings
	if (ActionMove && ActionLook && ActionJump)
	{
		//ground inputActions
		EnhInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ABalingaControllerBase::Move);
		EnhInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ABalingaControllerBase::Look);

		//note might use ongoing its called every frame while the action is pressed
		EnhInputComponent->BindAction(ActionJump, ETriggerEvent::Started, this, &ABalingaControllerBase::JumpTimer);
		EnhInputComponent->BindAction(ActionJump, ETriggerEvent::Completed, this, &ABalingaControllerBase::BalingaJump);
	}
	else   {checkf(false, TEXT("One or more input actions were not specified."));}
}
void ABalingaControllerBase::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), MovementVector.X);
	}
}
void ABalingaControllerBase::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

	#pragma region jumpFunc

void ABalingaControllerBase::JumpTimer()
{
	if (PlayerCharacter) PlayerCharacter->JumpTimer();
}

void ABalingaControllerBase::BalingaJump()
{
	if (PlayerCharacter)PlayerCharacter->BalingaJump();
}

#pragma endregion


//void ABalingaControllerBase::HandleToggleSprint(){}
void ABalingaControllerBase::OnUnPossess()
{
	// Unbinds actions after unpossessing
	EnhInputComponent->ClearActionBindings();
	Super::OnUnPossess();
}