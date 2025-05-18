// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(PlayerInputMappingContext_Ground, 0);
	}
}

void ABasePlayer::Move(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	UE_LOG(LogTemp, Warning, TEXT("we working"));
}

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
	}

}
void ABasePlayer::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PlayerCharacter = Cast<ABasePlayer>(aPawn);
	check(PlayerCharacter, Text("ABasePlayer derived classes should only posess ABasePlayer"));

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get referance to the EnhancedInputComponent"));

	if (MoveAction)
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::HandleMove);

	if (LookAction)
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayer::HandleLook);

	if (JumpAction)
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABasePlayer::HandleJump);
}

void ABasePlayer::OnUnPossess()
{

}
