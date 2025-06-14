

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BalingaControllerBase.generated.h"

// forward declare for referencing
// include if access memebers
class ABalingaBase;
class UInputAction;
class UInputMappingContext;
class ABaseItem;
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
	TObjectPtr<UInputAction> MoveAction{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> JumpAction{ nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|other|Actions")
	TObjectPtr<UInputAction> LookAction{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|other|Actions")
	TObjectPtr<UInputAction> AttackAction{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|other|Actions")
	TObjectPtr<UInputAction> DropAction{ nullptr };

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);

	void StartJump(const FInputActionValue& InputActionValue);
	void EndJump(const FInputActionValue& InputActionValue);
	void Attack(const FInputActionValue& InputActionValue);
	void DropItem(const FInputActionValue& InputActionValue);

private:
	GENERATED_BODY()

	// Stores ref to one of our parent class', AActor's, InputComponent cast to an EnhInputComponent
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhInputComponent{ nullptr };

	// Stores ref to the player's pawn
	UPROPERTY()
	TObjectPtr<ABalingaBase> PlayerCharacter{ nullptr };
 

};
