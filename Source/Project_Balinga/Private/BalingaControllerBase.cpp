#include "BalingaControllerBase.h"
#include "BalingaBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "BalingaMovement.h"
#include "UI/BalingaHudBase.h"
#include "BaseItem.h"				//Items

void ABalingaControllerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	Balinga = Cast<ABalingaBase>(aPawn);
	checkf(Balinga, TEXT("ABalingaController derived classes should only possess ABalinga derived pawns."));

	EnhInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent."));
	
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem"));

	checkf(BaseInputMapping, TEXT("InputMappingContent was not specified."));

	InputSubsystem->ClearAllMappings(); // Shouldn't do this if you have multiple mapping contexts active at the same time
	InputSubsystem->AddMappingContext(BaseInputMapping, 0);

	// Bind actions to bindings
	if (MoveAction && LookAction && JumpAction && LandAction)
	{
		// Ground
		EnhInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABalingaControllerBase::Move);
		EnhInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABalingaControllerBase::StartJump);
		EnhInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABalingaControllerBase::EndJump);

		// In Air / Flight
		EnhInputComponent->BindAction(LandAction, ETriggerEvent::Triggered, this, &ABalingaControllerBase::Land);

		// Abilities 
		EnhInputComponent->BindAction(LookAction,   ETriggerEvent::Triggered, this, &ABalingaControllerBase::Look);
		EnhInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABalingaControllerBase::Attack);
		EnhInputComponent->BindAction(DropAction,   ETriggerEvent::Started, this, &ABalingaControllerBase::DropItem);

	}
	else   {checkf(false, TEXT("One or more input actions were not specified."));}

	//Qjg0LTg3MbShowMouseCursor = true;
}

#pragma region Movement
void ABalingaControllerBase::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}
void ABalingaControllerBase::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (Balinga)
	{
		Balinga->AddMovementInput(Balinga->GetActorForwardVector(), MovementVector.Y);
		Balinga->AddMovementInput(Balinga->GetActorRightVector(), MovementVector.X);
	}
}
void ABalingaControllerBase::StartJump(const FInputActionValue& InputActionValue)
{
	Balinga->Jump();

	if (!BalingaMovement->IsCustomMovementMode(CMOVE_Glide))
	{
		Balinga->GetCharacterMovement()->GravityScale = JumpGravityScale;
	}
}
void ABalingaControllerBase::EndJump(const FInputActionValue& InputActionValue)
{
	Balinga->GetCharacterMovement()->GravityScale = BaseGravityScale;
}

void ABalingaControllerBase::Land(const FInputActionValue& InputActionValue) 
{
	BalingaMovement->LandPressed();
}
#pragma endregion // look, move, jump, land

#pragma region Abilities
void ABalingaControllerBase::Attack(const FInputActionValue& InputActionValue) 
{ 
	TryAttack(); 
}
void ABalingaControllerBase::DropItem(const FInputActionValue & InputActionValue)
{
	if (!Balinga->HeldItem) return;

	ABaseItem* HeldItem = Balinga->HeldItem;

	HeldItem->ItemDrop();
	Balinga->HeldItem = nullptr;
}
void ABalingaControllerBase::TryAttack()
{
	if (bCanAttack)
	{
		Balinga->AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		TArray<AActor*> OverlappingActors;
		Balinga->AttackSphere->GetOverlappingActors(OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			Balinga->OnAttackOverlap(nullptr, Actor, nullptr, 0, false, FHitResult());
		}

		Balinga->AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bCanAttack = false;
		GetWorld()->GetTimerManager().SetTimer
		(
			AttackCooldownTimer,		//Timercooldown Timer
			this,
			&ABalingaControllerBase::AttackCD,
			AttackCooldown,
			false
		);
	}
	else
	{GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, FString("on CD attack"));}
}
void ABalingaControllerBase::AttackCD()
{
	GEngine->AddOnScreenDebugMessage(2, 2, FColor::Cyan, FString("can Attack"));
	bCanAttack = true;
}

#pragma endregion //attack and stuff

FVector2D ABalingaControllerBase::GetAimerPosition()
{
	if (MyHUD)
	{
		return Cast<ABalingaHudBase>(MyHUD)->GetAimerPosition();
	}

	return FVector2D::ZeroVector;
}

void ABalingaControllerBase::OnUnPossess()
{
	// Unbinds actions after unpossessing
	EnhInputComponent->ClearActionBindings();
	Super::OnUnPossess();
}