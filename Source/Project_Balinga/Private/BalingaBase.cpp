// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABalingaBase::ABalingaBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ABalingaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABalingaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABalingaBase::JumpTimer()
{
	JumpHeldTime = 0.0f;
	//start a timer on input down (trying not to use tick )
	return JumpHeldTime;
}

void ABalingaBase::BalingaJump()
{
	if (JumpHeldTime < 0.15f) 
	{
		//balinga jump 
	}
	else
	{Jump();} //default jump
}

