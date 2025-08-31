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

		UPROPERTY(BlueprintReadOnly, Category = "Balinga State")
		ABalingaBase* BalingaRef = nullptr;

		UPROPERTY(BlueprintReadOnly, Category = "Balinga State")
		ABalingaControllerBase* BalingaControllerRef = nullptr;


		virtual void OnEnterState(AActor* StateOwner) override;
		virtual void OnExitState() override;
		//virtual void OnTickState() override;

		//get Balinga ref
		UFUNCTION(BlueprintPure, Category = "Balinga State")
		ABalingaBase* GetBalinga() const { return BalingaRef; }

		//get Balinga Controller ref
		UFUNCTION(BlueprintPure, Category = "Balinga State")
		ABalingaControllerBase* GetBalingaController() const { return BalingaControllerRef; }

		//play animation montage
		UFUNCTION(BlueprintCallable, Category = "Balinga State")
		void PlayAnimationMontage(UAnimMontage* Montage);
};
