// Fill out your copyright notice in the Description page of Project Settings.


#include "Jump_Balinga.h"

UJump_Balinga::UJump_Balinga()
{
	StateName = "Jump";
	bCanTickState = false;
	bCanRepeat = false;
}

void UJump_Balinga::OnEnterState(AActor* StateOwner)
{
	Super::OnEnterState(StateOwner);

}

void UJump_Balinga::OnTickState()
{

}

void UJump_Balinga::OnExitState()
{
	Super::OnExitState();

}
