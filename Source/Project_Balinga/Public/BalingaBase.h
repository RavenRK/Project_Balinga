// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"

#include "BalingaBase.generated.h"


UCLASS()
class ABalingaBase : public ACharacter
{
public:
	// Sets default values for this character's properties
	ABalingaBase();

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

	//Ground parameters
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump|GravityScale")
	float BaseGravityScale = 9.8f;
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump|GravityScale")
	float JumpGravityScale = 4.9f;
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump")
	float JumpVelocity = 1200;

	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|")
	float MoveSpeed = 600;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Move Balinga|other|Attack")
	USphereComponent* AttackSphere;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartJump();
	void EndJump();
	void Attack();
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, const FHitResult& SweepResult);
private:
	GENERATED_BODY()
};
