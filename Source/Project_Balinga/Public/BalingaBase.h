// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	float JumpHeldTime = 0.0f;
	float MaxJumpHoldTime = 1.5f;
	float JumpTimer();
	void BalingaJump();
private:
	GENERATED_BODY()
};
