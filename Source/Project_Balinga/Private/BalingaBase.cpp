// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

//!
#include "BalingaCamera.h"

#include "DrawDebugHelpers.h"
#include "Logging/LogMacros.h"

// Sets default values
ABalingaBase::ABalingaBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBalingaMovement>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	BalingaMovement = Cast<UBalingaMovement>(GetCharacterMovement());
	BalingaCamera = Cast<UBalingaCamera>(GetCharacterMovement());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	
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

	BalingaMovement->JumpZVelocity = JumpVelocity;
	BalingaMovement->GravityScale = BaseGravityScale;
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
		GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Red, TEXT("Flying"));
		camMode = 1; // Set camMode to 1 for flying
	}
	else if (Pitch < -30.0f)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Green, TEXT("Diving"));
		camMode = 2; // Set camMode to 2 for diving
	}
	else if (Pitch < -5.0f && Pitch > -30.0f)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Green, TEXT("Gliding"));
		camMode = 2; // Set camMode to 2 for diving
	}
}

void ABalingaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{Super::SetupPlayerInputComponent(PlayerInputComponent);}

#pragma region GroundedMovement
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
void ABalingaBase::StartJump()	
{
	Jump();

	if (!BalingaMovement->IsCustomMovementMode(CMOVE_Glide))
	{
		GetCharacterMovement()->GravityScale = JumpGravityScale;
	}
}
void ABalingaBase::EndJump()	{GetCharacterMovement()->GravityScale = BaseGravityScale;}
#pragma endregion

void ABalingaBase::Land()
{
	BalingaMovement->LandPressed();
}

#pragma region Abilities
//enable AttackSphere and start CD
void ABalingaBase::TryAttack()
{
	if (bCanAttack)
	{
		AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		TArray<AActor*> OverlappingActors;
		AttackSphere->GetOverlappingActors(OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			OnAttackOverlap(nullptr, Actor, nullptr, 0, false, FHitResult());
		}

		AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bCanAttack = false;
		GetWorld()->GetTimerManager().SetTimer
		(
			AttackCooldownTimer,		//Timercooldown Timer
			this,						
			&ABalingaBase::AttackCD,
			AttackCooldown, 
			false
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, FString("on CD attack"));
	}
}
void ABalingaBase::AttackCD() 
{ 
	GEngine->AddOnScreenDebugMessage(2, 2, FColor::Cyan, FString("can Attack"));
	bCanAttack = true;
}
void ABalingaBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
void ABalingaBase::DropItem()
{
	if (!HeldItem) return;

	HeldItem->ItemDrop();
	HeldItem = nullptr;
}
#pragma endregion

