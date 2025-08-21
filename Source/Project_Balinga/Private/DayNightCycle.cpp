// Fill out your copyright notice in the Description page of Project Settings.


#include "DayNightCycle.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"

ADayNightCycle::ADayNightCycle()
{
	PrimaryActorTick.bCanEverTick = true;

	DayLength = 120.f; // 120s
	CurrentTimeOfDay = 0.f;
}

void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();

}

void ADayNightCycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!WorldSun) return;

	CurrentTimeOfDay += DeltaTime / DayLength;
	if (CurrentTimeOfDay > 1.0f)
		CurrentTimeOfDay -= 1.0f;

	float SunPitch = CurrentTimeOfDay * 360.0f;
	FRotator NewRotation = FRotator(SunPitch, -90.0f, 0.0f);
	WorldSun->SetActorRotation(NewRotation);

	float LightAlpha = FMath::Clamp(FMath::Sin(CurrentTimeOfDay * PI), 0.0f, 1.0f);
	float SunIntensity = FMath::Lerp(0.0f, 6, LightAlpha);
	WorldSun->GetLightComponent()->SetIntensity(SunIntensity);
}

