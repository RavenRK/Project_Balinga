

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BalingaControllerBase.generated.h"

// forward declare for referencing
// include if access memebers
class ABalingaBase;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(Abstract)
class ABalingaControllerBase : public APlayerController
{
public:
	//	<< Input Mapping >>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Mapping")
	TObjectPtr<UInputMappingContext> GroundInputMapping{ nullptr };
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Air Mapping")
	//TObjectPtr<UInputMappingContext> AirInputMapping{ nullptr };

		//	<< Input Action >>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> ActionMove{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> ActionLook{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> ActionJump{ nullptr };
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionToggleSprint{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionToggleCrouch{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionPsyBlast{ nullptr };*/
protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void StartJump(const FInputActionValue& InputActionValue);
	void EndJump(const FInputActionValue& InputActionValue);
	void Attack(const FInputActionValue& InputActionValue);
	//void HandleToggleSprint();
private:
	GENERATED_BODY()

	// Stores ref to one of our parent class', AActor's, InputComponent cast to an EnhInputComponent
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhInputComponent{ nullptr };

	// Stores ref to the player's pawn
	UPROPERTY()
	TObjectPtr<ABalingaBase> PlayerCharacter{ nullptr };

	//jump variable
 

};
