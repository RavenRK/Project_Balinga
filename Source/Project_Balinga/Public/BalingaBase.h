// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BalingaBase.generated.h"

UCLASS()
class ABalingaBase : public ACharacter
{
	typedef ACharacter Super;

public:
	// Sets default values for this character's properties
	ABalingaBase();
	ABalingaBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) class UBalingaMovement* BalingaMovement;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void CheckJumpInput(float DeltaTime) override;

	//to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float JumpHeldTime = 0.0f;
	float MaxJumpHoldTime = 1.5f;
	float JumpTimer();
	void BalingaJump();

private:
	GENERATED_BODY()
};
