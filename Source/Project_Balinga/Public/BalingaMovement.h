#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaBase.h"
#include "DrawDebugHelpers.h"

#include "BalingaMovement.generated.h"

// Similar to EMovemementMode in EngineTypes.h, none and max are there out of convention
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(Hidden), 
	CMOVE_Fly UMETA(DisplayName = "Fly"),
	CMOVE_MAX UMETA(Hidden),
};


UCLASS(Blueprintable, Category="Movement")
class UBalingaMovement : public UCharacterMovementComponent
{
	typedef UCharacterMovementComponent Super;

public:
	UBalingaMovement();

	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

private:
	GENERATED_BODY()

	// Each force is an acceleration multiplied by mass (f = ma) that we apply to the velocity of their corresponding axes
	// They don't accumulate, the velocity accumulates their acceleration
	// They're calculated each frame they're needed off of the flight parameters and other data
	// They're floats because the magnitude of gravity (GravityZ) is
	// Gravity stuff is already built in

	// Editor scales don't change during runtime, we set them based on what we feel is good
	UPROPERTY(EditDefaultsOnly) float defaultThrustScale;
	UPROPERTY(EditDefaultsOnly) float defaultDirectionalScaleScale;
	UPROPERTY(EditDefaultsOnly) float defaultDragScale;
	UPROPERTY(EditDefaultsOnly) float defaultLiftScale;
	float thrustScale;
	float directionalScaleScale;
	float dragScale;
	float liftScale;

	UPROPERTY(EditDefaultsOnly) float defaultAngleOfAttack;
	UPROPERTY(EditDefaultsOnly) float defaultSurfaceArea;
	UPROPERTY(EditDefaultsOnly) FVector defaultWindVelocity;
	UPROPERTY(EditDefaultsOnly) float defaultAirDensity;
	float angleOfAttack;
	float surfaceArea;
	FVector windVelocity;
	float airDensity;

	UPROPERTY() TObjectPtr<ABalingaBase> BalingaOwner; // Used to access Balinga things outside CharacterOwner
	FVector actorForward;
	FVector actorUp;

	void PhysFly(float deltaTime, int32 Iterations);

public:
	void EnterFly();
	void ExitFly();
	void DoFlap();

protected:
	virtual void InitializeComponent() override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags);

private:
	// Holds data used in phys functions that might change and thus need to be saved to accurately replicate moves on the server
	class FSavedMove_Balinga : public FSavedMove_Character
	{

		typedef FSavedMove_Character Super;

	public:
		float saved_thrustScale;
		float saved_directionalScaleScale;
		float saved_dragScale;
		float saved_liftScale;
		// Gravity is not ours

		float saved_angleOfAttack;
		float saved_surfaceArea;
		FVector saved_windVelocity;
		float saved_airDensity;

		FVector saved_actorForward;
		FVector saved_actorUp;
		

	private:
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear();
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual uint8 GetCompressedFlags() const override;
	};

	class FNetworkPredictionData_Client_Balinga : public FNetworkPredictionData_Client_Character
	{
		typedef FNetworkPredictionData_Client_Character Super;

	public:

		FNetworkPredictionData_Client_Balinga(const UCharacterMovementComponent& ClientMovement);

		virtual FSavedMovePtr AllocateNewMove() override;
	};
};
