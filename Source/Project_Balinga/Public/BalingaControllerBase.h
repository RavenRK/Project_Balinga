

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BalingaControllerBase.generated.h"

// Can just forward declare if we're just referencing
// Would need to include if we're trying to access memebers
class ABalingaBase;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * 
 */
UCLASS(Abstract)
class ABalingaControllerBase : public APlayerController
{
public:
	// The InputAction to map to movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionMove{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionLook{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionJump{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionToggleSprint{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionToggleCrouch{ nullptr };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionPsyBlast{ nullptr };


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputMappingContext> InputMappingContext{ nullptr };

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleLook(const FInputActionValue& InputActionValue);
	void HandleJump();
	void HandleToggleSprint();

private:
	GENERATED_BODY()

	// Stores a reference to one of our parent class', AActor's, InputComponent cast to an EnhancedInputComponent
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent{ nullptr };

	// Stores a reference to the player's pawn
	UPROPERTY()
	TObjectPtr<ABalingaBase> PlayerCharacter{ nullptr };

};
