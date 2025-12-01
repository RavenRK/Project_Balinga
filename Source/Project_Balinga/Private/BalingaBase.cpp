// Fill out your copyright notice in the Description page of Project Settings.

#include "BalingaBase.h"

#include "BalingaMovement.h"		//Custom Movement

#include "BalingaCamera.h"			//Camera
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "BaseItem.h"				//Items
#include "BalingaStatemachine.h"	//State Machine

#include "DrawDebugHelpers.h"		//Debug
#include "Logging/LogMacros.h"		
#include "Components/SphereComponent.h"

// Sets default values
ABalingaBase::ABalingaBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBalingaMovement>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	//State Machine
	StateMachine = CreateDefaultSubobject<UBalingaStateMachine>(TEXT("State Machine"));
	StateMachine->InitStateMachine();

	//Custom Movement Component
	BalingaMovement = Cast<UBalingaMovement>(GetCharacterMovement());
	BalingaCamera = Cast<UBalingaCamera>(GetCharacterMovement());

	//Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	
	//Attck Sphere
	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackSphere->SetGenerateOverlapEvents(true);

	LeftLiftArrow = CreateDefaultSubobject<USceneComponent>(TEXT("LeftLiftArrow"));
	LeftLiftArrow->SetupAttachment(RootComponent);
	RightLiftArrow = CreateDefaultSubobject<USceneComponent>(TEXT("RightLiftArrow"));
	RightLiftArrow->SetupAttachment(RootComponent);
	DragArrow = CreateDefaultSubobject<USceneComponent>(TEXT("DragArrow"));
	DragArrow->SetupAttachment(RootComponent);

}



void ABalingaBase::BeginPlay()
{
	Super::BeginPlay();

	//BalingaMovement->JumpZVelocity = JumpVelocity;
	//BalingaMovement->GravityScale = BaseGravityScale;
	BalingaMovement->GetNavAgentPropertiesRef().bCanCrouch = true;

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ABalingaBase::OnAttackOverlap);
}

void ABalingaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float Speed = GetVelocity().Size();
	BalingaCamera->CameraControllerCheck(Camera, SpringArm, DeltaTime, camMode, Speed);

	FRotator BalingaRotation = GetActorRotation();
	float Pitch = BalingaRotation.Pitch;
	if (Pitch > 5)
	{
		// GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Red, TEXT("Flying"));
		camMode = 1; // Set camMode to 1 for flying
	}
	else if (Pitch < -30.0f)
	{
		// GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Green, TEXT("Diving"));
		camMode = 2; // Set camMode to 2 for diving
	}
	else if (Pitch < -5.0f && Pitch > -30.0f)
	{
		// GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Green, TEXT("Gliding"));
		camMode = 2; // Set camMode to 2 for diving
	}
}

void ABalingaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{Super::SetupPlayerInputComponent(PlayerInputComponent);}

void ABalingaBase::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (BalingaMovement)
	{
		if (bPressedJump)
		{
			// Should set up check for max coyote time and distance travelled since in air to decide coyote jump or flap
			const bool bFirstJump = JumpCurrentCount == 0;
			if (BalingaMovement->IsFalling() || BalingaMovement->IsCustomMovementMode(CMOVE_Glide))
			{
				if (!BalingaMovement->IsCustomMovementMode(CMOVE_Glide))
				{
					BalingaMovement->EnterGlide();
				}

				// Should check if we can eventually
				BalingaMovement->FlapPressed();
				return;
				
				//JumpCurrentCount++;
			}
			else
			{
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
}


void ABalingaBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && HeldItem == nullptr)
	{
		if (OtherActor->IsA(ABaseItem::StaticClass()))
		{
			ABaseItem* Item = Cast<ABaseItem>(OtherActor);
			if (Item)
			{
				PickUpItem(Item);
			}
		}
	}
}
void ABalingaBase::PickUpItem(ABaseItem* Item)
{
	if (!Item) return;

	Item->ItemPickUp(AttackSphere);
	HeldItem = Item;
}