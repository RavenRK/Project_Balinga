// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStateBalinga.h"

#include "Move_Balinga.generated.h"


UCLASS()
class PROJECT_BALINGA_API UMove_Balinga : public UBaseStateBalinga
{
	GENERATED_BODY()
	
public:
	UMove_Balinga();

	virtual void OnEnterState(AActor* StateOwner) override;
	virtual void OnTickState() override;
	virtual void OnExitState() override;

};
