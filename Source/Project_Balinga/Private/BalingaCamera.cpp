// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

void UBalingaCamera::Fly_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
	float CurrentFOV = Camera->FieldOfView;
	float CurrentArmLength = SpringArm->TargetArmLength;

	float NewFOV = FMath::FInterpTo(CurrentFOV, 100, DeltaTime, 2);
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, 450, DeltaTime, 2);

	Camera->SetFieldOfView(NewFOV);
	SpringArm->TargetArmLength = NewArmLength;
}
void UBalingaCamera::ground_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{
	float CurrentFOV = Camera->FieldOfView;
	float CurrentArmLength = SpringArm->TargetArmLength;
	float NewFOV = FMath::FInterpTo(CurrentFOV, 90, DeltaTime, 2);
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, 300, DeltaTime, 2);

	Camera->SetFieldOfView(NewFOV);
	SpringArm->TargetArmLength = NewArmLength;
}
void UBalingaCamera::Dive_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, float Speed)
{
	float CurrentFOV = Camera->FieldOfView;
	float CurrentArmLength = SpringArm->TargetArmLength;

	float MinSpeed = 0.0f;
	float MaxSpeed = 4500;

	float MinFOV = 90.0f;
	float MaxFOV = 125.0f;

	float MinArmLength = 300.0f;
	float MaxArmLength = 500.0f;

	//clamp max/min fov & interpolate
	float TargetFOV = FMath::GetMappedRangeValueClamped(FVector2D(MinSpeed, MaxSpeed), FVector2D(MinFOV, MaxFOV), Speed);
	float TargetArmLength = FMath::GetMappedRangeValueClamped(FVector2D(MinSpeed, MaxSpeed), FVector2D(MinArmLength, MaxArmLength), Speed);

	float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 5.0f);
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, 5.0f);

	Camera->SetFieldOfView(NewFOV);
	SpringArm->TargetArmLength = NewArmLength;
}
void UBalingaCamera::Crash_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{

}
void UBalingaCamera::RL_CamSettings(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime)
{

}


void UBalingaCamera::CameraControllerCheck(UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime, int CamChnage, float Speed)
{
	switch (CamChnage)
	{
	case 0:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("ground_CamSettings"));
		ground_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	case 1:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Fly_CamSettings"));
		Fly_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	case 2:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Dive_CamSettings"));
		Dive_CamSettings(Camera, SpringArm, DeltaTime, Speed);
		break;
	case 3:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Crash_CamSettings"));
		Crash_CamSettings(Camera, SpringArm, DeltaTime);
		break;
	}
}