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
	void FlapReleased();

	void LandPressed();
	void LandReleased();

	bool bWantsToFlap;

private:
	GENERATED_BODY()

	bool bWantsToLand;
	// Minimum fraction of a second passed to perform a substep
	UPROPERTY(EditDefaultsOnly, Category = "General") float FixedDeltaTimeDivisor;
	UPROPERTY(EditDefaultsOnly, Category = "General") bool bShouldAverageForcesAndTorques;
	// How many frames averaging simulates (probably only use up to 1)
	UPROPERTY(EditDefaultsOnly, Category = "General") float PredictionScale;
	// Fraction of velocity added to position
	UPROPERTY(EditDefaultsOnly, Category = "General") float VelocityScale;
	// Fraction of angular velocity added to orientation
	UPROPERTY(EditDefaultsOnly, Category = "General") float AngularVelocityScale;
	float AccumulatedDeltaTime; // Amount of delta time passed to substep through
	float FixedDeltaTime; // Minimum amount of milliseconds passed to perform a substep
	FVector TrueVelocity;
	float TrueVelToVelMagRatio;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") TArray<bool> bWhichGlideForcesAndTorquesEnabled;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") bool bShouldLimitGlideSpeed;
	// Speed to linearly limit to whilst gliding (complicated by velocity scale)
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") float MaxGlideSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") bool bShouldLimitGlideAngularSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") float MaxGlideAngularSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding") float MaxWingAoaOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Thrust") float DefaultThrustScale;	
	float ThrustScale;
	FVector ThrustThisFrame;
	
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Gravity") float GlideGravityScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultLiftScale;
	// Delete when bothered (might reimplement, but still delete)
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float DefaultMaxLiftDesiredScale;
	// Optimal angle of attack for lift, after which lift coefficient decreases
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Lift") float CriticalAoa;
	float LiftScale;
	float MaxLiftDesiredScale;

	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultDragScale;
	// Delete when bothered (might reimplement, but still delete)
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DefaultMaxDragDesiredScale;
	// Minimum percentage of drag equation always applied, unless drag scale 0
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float ParasiticDrag;
	// Gives us the drag-AoA curve we want, should make more similar to critical AoA
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Drag") float DragCoefficientAoaScale;
	float DragScale;
	float MaxDragDesiredScale;

	// Will eventually matter and be a 3D vector, or 3x3 matrix
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MomentInertia;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") bool bShouldSnapRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") bool bShouldUseVectorRotation;


	// Categorise deeper when more params

	// The minimum lift accel that we'll input into the roll calculation. Increases the amount of roll (greater left-right lifts, greater difference, greater torque)
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinRollForceAccel;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MaxRollOppVelocityScale;

	// May implement later, more of a finishing touch
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float BonusLiftRollScale;
	// The minimum lift accel that we'll input into the roll calculation. Increases the amount of roll (greater forward-back lifts, greater difference, greater torque)
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinPitchForceAccel;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MaxPitchOppVelocityScale;
	// Minimum lift coefficient to start auto pitching towards lift direction
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float MinAutoPitchCl;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoLiftPitchScale;
	// Aligns actor forward with velocity
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoAlignScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float VelPitchOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoAlignForwardScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoAlignRightScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoAlignUpScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float AutoAlignActiveInputScale;
	UPROPERTY(EditDefaultsOnly, Category = "Balinga Movement: Gliding|Rotation") float RollStabilityScale;

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

	static float CalcAngleOfAttack(FVector GivenVelocity, FVector GivenWingDirection, FVector ActorForward, FVector ActorRight, FVector ActorUp);
	static float CalcAoaSign(FVector GivenVelocity, FVector GivenWingDirection, FVector ActorRight, FVector ActorUp);
	FVector CalcWingDirection(float AimerPercentComponent, FVector ActorForward, FVector ActorRight);

	// Adds a given force at a given position relative to the actor. 
	void AddForceAtPos(FVector Force, FVector Position, float DeltaTime);

	void AddForceToVel(FVector Force, float DeltaTime);
	FVector CalcAccelForce(FVector GivenAcceleration, float DeltaTime) const;
	static FVector CalcAccelForce(FVector GivenAcceleration, float GivenMass, float DeltaTime);
	FVector CalcForceAccel(FVector Force, float DeltaTime) const;
	static FVector CalcForceAccel(FVector Force, float GivenMass, float DeltaTime);

	static FVector CalcTorqueFromForceAtPos(FVector Force, FVector Position);
	void AddTorqueToAngularVel(FVector Torque, float DeltaTime);
	FVector CalcAccelTorque(FVector GivenAcceleration, float DeltaTime) const;
	static FVector CalcAccelTorque(FVector GivenAcceleration, float GivenMomentInertia, float DeltaTime);
	FVector CalcTorqueAccel(FVector Torque, float DeltaTime) const;
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
		TORQUES_AutoAlign,
		TORQUES_RollStability,
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
		float MaxRollOppVelocityScale;
		float BonusLiftRollScale;

		float MinPitchForceAccel;
		float MaxPitchOppVelocityScale;
		float MinAutoPitchCl;
		float AutoLiftPitchScale;

		float AutoAlignScale;
		float VelPitchOffset;
		float AutoAlignForwardScale;
		float AutoAlignRightScale;
		float AutoAlignUpScale;
		float AutoAlignActiveInputScale;

		float RollStabilityScale;

		float AngularDampScale;


	public:
		mutable float DebugDragCoefficient;
		mutable FVector DebugAutoAlignAxis;
		mutable FVector DebugDesiredRotAxis;

		mutable float DebugLiftCoefficient;

		mutable FVector DebugTrueLiftRoll;
		mutable FVector DebugTrueLiftPitch;

		mutable float DebugAutoAlignInputScale;
		mutable float DebugTrueAutoAlignInputScale;
		mutable float DebugRollStabilityInputScale;
		mutable float DebugTrueRollStabilityInputScale;

		FGlideArgs(float DeltaTime, const TArray<bool>& BWhichForcesAndTorquesEnabled, float Mass, float MomentInertia,
			bool bShouldUseEulerRotation, const FVector& Velocity, const FVector& AngularVelocity,
			const FVector& WindVelocity, const FVector& ActorForward, const FVector& ActorRight, const FVector& ActorUp,
			const FVector& Thrust, const FVector2D& AimerPercentPos, float AngleOfAttack, const FVector& WingDirection,
			float LiftScale, float CriticalAoa, float DragScale, float ParasiticDrag, float DragCoefficientAoaScale,
			const FVector& GravityDirection, float GravityScale, float MinRollForceAccel, float MaxRollOppVelocityScale,
			float BonusLiftRollScale, float MinPitchForceAccel, float MaxPitchOppVelocityScale, float MinAutoPitchCl,
			float AutoLiftPitchScale, float AutoAlignScale, float VelPitchOffset, float AutoAlignForwardScale,
			float AutoAlignRightScale, float AutoAlignUpScale, float AutoAlignActiveInputScale,
			float RollStabilityScale, float AngularDampScale)
			: DeltaTime(DeltaTime),
			  bWhichForcesAndTorquesEnabled(BWhichForcesAndTorquesEnabled),
			  Mass(Mass),
			  MomentInertia(MomentInertia),
			  bShouldUseEulerRotation(bShouldUseEulerRotation),
			  Velocity(Velocity),
			  AngularVelocity(AngularVelocity),
			  WindVelocity(WindVelocity),
			  ActorForward(ActorForward),
			  ActorRight(ActorRight),
			  ActorUp(ActorUp),
			  Thrust(Thrust),
			  AimerPercentPos(AimerPercentPos),
			  AngleOfAttack(AngleOfAttack),
			  WingDirection(WingDirection),
			  LiftScale(LiftScale),
			  CriticalAoa(CriticalAoa),
			  DragScale(DragScale),
			  ParasiticDrag(ParasiticDrag),
			  DragCoefficientAoaScale(DragCoefficientAoaScale),
			  GravityDirection(GravityDirection),
			  GravityScale(GravityScale),
			  MinRollForceAccel(MinRollForceAccel),
			  MaxRollOppVelocityScale(MaxRollOppVelocityScale),
			  BonusLiftRollScale(BonusLiftRollScale),
			  MinPitchForceAccel(MinPitchForceAccel),
			  MaxPitchOppVelocityScale(MaxPitchOppVelocityScale),
			  MinAutoPitchCl(MinAutoPitchCl),
			  AutoLiftPitchScale(AutoLiftPitchScale),
			  AutoAlignScale(AutoAlignScale),
			  VelPitchOffset(VelPitchOffset),
			  AutoAlignForwardScale(AutoAlignForwardScale),
			  AutoAlignRightScale(AutoAlignRightScale),
			  AutoAlignUpScale(AutoAlignUpScale),
			  AutoAlignActiveInputScale(AutoAlignActiveInputScale),
			  RollStabilityScale(RollStabilityScale),
			  AngularDampScale(AngularDampScale)
		{
		}


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
			float GivenAutoAlignScale, float GivenAutoAlignForwadScale, float GivenAutoAlignRightScale, float GivenAutoAlignUpScale,
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
			MinAutoPitchCl = GivenBonusLiftPitchScale;

			AutoAlignScale = GivenAutoAlignScale;
			AutoAlignForwardScale = GivenAutoAlignForwadScale;
			AutoAlignRightScale = GivenAutoAlignRightScale;
			AutoAlignUpScale = GivenAutoAlignUpScale;

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
			MinAutoPitchCl = GlideArgs->MinAutoPitchCl;

			AutoAlignScale = GlideArgs->AutoAlignScale;
			AutoAlignForwardScale = GlideArgs->AutoAlignForwardScale;
			AutoAlignRightScale = GlideArgs->AutoAlignRightScale;
			AutoAlignUpScale = GlideArgs->AutoAlignUpScale;

			RollStabilityScale = GlideArgs->RollStabilityScale;

			AngularDampScale = GlideArgs->AngularDampScale;
		}

		TTuple<TArray<FVector>, TArray<FVector>, FVector, FVector> CalcAverageGlideForcesAndTorques
		(
			TArray<FVector> Forces, TArray<FVector> Torques, FVector Force, FVector Torque, float PredictionScale
		) const;
		
		TArray<FVector> CalcGlideForcesAndTorques() const;

	private:
		FVector CalcLift(float LeftOrRightLiftScale, FVector FlowVelocity) const;
		TArray<FVector> CalcLifts(float LeftOrRightLiftScale, FVector FlowVelocity) const;

		FVector CalcDrag(FVector FlowVelocity) const;

		FVector CalcLiftRoll(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity) const;

		FVector CalcLiftPitch(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity) const;
		
		FVector CalcAutoAlign(FVector Drag, FVector Lift, FVector Gravity, FVector LiftRoll, FVector LiftPitch, FVector FlowVelocity) const;

		FVector CalcRollStability(FVector LiftRoll, FVector LiftPitch) const;

		FVector CalcAngularDamp() const;
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
