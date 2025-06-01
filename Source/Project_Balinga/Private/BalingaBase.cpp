// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"

#include "Logging/LogMacros.h"

// Sets default values
ABalingaBase::ABalingaBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBalingaMovement>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	BalingaMovement = Cast<UBalingaMovement>(GetCharacterMovement());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Enable when attacking	
}

void ABalingaBase::BeginPlay()
{
	Super::BeginPlay();

	BalingaMovement->JumpZVelocity = JumpVelocity;
	BalingaMovement->GravityScale = BaseGravityScale;
	BalingaMovement->GetNavAgentPropertiesRef().bCanCrouch = true;
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
{Super::SetupPlayerInputComponent(PlayerInputComponent);}

// Jumping functions
void ABalingaBase::StartJump()	{Jump();	GetCharacterMovement()->GravityScale = JumpGravityScale;}
void ABalingaBase::EndJump()	{GetCharacterMovement()->GravityScale = BaseGravityScale;}

// Attack functions
void ABalingaBase::TryAttack()
{
	if (bCanAttack)
	{
		OnAttackOverlap(AttackSphere, nullptr, FHitResult());
	}
	DrawDebugSphere(
		GetWorld(),
		AttackSphere->GetComponentLocation(),   // Location
		100,                  // Radius
		12,                   // Segments
		FColor::Red,          // Color
		false,                // Persistent lines
		2.0f                  // Lifetime
	);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
} 
void ABalingaBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{


		UE_LOG(LogTemp, Warning, TEXT("Other Actor: "), OtherActor);
		//Apply damage or what ever
	}
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
