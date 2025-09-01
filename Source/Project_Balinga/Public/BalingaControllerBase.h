


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BalingaControllerBase.generated.h"

// forward declare for referencing
// include if access memebers
class ABalingaBase;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
class ABaseItem;
class ABalingaHudBase;
class UAimerBase;
struct FInputActionValue;

UCLASS(Abstract)
class ABalingaControllerBase : public APlayerController
{
public:
	//	<< Input Mapping >>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Mapping")
	TObjectPtr<UInputMappingContext> BaseInputMapping{ nullptr };
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Air Mapping")
	//TObjectPtr<UInputMappingContext> AirInputMapping{ nullptr };

		//	<< Input Action >>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Other|Actions")
	TObjectPtr<UInputAction> LookAction{ nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> MoveAction{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Ground|Actions")
	TObjectPtr<UInputAction> JumpAction{ nullptr };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|In Air / Flying|Actions")
	TObjectPtr<UInputAction> LandAction{ nullptr };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Other|Actions")
	TObjectPtr<UInputAction> AttackAction{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement|Other|Actions")
	TObjectPtr<UInputAction> DropAction{ nullptr };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CamAction")
	TObjectPtr<UInputAction> CamAction{ nullptr };

	FVector2D GetAimerPosition();

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	
	void Look(const FInputActionValue& InputActionValue);

	void Move(const FInputActionValue& InputActionValue);
	void StartJump(const FInputActionValue& InputActionValue);
	void EndJump(const FInputActionValue& InputActionValue);

	void Land(const FInputActionValue& InputActionValue);

	void Attack(const FInputActionValue& InputActionValue);
	void DropItem(const FInputActionValue& InputActionValue);


private:
	GENERATED_BODY()

	UPROPERTY() TObjectPtr<UEnhancedInputComponent> EnhInputComponent{ nullptr };

	UPROPERTY() TObjectPtr<ABalingaBase> Balinga{ nullptr };

	UPROPERTY() TObjectPtr<ABalingaHudBase> Hud;

	
	
};
