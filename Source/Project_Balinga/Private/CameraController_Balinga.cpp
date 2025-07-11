// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraController_Balinga.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

void CameraController_Balinga::Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm)
{
}

void CameraController_Balinga::Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm)
{
}

void CameraController_Balinga::Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm)
{
	// camer shake and stuff
}

void CameraController_Balinga::ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm)
{
	if (Camera && SpringArm)
	{
		SpringArm->TargetArmLength = 300.0f; // Distance from the character
		SpringArm->SocketOffset = FVector(0.0f, 0.0f, 50.0f); // Offset above the character
		SpringArm->bUsePawnControlRotation = true; // Follow the character's rotation
		Camera->FieldOfView = 90.0f; // Wider view for ground movement
	}
}
