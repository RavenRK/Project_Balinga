// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BalingaAnimInstance.h"

#include "BalingaStatemachine.generated.h"

class UBaseState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_BALINGA_API UBalingaStatemachine : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBalingaStatemachine();

protected:
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
    FName InitialState = "Idle";

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine")
    TMap<FName, TSubclassOf<UBaseState>> AvailableStates;

    //DEBUG
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State Machine Debug")
    bool bDebug = true;

    UPROPERTY(BlueprintReadOnly)
    TArray<UBaseState*> StateHistory;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State Machine Debug", meta = (ClampMin = "0", ClampMix = "10", UImin = "0", UIMax = "10"))
    int32 StateHistoryLimit = 5;

    UPROPERTY(BlueprintReadOnly)
    UBaseState* CurrentState = nullptr;

    UPROPERTY()
    class UBalingaAnimInstance* AnimInstance;

    UPROPERTY()
    TMap<FName, UBaseState*> StateMap;

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    void SwitchStateByKey(FName KeyState);

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    void SwitchState(UBaseState* NewState);

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    void InitStateMachine();

private:

    bool bCanTickState = false;
    void InitState();
};
