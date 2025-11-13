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



protected:
	virtual void InitializeComponent() override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

public:
	virtual void PostInitProperties() override;

	void EnterGlide();
	void ExitGlide();

	void FlapPressed();

	void LandPressed();
	void LandReleased();

private:
	GENERATED_BODY()

	bool bWantsToLand;
	
	UPROPERTY(EditDefaultsOnly, Category = "General") float FixedDeltaTimeDivisor;
	UPROPERTY(EditDefaultsOnly, Category = "General") bool bShouldAverageForcesAndTorques; // Eventually specify to glide (probs when i start flapping)
	UPROPERTY(EditDefaultsOnly, Category = "General") float PredictionScale;
	float AccumulatedDeltaTime;
	float FixedDeltaTime;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") TArray<bool> bWhichGlideForcesAndTorquesEnabled;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") bool bShouldLimitGlideSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") float MaxGlideSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Thrust") float DefaultThrustScale;	
	float ThrustScale;
	FVector ThrustThisFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Gravity") float GlideGravityScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultLiftScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultMaxLiftDesiredScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float CriticalAoa;
	float LiftScale;
	float MaxLiftDesiredScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultDragScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultMaxDragDesiredScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float ParasiticDrag;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DragCoefficientAoaScale;
	float DragScale;
	float MaxDragDesiredScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MomentInertia;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") bool bShouldSnapRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") bool bShouldUseVectorRotation;


	// Categorise deeper when more params

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinRollForceAccel;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float BonusLiftRollScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinPitchForceAccel;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float BonusLiftPitchScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float DragTorqueScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float DefaultAngularDampScale;
	float AngularDampScale;

	FVector AngularVelocity;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Coefficient") float DefaultSurfaceArea;
	float SurfaceArea;
	FVector WindVelocity;
	float AirDensity;


	UPROPERTY() TObjectPtr<ABalingaBase> BalingaOwner; // Used to access Balinga things outside CharacterOwner

	void PhysGlide(float DeltaTime, int32 Iterations);

	static FVector CalcDesiredDiffDirection(FVector FlowDirection, FVector ActorForward);

	static float CalcAngleOfAttack(FVector GivenVelocity, FVector GivenWindVelocity, FVector ActorRight, FVector ActorUp);
	static float CalcAoaSign(FVector GivenVelocity, FVector GivenWindVelocity, FVector ActorRight, FVector ActorUp);

	// Adds a given force at a given position relative to the actor. 
	void AddForceAtPos(FVector Force, FVector Position, float DeltaTime);

	void AddForceToVel(FVector Force, float DeltaTime);
	FVector CalcAccelForce(FVector GivenAcceleration, float DeltaTime);
	static FVector CalcAccelForce(FVector GivenAcceleration, float GivenMass, float DeltaTime);
	FVector CalcForceAccel(FVector Force, float DeltaTime);
	static FVector CalcForceAccel(FVector Force, float GivenMass, float DeltaTime);

	static FVector CalcTorqueFromForceAtPos(FVector Force, FVector Position);
	void AddTorqueToAngularVel(FVector Torque, float DeltaTime);
	FVector CalcAccelTorque(FVector GivenAcceleration, float DeltaTime);
	static FVector CalcAccelTorque(FVector GivenAcceleration, float GivenMomentInertia, float DeltaTime);
	FVector CalcTorqueAccel(FVector Torque, float DeltaTime);
	static FVector CalcTorqueAccel(FVector Torque, float GivenMomentInertia, float DeltaTime);

	static FVector CombineRotationVectors(TArray<FVector> Vectors);

	static float CalcLimitedSpeedScale(FVector GivenVelocity, FVector NewVelocity, float GivenMaxSpeed);

	static FString ConvertVectorArrayToString(TArray<FVector> Vectors)
	{
		FStringBuilderBase VectorString = FStringBuilderBase();
		for (int i = 0; i < Vectors.Num(); i++)
		{
			VectorString.Append(Vectors[i].ToString() + " ");
		}

		return VectorString.ToString();
	}

	UPROPERTY(EditAnywhere) bool bShouldEnableDebug;

protected:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

public:
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

private:

	enum EGlideForces
	{
		FORCES_LeftWingLift,
		FORCES_RightWingLift,
		FORCES_Drag,
		FORCES_Thrust,
		FORCES_Gravity,
		FORCES_MAX
	};
	enum EGlideTorques
	{
		TORQUES_LiftRoll,
		TORQUES_LiftPitch,
		TORQUES_DragTorque,
		TORQUES_AngularDamp,
		TORQUES_MAX
	};

	class FGlideArgs
	{
		float DeltaTime;

		TArray<bool> bWhichForcesAndTorquesEnabled;

		float Mass;

		float MomentInertia;
		bool bShouldUseEulerRotation;

		FVector Velocity;
		FVector AngularVelocity;
		FVector WindVelocity;

		FVector ActorForward;
		FVector ActorRight;
		FVector ActorUp;

		FVector Thrust;
		FVector2D AimerPercentPos;
		float AngleOfAttack;
		FVector WingDirection;

		float LiftScale;
		float CriticalAoa;

		float DragScale;
		float ParasiticDrag;
		float DragCoefficientAoaScale;

		FVector GravityDirection;
		float GravityScale;

		float MinRollForceAccel;
		float BonusLiftRollScale;

		float MinPitchForceAccel;
		float BonusLiftPitchScale;

		float DragTorqueScale;

		float AngularDampScale;


	public:
		FVector debugDragTorqueAxis;
		FVector debugDesiredRotAxis;

		FGlideArgs
		(
			float GivenDeltaTime,
			TArray<bool> bGivenWhichForcesAndTorquesEnabled,
			float GivenMass,
			float GivenMomentInertia, bool bGivenShouldUseEulerRotation,
			FVector GivenVelocity, FVector GivenAngularVelocity, FVector GivenWindVelocity,
			FVector GivenActorForward, FVector GivenActorRight, FVector GivenActorUp,
			FVector GivenThrust, FVector2D GivenAimerPercentPos, float GivenAngleOfAttack, FVector GivenWingDirection,
			float GivenLiftScale, float GivenCriticalAoa,
			float GivenDragScale, float GivenParasiticDrag, float GivenDragCoefficientAoaScale,
			FVector GivenGravityDirection, float GivenGravityScale,
			float GivenMinRollForceAccel, float GivenBonusLiftRollScale,
			float GivenMinPitchForceAccel, float GivenBonusLiftPitchScale,
			float GivenDragTorqueScale,
			float GivenAngularDampScale
		)
		{
			DeltaTime = GivenDeltaTime;

			bWhichForcesAndTorquesEnabled = bGivenWhichForcesAndTorquesEnabled;

			Mass = GivenMass;

			MomentInertia = GivenMomentInertia;
			bShouldUseEulerRotation = bGivenShouldUseEulerRotation;

			Velocity = GivenVelocity;
			AngularVelocity = GivenAngularVelocity;
			WindVelocity = GivenWindVelocity;

			ActorForward = GivenActorForward;
			ActorRight = GivenActorRight;
			ActorUp = GivenActorUp;

			Thrust = GivenThrust;
			AimerPercentPos = GivenAimerPercentPos;
			AngleOfAttack = GivenAngleOfAttack;
			WingDirection = GivenWingDirection;

			LiftScale = GivenLiftScale;
			CriticalAoa = GivenCriticalAoa;

			DragScale = GivenDragScale;
			ParasiticDrag = GivenParasiticDrag;
			DragCoefficientAoaScale = GivenDragCoefficientAoaScale;

			GravityDirection = GivenGravityDirection;
			GravityScale = GivenGravityScale;

			MinRollForceAccel = GivenMinRollForceAccel;
			BonusLiftRollScale = GivenBonusLiftRollScale;

			MinPitchForceAccel = GivenMinPitchForceAccel;
			BonusLiftPitchScale = GivenBonusLiftPitchScale;

			DragTorqueScale = GivenDragTorqueScale;

			AngularDampScale = GivenAngularDampScale;
		}

		FGlideArgs
		(
			FGlideArgs* GlideArgs
		)
		{
			DeltaTime = GlideArgs->DeltaTime;

			bWhichForcesAndTorquesEnabled = GlideArgs->bWhichForcesAndTorquesEnabled;

			Mass = GlideArgs->Mass;

			MomentInertia = GlideArgs->MomentInertia;
			bShouldUseEulerRotation = GlideArgs->bShouldUseEulerRotation;

			Velocity = GlideArgs->Velocity;
			AngularVelocity = GlideArgs->AngularVelocity;
			WindVelocity = GlideArgs->WindVelocity;

			ActorForward = GlideArgs->ActorForward;
			ActorRight = GlideArgs->ActorRight;
			ActorUp = GlideArgs->ActorUp;

			Thrust = GlideArgs->Thrust;
			AimerPercentPos = GlideArgs->AimerPercentPos;
			AngleOfAttack = GlideArgs->AngleOfAttack;
			WingDirection = GlideArgs->WingDirection;

			LiftScale = GlideArgs->LiftScale;
			CriticalAoa = GlideArgs->CriticalAoa;

			DragScale = GlideArgs->DragScale;
			ParasiticDrag = GlideArgs->ParasiticDrag;
			DragCoefficientAoaScale = GlideArgs->DragCoefficientAoaScale;

			GravityDirection = GlideArgs->GravityDirection;
			GravityScale = GlideArgs->GravityScale;

			MinRollForceAccel = GlideArgs->MinRollForceAccel;
			BonusLiftRollScale = GlideArgs->BonusLiftRollScale;

			MinPitchForceAccel = GlideArgs->MinPitchForceAccel;
			BonusLiftPitchScale = GlideArgs->BonusLiftPitchScale;

			AngularDampScale = GlideArgs->AngularDampScale;
		}

		TTuple<TArray<FVector>, TArray<FVector>, FVector, FVector> CalcAverageGlideForcesAndTorques
		(
			TArray<FVector> Forces, TArray<FVector> Torques, FVector Force, FVector Torque, float PredictionScale
		);
		
		TArray<FVector> CalcGlideForcesAndTorques();

	private:
		FVector CalcLift(float LeftOrRightLiftScale, FVector FlowVelocity);
		TArray<FVector> CalcLifts(float LeftOrRightLiftScale, FVector FlowVelocity);

		FVector CalcDrag(FVector FlowVelocity);

		FVector CalcLiftRoll(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity);

		FVector CalcLiftPitch(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity);
		
		FVector CalcDragTorque(FVector Drag, FVector LiftRoll, FVector LiftPitch, FVector FlowVelocity);

		FVector CalcAngularDamp();
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
		float saved_MaxDragDesiredScale;

		float saved_LiftScale;
		float saved_MinLiftDesiredScale;
		float saved_MaxLiftDesiredScale;
		// Gravity is not ours

		float saved_AngleOfAttack;
		float saved_SurfaceArea;
		FVector saved_WindVelocity;
		float saved_AirDensity;
		

	private:
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
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
