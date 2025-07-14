// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"



void UBalingaCamera::Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
	float CurrentFOV = Camera->FieldOfView;
	float CurrentArmLength = SpringArm->TargetArmLength;
	float NewFOV = FMath::FInterpTo(CurrentFOV, 125, DeltaTime, 2);
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, 450, DeltaTime, 2);

	Camera->SetFieldOfView(NewFOV);
	SpringArm->bUsePawnControlRotation = true;
}

void UBalingaCamera::Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
}
void UBalingaCamera::Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
}


void UBalingaCamera::ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
	float CurrentFOV = Camera->FieldOfView;
	float CurrentArmLength = SpringArm->TargetArmLength;
	float NewFOV = FMath::FInterpTo(CurrentFOV, 90, DeltaTime, 2);
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, 300, DeltaTime, 2);

	Camera->SetFieldOfView(NewFOV);
	SpringArm->bUsePawnControlRotation = true;
}

void UBalingaCamera::CameraControllerCheck(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, int CamChnage)
{
	switch (CamChnage)
	{
	case 0:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("ground_CamSettings"));
		ground_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	case 1:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Fly_CamSettings"));
		Fly_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	case 2:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Dive_CamSettings"));
		Dive_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	case 3:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Crash_CamSettings"));
		Crash_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	}
}