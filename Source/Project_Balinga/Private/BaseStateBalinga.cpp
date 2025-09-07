// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStateBalinga.h"
#include "BalingaControllerBase.h"

void UBaseStateBalinga::OnEnterState(AActor* StateOwner)
{
	Super::OnEnterState(StateOwner);

    BalingaRef = Cast<ABalingaBase>(StateOwner);
    if (BalingaRef)
    {
        BalingaControllerRef = Cast<ABalingaControllerBase>(BalingaRef->GetController());
    }

}

void UBaseStateBalinga::OnExitState()
{

}

void UBaseStateBalinga::PlayAnimationMontage(UAnimMontage* Montage)
{
    if (BalingaRef && Montage)
    {
        if (USkeletalMeshComponent* Mesh = BalingaRef->GetMesh())
        {
            if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
            {
                AnimInstance->Montage_Play(Montage);
            }
        }
    }
}