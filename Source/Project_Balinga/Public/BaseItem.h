// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UCLASS()
class PROJECT_BALINGA_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	bool canUse = true;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
