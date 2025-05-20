#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaBase.h"
#include "BalingaMovementComponentBase.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(Hidden),
	CMOVE_Fly UMETA(DisplayName = "Fly"),
	CMOVE_MAX UMETA(Hidden),
};

UCLASS()
class UBalingaMovementComponentBase : public UCharacterMovementComponent
{
public:
	UBalingaMovementComponentBase();

	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

private:
	GENERATED_BODY() 

	class FSavedMove_Balinga : FSavedMove_Character
	{
	};

	// Set our own owner class so we can access the extra stuff we added to our Character child class (BalingaBase)
	UPROPERTY() TObjectPtr<ABalingaBase> BalingaOwner; 

	



protected:
	virtual void InitializeComponent() override;
};
