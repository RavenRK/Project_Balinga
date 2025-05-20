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

// Called when the game starts or when spawned
void ABalingaBase::BeginPlay()
{
	Super::BeginPlay();

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called every frame
void ABalingaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABalingaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

