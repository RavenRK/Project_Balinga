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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	GENERATED_BODY()
};
