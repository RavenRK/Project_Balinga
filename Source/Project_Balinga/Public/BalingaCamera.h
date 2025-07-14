// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECT_BALINGA_API UBalingaCamera : public UCharacterMovementComponent
{
	GENERATED_BODY()
private:
	// FOV settings 
	float defaultFOV = 90.0f;
	float defaultFlyFOV = 75.0f;
	float minDiveFOV = 60.0f;
	float maxDiveFOV = 120.0f;

	// Spring arm settings
	float defaultSpringArmLength = 150.0f;
	float minDiveSpringArmLength = 300.0f;
	float maxDiveSpringArmLength = 450.0f;
	float flySpringArmLength = 500.0f;
public:

	void Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	void CameraControllerCheck(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, int CamChnage);

};
