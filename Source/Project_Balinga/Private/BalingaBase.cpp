// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"



// Sets default values
ABalingaBase::ABalingaBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Enable when attacking

	//bUseControllerRotationYaw = false;
	//GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABalingaBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->GravityScale = BaseGravityScale;

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

void ABalingaBase::StartJump()	{Jump();	GetCharacterMovement()->GravityScale = JumpGravityScale;}
void ABalingaBase::EndJump()	{GetCharacterMovement()->GravityScale = BaseGravityScale;}

// enable collision when attacking
void ABalingaBase::Attack()
{
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
// what to do after attack adn disable collision
void ABalingaBase::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//Apply damage or what ever
	}
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
