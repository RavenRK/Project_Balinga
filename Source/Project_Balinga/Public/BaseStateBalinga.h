// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "BaseStateBalinga.generated.h"


class ABalingaBase;
class ABalingaControllerBase;

UCLASS()
class PROJECT_BALINGA_API UBaseStateBalinga : public UBaseState
{
	GENERATED_BODY()

	public:

		UPROPERTY(BlueprintReadOnly)
		class ABalingaBase* BalingaRef = nullptr;
		UPROPERTY(BlueprintReadOnly)
		class ABalingaControllerBase* BalingaControllerRef = nullptr;


		virtual void OnEnterState(AActor* StateOwner) override;
		//virtual void OnTickState() override;
		virtual void OnExitState() override;

		//UFUNCTION(BlueprintCallable, Category = "Balinga State")
		//void PlayAnimationMontage(UAnimMontage* Montage)
		//{
		//	if (BalingaRef && Montage)
		//		BalingaRef->PlayAnimMontage(Montage);

		//}

		FORCEINLINE ABalingaBase* GetBalinga() const { return BalingaRef; }
		FORCEINLINE ABalingaControllerBase* GetBalingaController() const { return BalingaControllerRef; }
};
