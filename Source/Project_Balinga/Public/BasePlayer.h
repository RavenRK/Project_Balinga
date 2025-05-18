// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BasePlayer.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;

UCLASS(Abstract)
class PROJECT_BALINGA_API ABasePlayer : public APlayerController
{
public: 
	//input mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ground")
	UInputMappingContext* PlayerInputMappingContext_Ground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Air")
	UInputMappingContext* PlayerInputMappingContext_Air;

	//input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ground|Movement")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ground|Movement")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ground|Movement")
	UInputAction* LookAction;
private:
	UPROPERTY()	//store a ref of input component cast to enhancedinputComponent
	UEnhancedInputComponent* EnhancedInputComponent = nullptr;
	UPROPERTY()	//store a ref to pawn we control
	ABasePlayer* PlayerCharacter = nullptr;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
};
