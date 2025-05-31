// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
ABalingaBase::ABalingaBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBalingaMovement>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BalingaMovement = Cast<UBalingaMovement>(GetCharacterMovement());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	//bUseControllerRotationYaw = false;
	//GetCharacterMovement()->bOrientRotationToMovement = false;
	
}

void ABalingaBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABalingaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABalingaBase::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (BalingaMovement)
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			// Set up check for max coyote time and distance travelled since in air to decide coyote jump or flap
			const bool bFirstJump = JumpCurrentCount == 0;
			if (BalingaMovement->IsFalling() || BalingaMovement->IsCustomMovementMode(CMOVE_Fly))
			{
				if (!BalingaMovement->IsCustomMovementMode(CMOVE_Fly))
				{
					BalingaMovement->EnterFly();
				}

				// Should check if we can eventually
				BalingaMovement->DoFlap();
				UE_LOG(LogTemp, Warning, TEXT("FLAPPED"))
				return;
				
				//JumpCurrentCount++;
			}
			
			const bool bDidJump = CanJump() && BalingaMovement->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}

void ABalingaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABalingaBase::JumpTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("print text timer "));
	JumpHeldTime = 0.0f;
	//start a timer on input down (trying not to use tick )
	return JumpHeldTime;
}

void ABalingaBase::BalingaJump()
{
	UE_LOG(LogTemp, Warning, TEXT("JUMPED"));
	Jump(); //default jump
}

