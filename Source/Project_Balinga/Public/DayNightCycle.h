// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycle.generated.h"

UCLASS()
class PROJECT_BALINGA_API ADayNightCycle : public AActor
{
	GENERATED_BODY()
	
public:	
	ADayNightCycle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	class ADirectionalLight* WorldSun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float DayLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float MaxSunIntensity = 6.0f;

	float CurrentTimeOfDay;
};
