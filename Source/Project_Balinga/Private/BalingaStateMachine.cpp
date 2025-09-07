// Fill out your copyright notice in the Description page of Project Settings.

#include "BalingaStatemachine.h"
#include "BaseState.h"
#include "BalingaAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include "Engine/Engine.h" // Need for GEngine->AddOnScreenDebugMessage dont forget !



UBalingaStateMachine::UBalingaStateMachine()
{
	PrimaryComponentTick.bCanEverTick = true;
	InitialState = "Idle";
}

void UBalingaStateMachine::BeginPlay()
{
	Super::BeginPlay();
	InitState();
	InitStateMachine();
}

void UBalingaStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanTickState && CurrentState && CurrentState->bCanTickState)
		CurrentState->OnTickState();

	//DEBUG pritn current state
	if (CurrentState)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			1.f,
			FColor::Green,
			FString::Printf(TEXT("Current State: %s"), *CurrentState->StateName.ToString())
		);

		UE_LOG(LogTemp, Warning, TEXT("Current State: %s"), *CurrentState->StateName.ToString());
	}
}

void UBalingaStateMachine::SwitchStateByKey(FName KeyState)
{
	UBaseState* NewState = StateMap.FindRef(KeyState);

	if (!IsValid(NewState))
	{
		return;
	}

	if (!CurrentState)
	{
		CurrentState = NewState;
	}
	else
	{
		if (CurrentState->GetClass() == NewState->GetClass() && !CurrentState->bCanRepeat)
		{
			return;
		}

		bCanTickState = false;
		CurrentState->OnExitState();

		if (StateHistory.Num() >= StateHistoryLimit)
		{
			StateHistory.RemoveAt(0);
		}
		StateHistory.Push(CurrentState);

		CurrentState = NewState;
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
void UBalingaStateMachine::SwitchState(UBaseState* NewState)
{
	if (!IsValid(NewState))
	{
		return;
	}

	if (CurrentState)
	{
		if (CurrentState->GetClass() == NewState->GetClass() && !CurrentState->bCanRepeat)
		{
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

void UBalingaStateMachine::InitStateMachine()
{
	//SwitchStateByKey(InitialState);
}

void UBalingaStateMachine::InitState()
{
	for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
	{
		UBaseState* State = NewObject<UBaseState>(this, It.Value());
		StateMap.Add(It->Key, State);
	}
}
