#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaMovementComponentBase.generated.h"

//UENUM(BlueprintType)
//enum ECustomMovementMode
//{
//	CMOVE_None UMETA(Hidden),
//	CMOVE_Fly UMETA(DisplayName = "Fly"),
//	CMOVE_MAX UMETA(Hidden),
//};

UCLASS()
class PROJECT_BALINGA_API UBalingaMovementComponentBase : public UCharacterMovementComponent
{
public:
	UBalingaMovementComponentBase();

private:
	GENERATED_BODY()

	//class FSavedMove_Test : FSavedMove_Character
	//{
	//};

	// Transient (idk what that means, maybe stuff that changes throughout runtime)


protected:
	//virtual void InitializeComponent() override;
};
