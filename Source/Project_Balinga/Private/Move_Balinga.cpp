// Fill out your copyright notice in the Description page of Project Settings.


#include "Move_Balinga.h"
#include "BalingaStatemachine.h"

UMove_Balinga::UMove_Balinga()
{
	StateName = "Move";
	bCanTickState = true;
	bCanRepeat = true;
}

void UMove_Balinga::OnEnterState(AActor* StateOwner)
{
	Super::OnEnterState(StateOwner);
}

void UMove_Balinga::OnTickState()
{
	if (BalingaRef)
	{

		// Auto return to Idle if no velocity
		if (BalingaRef->GetVelocity().Size() < 1.f)
		{
			if (UBalingaStatemachine* SM = BalingaRef->FindComponentByClass<UBalingaStatemachine>())
			{
				SM->SwitchStateByKey("Idle");
			}
		}
	}
}

void UMove_Balinga::OnExitState()
{
	Super::OnExitState();

}
