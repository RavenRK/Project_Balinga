// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaStatemachine.h"
#include "BaseState.h"


UBalingaStatemachine::UBalingaStatemachine()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBalingaStatemachine::BeginPlay()
{
	Super::BeginPlay();
	InitState();
	InitStateMachine();
}

void UBalingaStatemachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bCanTickState && CurrentState && CurrentState->bCanTickState)
    {
        CurrentState->OnTickState();
    }
    if (bDebug)
    {
        if (CurrentState)
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Current State: %s"), *CurrentState->StateName.ToString()));
       
        if (StateHistory.Num() > 0)
        {
            for (int32 i = 0; i < StateHistory.Num(); i++)
				GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("State History %d: %s"), i, *StateHistory[i]->StateName.ToString()));
        }
    }

}

void UBalingaStatemachine::SwitchStateByKey(FName KeyState)
{
    UBaseState* NewState = StateMap.FindRef(KeyState);

    if (IsValid(NewState))
    {
        if (!CurrentState)
        {
            CurrentState = NewState;
        }
        else
        {
            if (CurrentState->GetClass() == NewState->GetClass() && CurrentState->bCanRepeat == false)
            {
                if (bDebug)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State is the same and cannot repeat"));
                }
            }
            else
            {
                bCanTickState = false;
                CurrentState->OnExitState();

                if (StateHistory.Num() >= StateHistoryLimit)
                {
                    StateHistory.RemoveAt(0);
                }
                StateHistory.Push(CurrentState);

                CurrentState = NewState;
            }
        }

        if (CurrentState)
        {
            CurrentState->OnEnterState(GetOwner());
            bCanTickState = true;
        }

        if (AActor* Owner = GetOwner())
        {
            if (USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
            {
                if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
                {
                    if (UBalingaAnimInstance* BalingaAnim = Cast<UBalingaAnimInstance>(AnimInst))
                    {
                        BalingaAnim->CurrentStateName = CurrentState->StateName;
                    }
                }
            }
        }
    }
    else
    {
        if (bDebug)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found"));
        }
    }
}

void UBalingaStatemachine::SwitchState(UBaseState* NewState)
{
    if (!IsValid(NewState)) return;

    if (CurrentState)
    {
        if (CurrentState->GetClass() == NewState->GetClass() && !CurrentState->bCanRepeat)
        {
            if (bDebug)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State is the same and cannot repeat"));
            }
            return;
        }

        bCanTickState = false;
        CurrentState->OnExitState();

        if (StateHistory.Num() >= StateHistoryLimit)
        {
            StateHistory.RemoveAt(0);
        }
        StateHistory.Push(CurrentState);
    }

    CurrentState = NewState;
    CurrentState->OnEnterState(GetOwner());
    bCanTickState = true;
}

void UBalingaStatemachine::InitStateMachine()
{
    SwitchStateByKey(InitialState);
}

void UBalingaStatemachine::InitState()
{
    // Create instances of all available states and store them in the StateMap
    for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
    {
        UBaseState* State = NewObject<UBaseState>(this, It.Value());
        StateMap.Add(It->Key, State);
    }
}

