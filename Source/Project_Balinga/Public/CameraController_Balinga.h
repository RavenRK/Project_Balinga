// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BalingaBase.h"

class UCameraComponent;
class USpringArmComponent;

class PROJECT_BALINGA_API CameraController_Balinga
{
private:
	// FOV settings 
	UPROPERTY(EditAnywhere, Category = "Camera")
	float defaultFOV = 90.0f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float defaultFlyFOV = 75.0f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float minDiveFOV = 60.0f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float maxDiveFOV = 120.0f;

	// Spring arm settings
	UPROPERTY(EditAnywhere,Category  = "Camera|SpringArm")
	float defaultSpringArmLength = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|SpringArm")
	float minDiveSpringArmLength = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|SpringArm")
	float maxDiveSpringArmLength = 450.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|SpringArm")
	float flySpringArmLength = 500.0f;
public:
	void Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm);
	void Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm);
	void Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm);
	void ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm);

};
