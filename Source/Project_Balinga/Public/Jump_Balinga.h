// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStateBalinga.h"
#include "Jump_Balinga.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BALINGA_API UJump_Balinga : public UBaseStateBalinga
{
	GENERATED_BODY()

public:
	UJump_Balinga();

	virtual void OnEnterState(AActor* StateOwner) override;
	virtual void OnTickState() override;
	virtual void OnExitState() override;
	
};
