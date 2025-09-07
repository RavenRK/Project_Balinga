// Fill out your copyright notice in the Description page of Project Settings.


#include "BalingaStateMachine.h"
#include "BaseState.h"


ABalingaStateMachine::ABalingaStateMachine()
{
	PrimaryActorTick.bCanEverTick = true;

}
void ABalingaStateMachine::BeginPlay()
{
	Super::BeginPlay();
	InitState();
}
void ABalingaStateMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCanTickState && CurrentState && CurrentState->bCanTickState)
	{
		CurrentState->OnTickState();
	}
	if (bDebug)
	{
		if (CurrentState)
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Current State: %s"), *CurrentState->StateName.ToString()));
		if (StateHistory.Num() > 0)
		{
			for (int32 i = 0; i < StateHistory.Num(); i++)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("State History %d: %s"), i, *StateHistory[i]->StateName.ToString()));
			}
		}
	}
}

void ABalingaStateMachine::SwitchStateByKey(FString KeyState)
{
	UBaseState* NewState = StateMap.FindRef(KeyState);

	if (NewState->IsValidLowLevel())
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
	}
	else
	{
		if (bDebug)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found"));
		}
	}
}

void ABalingaStateMachine::SwitchState(UBaseState* NewState)
{

}

void ABalingaStateMachine::InitStateMachine()
{
	//SwitchStateByKey(InitialState);
}
void ABalingaStateMachine::InitState()
{
	// Create instances of all available states and store them in the StateMap
	for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
	{
		UBaseState* State = NewObject<UBaseState>(this, It.Value());
		StateMap.Add(It->Key, State);
	}
}

