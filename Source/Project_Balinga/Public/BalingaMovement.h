#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BalingaBase.h"
#include "DrawDebugHelpers.h"
#include "BalingaMovement.generated.h"


class UAimerBase;

// Similar to EMovemementMode in EngineTypes.h, none and max are there out of convention
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(Hidden), 
	CMOVE_Glide UMETA(DisplayName = "Glide"),
	CMOVE_MAX UMETA(Hidden),
};


UCLASS(Blueprintable, Category="Movement")
class UBalingaMovement : public UCharacterMovementComponent
{
	typedef TFunction<FVector(void)>&& CalcForceOrTorqueDef;
	typedef UCharacterMovementComponent Super;

public:
	void EnterGlide();
	void ExitGlide();

	void FlapPressed();

	void LandPressed();
	void LandReleased();

	template<typename CalcForceOrTorque
	>
	void TestTorqueAtDifferentDT(CalcForceOrTorque CalcTorque, CalcForceOrTorqueDef CalcOtherTorque, float StartDeltaTime, float DeltaDeltaTime, int IterationAmount);

private:
	GENERATED_BODY()

	bool bWantsToLand;

	
	UPROPERTY(EditDefaultsOnly, Category = "General") float PredictionScale;
	UPROPERTY(EditDefaultsOnly, Category = "General") bool bShouldAverageForcesAndTorques;
	UPROPERTY(EditDefaultsOnly, Category = "General") float FixedDeltaTimeDivisor;
	float AccumulatedDeltaTime;
	float FixedDeltaTime;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") bool bShouldLimitGlideSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") float MaxGlideSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Thrust") float DefaultThrustScale;	
	float ThrustScale;
	FVector ThrustThisFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultDragScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultMinDragDesiredScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultDragDesiredScaleScale;
	float DragScale;
	float MinDragDesiredScale;
	float DragDesiredScaleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultLiftScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultMinLiftDesiredScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultLiftDesiredScaleScale;
	float LiftScale;
	float MinLiftDesiredScale;
	float LiftDesiredScaleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") bool bShouldSnapRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MomentInertia;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AimerAoaScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float WingTorqueScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float WingMidpointDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinRollAccel;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float DefaultAngularDampScale;
	float AngularDampScale;

	FVector AngularVelocity;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Coefficient") float DefaultSurfaceArea;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Coefficient") FVector DefaultWindVelocity;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Coefficient") float DefaultAirDensity;
	float SurfaceArea;
	FVector WindVelocity;
	float AirDensity;

	UPROPERTY() TObjectPtr<ABalingaBase> BalingaOwner; // Used to access Balinga things outside CharacterOwner


	void PhysGlide(float DeltaTime, int32 Iterations);

	TArray<FVector> CalcGlideForceAndTorque(FVector GivenVelocity, FVector GivenAngularVelocity, FVector GivenWindVelocity, float DeltaTime);


	FVector CalcLift(FVector FlowDirection, FVector DesiredDifference, FVector ActorRight, FVector ActorForward, FVector ActorUp, float DeltaTime);
	TArray<FVector> CalcLifts(FVector FlowDirection, FVector DesiredDifference, FVector ActorRight, FVector ActorForward, FVector ActorUp, float DeltaTime);

	FVector CalcDrag(FVector FlowDirection, FVector DesiredDifferenceDirection, float DeltaTime);

	FVector CalcLiftRoll(TArray<FVector> WingLifts, FVector ActorForward, FVector ActorUp);
	FVector CalcLiftRoll(FVector FlowVelocity, FVector ActorRight, FVector ActorForward, FVector ActorUp);

	FVector CalcLiftPitch(TArray<FVector> WingLifts, FVector ActorRight, FVector ActorUp);

	FVector CalcAngularDamp(FVector GivenAngularVelocity, float DeltaTime);
	FVector CalcAngularDamp(FVector GivenAngularVelocity, float GivenAngularDampScale, float DeltaTime);

	FVector CalcDesiredDiffDirection(FVector FlowDirection, FVector ActorForward);

	float CalcAngleOfAttack(FVector GivenVelocity, FVector ActorRight, FVector ActorUp);
	float CalcAoaSign(FVector GivenVelocity, FVector ActorRight, FVector ActorUp);

	// Adds a given force at a given position relative to the actor. 
	void AddForceAtPos(FVector Force, FVector Position, float DeltaTime);

	void AddForceToVel(FVector Force, float DeltaTime);
	FVector CalcForceAccel(FVector Force, float DeltaTime);

	FVector CalcTorqueFromForceAtPos(FVector Force, FVector Position);
	void AddTorqueToAngularVel(FVector Torque, float DeltaTime);
	FVector CalcTorqueAccel(FVector Torque, float DeltaTime);

	float CalcLimitedNewSpeedScale(FVector GivenVelocity, FVector NewVelocity);

	UPROPERTY(EditAnywhere) bool bShouldEnableDebug;

	UPROPERTY(EditAnywhere) float MovementLogGroup;
	UPROPERTY(EditAnywhere) float VelocityLogOffset;
	UPROPERTY(EditAnywhere) float AngularVelocityLogOffset;

	UPROPERTY(EditAnywhere) float ForceLogGroup;
	TArray<FVector> WingLiftAccels;
	UPROPERTY(EditAnywhere) float WingLiftAccelsLogOffset;
	FVector DragAccel;
	UPROPERTY(EditAnywhere) float DragAccelLogOffset;

	UPROPERTY(EditAnywhere) float TorqueLogGroup;
	FVector2D AimerPosition;
	UPROPERTY(EditAnywhere) float AimerPositionLogOffset;
	FVector WingDirection;
	UPROPERTY(EditAnywhere) float WingDirectionLogOffset;

	UPROPERTY(EditAnywhere) float AoaLogGroup;
	float AngleOfAttack;
	UPROPERTY(EditAnywhere) float AoaLogOffset;
	float AoaDot;
	UPROPERTY(EditAnywhere) float AoaDotLogOffset;
	float LiftCoefficient;
	UPROPERTY(EditAnywhere) float LiftAoaLogOffset;
	float DragCoefficient;
	UPROPERTY(EditAnywhere) float DragAoaLogOffset;

	UPROPERTY(EditAnywhere) float DesiredLogGroup;
	UPROPERTY(EditAnywhere) float DesiredDifferenceLogOffset;
	UPROPERTY(EditAnywhere) float LiftDesiredLogOffset;
	UPROPERTY(EditAnywhere) float DragDesiredLogOffset;

protected:
	virtual void InitializeComponent() override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

public:
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags);

private:

	enum EForces
	{
		FORCES_LeftWingLift,
		FORCES_RightWingLift,
		FORCES_Drag,
		FORCES_Thrust,
		FORCES_Gravity,
		FORCES_MAX
	};

	enum ETorques
	{
		TORQUES_LiftRoll,
		TORQUES_AngularDamp,
		TORQUES_MAX
	};

	// Holds data used in phys functions that might change and thus need to be saved to accurately replicate moves on the server
	class FSavedMove_Balinga : public FSavedMove_Character
	{

		typedef FSavedMove_Character Super;

	public:

		enum CompressedFlags
		{
			FLAG_JumpPressed = 0x01,	// Jump pressed
			FLAG_WantsToCrouch = 0x02,	// Wants to crouch
			FLAG_Reserved_1 = 0x04,	// Reserved for future use
			FLAG_Reserved_2 = 0x08,	// Reserved for future use
			// Remaining bit masks are available for custom flags.
			FLAG_Land = 0x10,
			FLAG_Custom_1 = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,
		};

		bool saved_bWantsToLand;

		float saved_ThrustScale;

		float saved_DragScale;
		float saved_MinDragDesiredScale;
		float saved_DragDesiredScaleScale;

		float saved_LiftScale;
		float saved_MinLiftDesiredScale;
		float saved_LiftDesiredScaleScale;
		// Gravity is not ours

		float saved_AngleOfAttack;
		float saved_SurfaceArea;
		FVector saved_WindVelocity;
		float saved_AirDensity;
		

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
