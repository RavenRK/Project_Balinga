// Fill out your copyright notice in the Description page of Project Settings.


#include "Idle_Balinga.h"
#include "BalingaStatemachine.h"

UIdle_Balinga::UIdle_Balinga()
{
	StateName = "Idle";
	bCanTickState = true;
	bCanRepeat = true;
}

void UIdle_Balinga::OnEnterState(AActor* StateOwner)
{
	Super::OnEnterState(StateOwner);

}

void UIdle_Balinga::OnTickState()
{
    if (BalingaRef)
    {
        float Speed = BalingaRef->GetVelocity().Size();

        // If the character starts moving -> switch to Move state
        if (Speed > 1)
        {
            if (UBalingaStatemachine* SM = BalingaRef->FindComponentByClass<UBalingaStatemachine>())
            {
                SM->SwitchStateByKey("Move");
            }
        }
    }
}

void UIdle_Balinga::OnExitState()
{
	Super::OnExitState();

}

