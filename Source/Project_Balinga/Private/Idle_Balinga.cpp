// Fill out your copyright notice in the Description page of Project Settings.


#include "Idle_Balinga.h"
#include "BalingaBase.h"  

void UIdle_Balinga::OnEnterState(AActor* StateOwner)
{
    BalingaAnim->CurrentStateName = CurrentState->StateName;
}

void UIdle_Balinga::OnTickState()
{
    if (BalingaRef)
    {
        // If the player has velocity, they should leave Idle
        FVector Velocity = BalingaRef->GetVelocity();
        if (Velocity.SizeSquared() > 0.1f)
        {
            // TODO: Switch to Run/Walk state through StateMachine
            UE_LOG(LogTemp, Log, TEXT("%s wants to leave Idle"), *BalingaRef->GetName());
        }
    }
}

void UIdle_Balinga::OnExitState()
{
    if (BalingaRef)
    {
        UE_LOG(LogTemp, Log, TEXT("%s exited Idle State"), *BalingaRef->GetName());
    }
}
