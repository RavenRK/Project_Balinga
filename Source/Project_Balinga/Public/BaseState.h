// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseState.generated.h"

UCLASS()
class PROJECT_BALINGA_API UBaseState : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> OwnerActor;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bCanTickState = false;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bCanRepeat = false;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName StateName;

	virtual void OnEnterState(AActor* StateOwner);
	virtual void OnTickState();
	virtual void OnExitState();
};
