// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStateBalinga.h"
#include "BalingaBase.h"              
#include "BalingaControllerBase.h"


void UBaseStateBalinga::OnEnterState(AActor* StateOwner)
{
	Super::OnEnterState(StateOwner);

    if (!BalingaRef)
        BalingaRef = Cast<ABalingaBase>(StateOwner);

    if (!BalingaControllerRef && BalingaRef)
        BalingaControllerRef = Cast<ABalingaControllerBase>(BalingaRef->GetController());



}

void UBaseStateBalinga::OnExitState()
{

}
