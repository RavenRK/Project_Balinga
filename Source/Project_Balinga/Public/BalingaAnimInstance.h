// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BalingaAnimInstance.generated.h"

UCLASS()
class PROJECT_BALINGA_API UBalingaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Balinga State")
	FName CurrentStateName;
};
