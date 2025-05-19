// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABaseCharacter.h"

void ABasePlayer::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PlayerCharacter = Cast<AABaseCharacter>(aPawn);
	checkf(PlayerCharacter, TEXT("ABasePlayer derived classes should only posess ABasePlayer"));

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get referance to the EnhancedInputComponent"));

	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(PlayerInputMappingContext_Ground, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem"));

	checkf(PlayerInputMappingContext_Ground, TEXT("InputMappingContent was not specified."));
	InputSubsystem->AddMappingContext(PlayerInputMappingContext_Ground, 0);

	if (MoveAction && LookAction && JumpAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayer::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABasePlayer::Jump);
	}
	else {checkf(false, TEXT("One or more input actions were not specified."))}
}

void ABasePlayer::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionVector = Value.Get<FVector2D>();

	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), DirectionVector.Y);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), DirectionVector.X);

}

void ABasePlayer::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);
}

void ABasePlayer::Jump(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UnCrouch();
		PlayerCharacter->Jump();
	}
}
void ABasePlayer::OnUnPossess() { EnhancedInputComponent->ClearActionBindings(); Super::OnUnPossess(); };

