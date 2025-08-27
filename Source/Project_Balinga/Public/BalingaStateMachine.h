// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BalingaStateMachine.generated.h"

class BaseState;

UCLASS()
class PROJECT_BALINGA_API ABalingaStateMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	ABalingaStateMachine();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	FString InitialState;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
	TMap<FString, TSubclassOf<UBaseState>> AvailableStates;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Machine Debug")
	bool bDebug = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<UBaseState*> StateHistory;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine Debug", meta = (ClampMin = "0", ClampMix = "10", UImin = "0", UIMax = "10"))
	int32 StateHistoryLimit = 5;
	UPROPERTY(BlueprintReadOnly)
	UBaseState* CurrentState = nullptr;

	UPROPERTY()
	TMap<FString, UBaseState*> StateMap;

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void SwitchStateByKey(FString KeyState);

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void SwitchState(UBaseState* NewState);

	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void InitStateMachine();

private:

	bool bCanTickState = false;
	void InitState();
};
