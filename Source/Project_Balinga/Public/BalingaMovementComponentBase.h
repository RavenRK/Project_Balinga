#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaBase.h"
#include "BalingaMovementComponentBase.generated.h"

// Similar to EMovemementMode in EngineTypes.h, none and max are there out of convention
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(Hidden), 

	// Fly and glide might become one depending on if they handle physics the same way
	CMOVE_Fly UMETA(DisplayName = "Fly"),
	CMOVE_Glide UMETA(DisplayName = "Glide"),
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

	// Holds data used in phys functions that might change and thus need to be remembered to accurately replicate moves on the server
	class FSavedMove_Balinga : FSavedMove_Character
	{
	public: 
		bool saved_bFlapInput;
		int saved_defaultInitThrustMagnitude;
		int8 saved_initThrustMagnitude;

		int8 saved_angleOfAttack;
		int8 saved_surfaceArea;
		int8 saved_airDensity;

		FVector saved_actorForward;
		FVector saved_actorUp;

	private:
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	bool bFlapInput;
	int8 defaultInitThrustMagnitude;
	int8 initThrustMagnitude;

	// Each force is an acceleration multiplied by mass (f = ma) that we apply to the velocity of their corresponding axes
	// They don't accumulate, the velocity accumulates their acceleration
	// They're calculated each frame they're needed off of the flight parameters and other data
	// They're floats because the magnitude of gravity (GravityZ) is

	// Scales aren't calculated at all, we set them based on what we feel is good
	float thrustScale;
	float liftScale;
	float dragScale;

	int8 angleOfAttack;
	int8 surfaceArea;
	FVector windVelocity;
	float airDensity;

	UPROPERTY() TObjectPtr<ABalingaBase> BalingaOwner; // Used to access Balinga things outside CharacterOwner
	FVector actorForward;
	FVector actorUp;

	void PhysFly(float deltaTime, int32 Iterations);

protected:
	virtual void InitializeComponent() override;
};
