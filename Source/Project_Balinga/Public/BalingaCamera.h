// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACharacter;

UCLASS()
class PROJECT_BALINGA_API UBalingaCamera : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	void RL_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, float Speed);
	void Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void CameraControllerCheck(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, int CamChnage, float Speed);

};
