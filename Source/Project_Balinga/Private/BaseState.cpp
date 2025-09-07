// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseState.h"

void UBaseState::OnEnterState(AActor* StateOwner)
{
	OwnerActor = StateOwner;
}

void UBaseState::OnTickState()
{
}

void UBaseState::OnExitState()
{
}
