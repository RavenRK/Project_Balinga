#include "BalingaMovement.h"
#include "Logging/LogMacros.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLoggerUtils.h"
#include "MathUtils.h"
#include "UI/AimerBase.h"
#include "BalingaControllerBase.h"
#include "Developer/NaniteUtilities/Public/Affine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UBalingaMovement::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());

	FixedDeltaTime = 1 / FixedDeltaTimeDivisor;

	ThrustScale = DefaultThrustScale;

	DragScale = DefaultDragScale;
	MaxDragDesiredScale = DefaultMaxDragDesiredScale;

	AngularDampScale = DefaultAngularDampScale;

	LiftScale = DefaultLiftScale;
	MaxLiftDesiredScale = DefaultMaxLiftDesiredScale;

	SurfaceArea = DefaultSurfaceArea;
}

void UBalingaMovement::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Glide:
		PhysGlide(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode."))
	}
}

void UBalingaMovement::PostInitProperties()
{
	Super::PostInitProperties();

	bWhichGlideForcesAndTorquesEnabled.SetNum(FORCES_MAX + TORQUES_MAX);
}

void UBalingaMovement::EnterGlide()
{
	// Do this with Balinga functions and make some of these variables private
	if (!bShouldSnapRotation)
	{
		CharacterOwner->bUseControllerRotationPitch = false;
		CharacterOwner->bUseControllerRotationRoll = false;
		CharacterOwner->bUseControllerRotationYaw = false;

		BalingaOwner->SpringArm->bUsePawnControlRotation = false;
	}

	if (bShouldSnapRotation)
	{
		CharacterOwner->bUseControllerRotationPitch = true;
		CharacterOwner->bUseControllerRotationRoll = true;
		CharacterOwner->bUseControllerRotationYaw = true;
	}

	bUseControllerDesiredRotation = false;
	bOrientRotationToMovement = false;

	AngularVelocity = FVector::ZeroVector;

	SetMovementMode(MOVE_Custom, CMOVE_Glide);
}
void UBalingaMovement::ExitGlide()
{
	//CharacterOwner->bUseControllerRotationPitch = true;
	//CharacterOwner->bUseControllerRotationRoll = true;
	CharacterOwner->bUseControllerRotationYaw = true;
	BalingaOwner->SpringArm->bUsePawnControlRotation = true;


	SetMovementMode(MOVE_Falling);
}

void UBalingaMovement::FlapPressed()
{
	// Thrust is applied in the direction the character is facing
	if (bWantsToFlap)
	{
		FVector ActorForward = CharacterOwner->GetActorForwardVector();
		ThrustThisFrame = ThrustScale * ActorForward;

		// Uncomment for non-continuous thrust, also stuff in balingacontroller
		// bWantsToFlap = false;
	}
}

void UBalingaMovement::FlapReleased()
{
	// Thrust is applied in the direction the character is facing
	if (!bWantsToFlap)
	{
		ThrustThisFrame = FVector::ZeroVector;
	}
}

void UBalingaMovement::LandPressed()
{
	FVector ZeroedEulerRot = FVector(0, 0, 0);
	FVector OtherEulerRot = FVector(0, 1, 0);
	FVector EulerRot = FVector(0, 90, 0);
	FQuat QuatRot = FQuat::MakeFromEuler(CombineRotationVectors({ OtherEulerRot, EulerRot }));
	FVector ConvertedEulerRot = QuatRot.Euler();
	UE_LOG(LogTemp, Warning, TEXT("Euler rotation: [%s]"), *(EulerRot.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Other euler rotation: [%s]"), *(OtherEulerRot.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Quaternion rotation: [%s]"), *(QuatRot.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Converted euler rotation: [%s]"), *(ConvertedEulerRot.ToString()));

	ExitGlide();
}
void UBalingaMovement::LandReleased() {}

void UBalingaMovement::PhysGlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Get input (doesn't need to be called every sub-step)
	FVector2D AimerPercentPos = Cast<ABalingaControllerBase>(GetController())->GetAimerPercentPosition();
	AimerPercentPos.Y *= -1;
	AimerPercentPos.X *= -1;

	AccumulatedDeltaTime += DeltaTime;

	// FixedDeltaTime for consistent physics
	while (AccumulatedDeltaTime >= FixedDeltaTime)
	{
		// Calc force and torque

		FVector ActorRight = CharacterOwner->GetActorRightVector();
		FVector ActorForward = CharacterOwner->GetActorForwardVector();
		FVector ActorUp = CharacterOwner->GetActorUpVector();

		FVector WingDirection = CalcWingDirection(AimerPercentPos.Y, ActorForward, ActorRight);
		UE_LOG(LogTemp, Warning, TEXT("Wing direction [%s]"), *WingDirection.ToString())
		UE_LOG(LogTemp, Warning, TEXT("Actor Forward [%s]"), *ActorForward.ToString())

		float AngleOfAttack = CalcAngleOfAttack(Velocity, WingDirection, ActorForward, ActorRight, ActorUp);

		FVector GivenGravityDirection = GetGravityDirection() * -1;
		float GivenGravityScale = GetGravityZ() * GlideGravityScale;

		// Input velocity and output acceleration are scaled to decouple the influence aerodynamic forces have on acceleration from max/terminal speed
		FGlideArgs GlideArgs = 
		{
			FixedDeltaTime,
			bWhichGlideForcesAndTorquesEnabled,
			Mass,
			MomentInertia, bShouldUseVectorRotation,
			Velocity * VelocityScale, AngularVelocity * AngularVelocityScale, WindVelocity,
			ActorForward, ActorRight, ActorUp,
			ThrustThisFrame, AimerPercentPos, AngleOfAttack, WingDirection,
			LiftScale, CriticalAoa, DragScale, ParasiticDrag, DragCoefficientAoaScale,
			GivenGravityDirection, GivenGravityScale,
			MinRollForceAccel, MaxRollOppVelocityScale, BonusLiftRollScale,
			MinPitchForceAccel, MaxPitchOppVelocityScale, MinAutoPitchCl, AutoLiftPitchScale,
			AutoAlignScale, VelPitchOffset, AutoAlignForwardScale, AutoAlignRightScale, AutoAlignUpScale,
			RollStabilityScale,
			AngularDampScale
		};

		TArray<FVector> CurrentForcesAndTorques = GlideArgs.CalcGlideForcesAndTorques();

		// Extract forces and torques 

		TArray<FVector> Forces = TArray<FVector>(&CurrentForcesAndTorques.GetData()[0], FORCES_MAX);
		FVector Force = FVector::ZeroVector;
		for (int i = 0; i < FORCES_MAX; i++) { Force += Forces[i]; }

		TArray<FVector> Torques = TArray<FVector>(&CurrentForcesAndTorques.GetData()[FORCES_MAX], TORQUES_MAX);
		FVector Torque = FVector::ZeroVector;
		if (bShouldUseVectorRotation)
		{
			Torque = FVector::ZeroVector;
			for (int i = 0; i < TORQUES_MAX; i++) { Torque += Torques[i]; }
		}
		else 
		{
			FQuat CurrentTorqueQuat = FQuat::MakeFromRotationVector(CombineRotationVectors(Torques));
			Torque = CurrentTorqueQuat.ToRotationVector(); 
		}

		if (bShouldAverageForcesAndTorques)
		{
			TTuple<TArray<FVector>, TArray<FVector>, FVector, FVector> AveragedForcesAndTorquesAndForceAndTorque = 
				GlideArgs.CalcAverageGlideForcesAndTorques(Forces, Torques, Force, Torque, PredictionScale);

			Forces = AveragedForcesAndTorquesAndForceAndTorque.Get<0>();
			Torques = AveragedForcesAndTorquesAndForceAndTorque.Get<1>();
			Force = AveragedForcesAndTorquesAndForceAndTorque.Get<2>();
			Torque = AveragedForcesAndTorquesAndForceAndTorque.Get<3>();
		}

		// Apply force to velocity

		// CalcAccel and descale the applied velocity scale
		FVector Accel = CalcForceAccel(Force, FixedDeltaTime) * (1 / VelocityScale);
		FVector NewVelocity = Velocity + Accel;
		
		if (bShouldLimitGlideSpeed)
		{
			float NewSpeedToSpeedRatio = (Velocity.Size() != 0)? NewVelocity.Size() / Velocity.Size() : 1;
			float LimitedSpeedScale = NewSpeedToSpeedRatio;

			LimitedSpeedScale = CalcLimitedSpeedScale(Velocity, NewVelocity, MaxGlideSpeed);

			if (NewSpeedToSpeedRatio != 0)
			{
				NewVelocity = NewVelocity / NewSpeedToSpeedRatio * LimitedSpeedScale;
			}
		}

		Velocity = NewVelocity;

		// Apply torque to angular velocity
		FVector OldAngularVelocity = FVector::ZeroVector;

		if (!bShouldSnapRotation)
		{
			// CalcAccel and descale the applied velocity scale
			FVector AngularAccel = CalcTorqueAccel(Torque, FixedDeltaTime) * (1 / AngularVelocityScale);
			FVector NewAngularVelocity = AngularVelocity;
			OldAngularVelocity = NewAngularVelocity;
			if (bShouldUseVectorRotation)
			{
				NewAngularVelocity += AngularAccel;
			}
			else 
			{
				NewAngularVelocity = CombineRotationVectors({AngularVelocity, AngularAccel});
			}

			OldAngularVelocity = NewAngularVelocity;

			if (bShouldLimitGlideAngularSpeed)
			{
				float NewSpeedToSpeedRatio = (AngularVelocity.Size() != 0)? NewAngularVelocity.Size() / AngularVelocity.Size() : 1;
				float LimitedSpeedScale = NewSpeedToSpeedRatio;

				LimitedSpeedScale = CalcLimitedSpeedScale(AngularVelocity, NewAngularVelocity, MaxGlideAngularSpeed);
				if (NewSpeedToSpeedRatio != 0)
				{
					NewAngularVelocity = (NewAngularVelocity / NewSpeedToSpeedRatio) * LimitedSpeedScale;
				}
			}
			AngularVelocity = NewAngularVelocity;

		}

		// Prep for SafeMoveUpdatedComponent()

		bJustTeleported = false;

		FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FHitResult Hit(1.f);
		FVector AdjustedVelocity = Velocity * FixedDeltaTime;

		FQuat OldOrientation = UpdatedComponent->GetComponentRotation().Quaternion();
		FVector AdjustedAngularVelocity = AngularVelocity * FixedDeltaTime;
		FQuat NewOrientation = FQuat::MakeFromRotationVector(AdjustedAngularVelocity) * OldOrientation;

		SafeMoveUpdatedComponent(AdjustedVelocity, NewOrientation, true, Hit); // Actually moves and rotates everything

		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, FixedDeltaTime, AdjustedVelocity);
			SlideAlongSurface(AdjustedVelocity, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}

		// Velocity is not guaranteed to be what we set it as (could collide), so set it to the actual change in distance over time
		if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / FixedDeltaTime;
		}

		if (bShouldEnableDebug)
		{
			FVector Position = UpdatedComponent->GetComponentLocation() + AdjustedVelocity;
			FVector LeftLiftPosition = BalingaOwner->LeftLiftArrow->GetComponentLocation() + AdjustedVelocity;
			FVector RightLiftPosition = BalingaOwner->RightLiftArrow->GetComponentLocation() + AdjustedVelocity;
			FVector DragPosition = BalingaOwner->DragArrow->GetComponentLocation() + AdjustedVelocity;

			float VlogScale = 1;
			float CylinderRadius = 1.5;
			float ArrowHeight = 20;
			float ArrowWidth = 0.28;

			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), LeftLiftPosition, LeftLiftPosition + CalcForceAccel(Forces[FORCES_LeftWingLift], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Yellow, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), RightLiftPosition, RightLiftPosition + CalcForceAccel(Forces[FORCES_RightWingLift], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Yellow, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + CalcForceAccel(Forces[FORCES_Drag], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Red, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Red);

			// FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), Position, Position + Torque.GetSafeNormal() * 200, CylinderRadius, 15, FColor::Green, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Red);
			// FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), Position, Position + GlideArgs.DebugAutoAlignAxis * 200, CylinderRadius, 15, FColor::Orange, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Orange);
			// FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), Position, Position + GlideArgs.DebugDesiredRotAxis * 200, CylinderRadius, 15, FColor::Purple, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Purple);

			// FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + GlideArgs.DebugTrueLiftRoll * 10, CylinderRadius, 15, FColor::Blue, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Blue);
			// FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + GlideArgs.DebugTrueLiftPitch, CylinderRadius, 15, FColor::Yellow, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);

			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + AngularVelocity * 100, CylinderRadius, 15, FColor::Green, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + OldAngularVelocity * 100, CylinderRadius, 15, FColor::Blue, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);

			GEngine->AddOnScreenDebugMessage(1, 100.0f, FColor::Green, FString::Printf(TEXT("Velocity: [%s] [%s]"), *Velocity.ToCompactString(), *FString::SanitizeFloat(Velocity.Size())));
			GEngine->AddOnScreenDebugMessage(2, 100.0f, FColor::Cyan, FString::Printf(TEXT("Thrust accel: [%s]"), *CalcForceAccel(Forces[FORCES_Thrust], FixedDeltaTime).ToCompactString()));
			
			GEngine->AddOnScreenDebugMessage(3, 100.0f, FColor::Purple, FString::Printf(TEXT("Angle of Attack: [%s]"), *FString::SanitizeFloat(FMath::RadiansToDegrees(AngleOfAttack))));
			FVector DebugLiftAccel = CalcForceAccel((Forces[EGlideForces::FORCES_LeftWingLift] + Forces[FORCES_RightWingLift]), FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(5, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift accel: [%s] [%s] Cl: [%s]"), *DebugLiftAccel.ToCompactString(), *FString::SanitizeFloat(DebugLiftAccel.Size()), *FString::SanitizeFloat(GlideArgs.DebugLiftCoefficient)));
			FVector DebugDragAccel = CalcForceAccel(Forces[FORCES_Drag], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(7, 100.0f, FColor::Red, FString::Printf(TEXT("Drag accel: [%s] [%s] Cd: [%s]"), *DebugDragAccel.ToCompactString(), *FString::SanitizeFloat(DebugDragAccel.Size()), *FString::SanitizeFloat(GlideArgs.DebugDragCoefficient)));

			//GEngine->AddOnScreenDebugMessage(12, 100.0f, FColor::Purple, FString::Printf(TEXT("Desired difference: [%s]"), *(FString::SanitizeFloat(DesiredDifference.X) + ", " + FString::SanitizeFloat(DesiredDifference.Y) + ", " + FString::SanitizeFloat(DesiredDifference.Z))));
			//GEngine->AddOnScreenDebugMessage(13, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift desired scale: [%s]"), *FString::SanitizeFloat(LiftDesiredScale)));
			//GEngine->AddOnScreenDebugMessage(7, 100.0f, FColor::Red, FString::Printf(TEXT("FORCES_Drag desired scale: [%s]"), *FString::SanitizeFloat(DragDesiredScale)));

			//FDebugDrawer::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", Position, Position + AngularVelocity, CylinderRadius, FColor::Green, "Angular velocity" + AngularVelocity.ToCompactString(), ArrowHeight, ArrowWidth, FColor::Green, "");

			//GEngine->AddOnScreenDebugMessage(6, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift torque projection scale: [%s]"), *FString::SanitizeFloat(LiftTorqueProjectionScale)));
			GEngine->AddOnScreenDebugMessage(9, 100.0f, FColor::Emerald, FString::Printf(TEXT("Angular velocity: [%s] [%s]"), *AngularVelocity.ToString(), *FString::SanitizeFloat(AngularVelocity.Size())));
			// GEngine->AddOnScreenDebugMessage(10, 100.0f, FColor::Green, FString::Printf(TEXT("Angular accel: [%s]"), *CalcTorqueAccel(Torque, FixedDeltaTime).ToString()));
			// GEngine->AddOnScreenDebugMessage(11, 100.0f, FColor::Silver, FString::Printf(TEXT("Aimer position: [%s]"), *AimerPosition.ToString()));
			FVector LiftRollAccel = CalcTorqueAccel(Torques[TORQUES_LiftRoll], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(12, 100.0f, FColor::Blue, FString::Printf(TEXT("Wing roll: [%s] [%s]"), *LiftRollAccel.ToString(), *FString::SanitizeFloat(LiftRollAccel.Size())));
			FVector LiftPitchAccel = CalcTorqueAccel(Torques[TORQUES_LiftPitch], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(13, 100.0f, FColor::Yellow, FString::Printf(TEXT("Wing pitch: [%s] [%s]"), *CalcTorqueAccel(Torques[TORQUES_LiftPitch], FixedDeltaTime).ToString(), *FString::SanitizeFloat(LiftPitchAccel.Size())));
			FVector AutoAlignAccel = CalcTorqueAccel(Torques[TORQUES_AutoAlign], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(14, 100.0f, FColor::Magenta, FString::Printf(TEXT("Auto align: [%s] [%s] [%s] [%s]"), *AutoAlignAccel.ToString(), *FString::SanitizeFloat(AutoAlignAccel.Size()), *FString::SanitizeFloat(GlideArgs.DebugTrueAutoAlignInputScale), *FString::SanitizeFloat(GlideArgs.DebugAutoAlignInputScale)));
			FVector RollStabilityAccel = CalcTorqueAccel(Torques[TORQUES_RollStability], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(15, 100.0f, FColor::Turquoise, FString::Printf(TEXT("Roll stability: [%s] [%s] [%s] [%s]"), *RollStabilityAccel.ToString(), *FString::SanitizeFloat(RollStabilityAccel.Size()), *FString::SanitizeFloat(GlideArgs.DebugTrueRollStabilityInputScale), *FString::SanitizeFloat(GlideArgs.DebugRollStabilityInputScale)));

			// GEngine->AddOnScreenDebugMessage(14, 100.0f, FColor::Red, FString::Printf(TEXT("Drag torque: [%s]"), *CalcTorqueAccel(Torques[TORQUES_AutoAlign], FixedDeltaTime).ToString()));
			FVector AngularDampAccel = CalcTorqueAccel(Torques[TORQUES_AngularDamp], FixedDeltaTime);
			GEngine->AddOnScreenDebugMessage(16, 100.0f, FColor::Orange, FString::Printf(TEXT("Angular damp: [%s] [%s]"), *AngularDampAccel.ToString(), *FString::SanitizeFloat(AngularDampAccel.Size())));

			// GEngine->AddOnScreenDebugMessage(17, 100.0f, FColor::Turquoise, FString::Printf(TEXT("True Velocity: [%s] [%s]"), *TrueVelocity.ToCompactString(), *FString::SanitizeFloat(TrueVelocity.Size())));
			// GEngine->AddOnScreenDebugMessage(18, 100.0f, FColor::Cyan, FString::Printf(TEXT("True Velocity to Velocity Ratio: [%s]"), *FString::SanitizeFloat(TrueVelToVelMagRatio)));
		}

		// Will add more when we fully do non-jittery deltatime
		AccumulatedDeltaTime -= FixedDeltaTime;
		Iterations++; // Probably use this like Unreal does (MaxIterationAmount)
	}
}

TTuple<TArray<FVector>, TArray<FVector>, FVector, FVector> UBalingaMovement::FGlideArgs::CalcAverageGlideForcesAndTorques
(
	TArray<FVector> Forces, TArray<FVector> Torques, FVector Force, FVector Torque,
	float PredictionScale
) const
{
	FVector PredictedVelocity = Velocity + CalcForceAccel(Force, Mass, DeltaTime * PredictionScale);
	FVector PredictedAngularVelocity = FVector::ZeroVector;
	if (bShouldUseEulerRotation)
	{
		PredictedAngularVelocity = AngularVelocity + CalcTorqueAccel(Torque, MomentInertia, DeltaTime * PredictionScale);
	}
	else
	{
		PredictedAngularVelocity = CombineRotationVectors({ CalcTorqueAccel(Torque, MomentInertia, DeltaTime * PredictionScale), AngularVelocity});
	}

	FGlideArgs PredictedArgs = FGlideArgs(*this);
	PredictedArgs.Velocity = PredictedVelocity;
	PredictedArgs.AngularVelocity = PredictedAngularVelocity;
	PredictedArgs.DeltaTime *= PredictionScale;

	TArray<FVector> PredictedForcesAndTorques = PredictedArgs.CalcGlideForcesAndTorques();

	TArray<FVector> PredictedForces = { &PredictedForcesAndTorques.GetData()[0], FORCES_MAX };
	TArray<FVector> PredictedTorques = { &PredictedForcesAndTorques.GetData()[FORCES_MAX], TORQUES_MAX };

	// Averaged forces feel smoother, but harder to debug (can't average AoA and some other stuff)
	TArray<FVector> AverageForces;
	AverageForces.Init(FVector::ZeroVector, FORCES_MAX);
	for (int i = 0; i < FORCES_MAX; i++) { AverageForces[i] = (Forces[i] + PredictedForces[i]) / 2; }
	FVector AverageForce = FVector::ZeroVector;
	for (int i = 0; i < FORCES_MAX; i++) { AverageForce += AverageForces[i]; }	

	TArray<FVector> AverageTorques;
	AverageTorques.Init(FVector::ZeroVector, TORQUES_MAX);
	for (int i = 0; i < TORQUES_MAX; i++) { AverageTorques[i] = (Torques[i] + PredictedTorques[i]) / 2; }
	FVector AverageTorque = FVector::ZeroVector;
	if (bShouldUseEulerRotation)
	{
		for (int i = 0; i < TORQUES_MAX; i++) { AverageTorque += AverageTorques[i]; }

	}
	else 
	{
		FQuat AverageTorqueQuat = FQuat::MakeFromRotationVector(CombineRotationVectors(AverageTorques));
		AverageTorque = AverageTorqueQuat.ToRotationVector();
	}

	return TTuple<TArray<FVector>, TArray<FVector>, FVector, FVector>(AverageForces, AverageTorques, AverageForce, AverageTorque);
}


TArray<FVector> UBalingaMovement::FGlideArgs::CalcGlideForcesAndTorques() const
{
	TArray<FVector> ForcesAndTorques;
	ForcesAndTorques.Init(FVector::ZeroVector, FORCES_MAX + TORQUES_MAX);

	// Calc forces and torques that don't need args

	// Gravity is independent of mass
	if (bWhichForcesAndTorquesEnabled[FORCES_Gravity])
	{
		ForcesAndTorques[FORCES_Gravity] = GravityDirection * GravityScale * Mass;
	} 

	// Thrust is applied instantly, not over time
	if (bWhichForcesAndTorquesEnabled[FORCES_Thrust])
	{
		ForcesAndTorques[FORCES_Thrust] = Thrust / DeltaTime;
	} 
	
	// Prep method args

	FVector FlowVelocity = Velocity + WindVelocity;
	FVector FlowDirection = FlowVelocity.GetSafeNormal();

	FVector DesiredDifference;
	FVector DesiredDiffDirection = CalcDesiredDiffDirection(FlowDirection, ActorForward);

	// Calc forces and torques

	TArray<FVector> WingLifts = CalcLifts(AimerPercentPos.X, FlowVelocity);

	if (bWhichForcesAndTorquesEnabled[FORCES_LeftWingLift]) { ForcesAndTorques[FORCES_LeftWingLift] = WingLifts[0]; }
	if (bWhichForcesAndTorquesEnabled[FORCES_RightWingLift]) { ForcesAndTorques[FORCES_RightWingLift] = WingLifts[1]; }

	// Have drag take current forces into account
	FlowVelocity += CalcForceAccel
	(
		(ForcesAndTorques[FORCES_LeftWingLift] + ForcesAndTorques[FORCES_RightWingLift] + ForcesAndTorques[FORCES_Thrust] + ForcesAndTorques[FORCES_Gravity]),
		Mass, DeltaTime
	);
	if (bWhichForcesAndTorquesEnabled[FORCES_Drag]) { ForcesAndTorques[FORCES_Drag] = CalcDrag(FlowVelocity); }

	if (bWhichForcesAndTorquesEnabled[FORCES_MAX + TORQUES_LiftRoll])
	{
		ForcesAndTorques[FORCES_MAX + TORQUES_LiftRoll] = CalcLiftRoll(AimerPercentPos.X, WingLifts, FlowVelocity);
	}

	if (bWhichForcesAndTorquesEnabled[FORCES_MAX + TORQUES_LiftPitch])
	{
		ForcesAndTorques[FORCES_MAX + TORQUES_LiftPitch] = CalcLiftPitch(AimerPercentPos.Y, WingLifts, FlowVelocity);
	}

	if (bWhichForcesAndTorquesEnabled[FORCES_MAX + TORQUES_AutoAlign])
	{
		// Self-corrects bird to face the position they're moving in
		// Probably do something like desired difference scaling to see if it makes it more helpful
		ForcesAndTorques[FORCES_MAX + TORQUES_AutoAlign] = CalcAutoAlign(ForcesAndTorques[FORCES_Drag], ForcesAndTorques[FORCES_MAX + TORQUES_LiftRoll], ForcesAndTorques[FORCES_MAX + TORQUES_LiftPitch], FlowVelocity);
	}

	if (bWhichForcesAndTorquesEnabled[FORCES_MAX + TORQUES_RollStability])
	{
		ForcesAndTorques[FORCES_MAX + TORQUES_RollStability] = CalcRollStability(ForcesAndTorques[FORCES_MAX + TORQUES_LiftRoll], ForcesAndTorques[FORCES_MAX + TORQUES_LiftPitch]);
	}
	
	// Have angular damp take current torques into account
	FVector AngularVelocityCopy = AngularVelocity;
	if (bShouldUseEulerRotation)
	{
		AngularVelocityCopy += CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_LiftRoll], MomentInertia, DeltaTime);
		AngularVelocityCopy += CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_LiftPitch], MomentInertia, DeltaTime);
		AngularVelocityCopy += CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_AutoAlign], MomentInertia, DeltaTime);
		AngularVelocityCopy += CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_RollStability], MomentInertia, DeltaTime);
	}
	else
	{
		AngularVelocityCopy = CombineRotationVectors({CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_LiftRoll], MomentInertia, DeltaTime), AngularVelocityCopy});
		AngularVelocityCopy = CombineRotationVectors({CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_LiftPitch], MomentInertia, DeltaTime), AngularVelocityCopy});
		AngularVelocityCopy = CombineRotationVectors({ CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_AutoAlign], MomentInertia, DeltaTime), AngularVelocityCopy });
		AngularVelocityCopy = CombineRotationVectors({ CalcTorqueAccel(ForcesAndTorques[FORCES_MAX + TORQUES_RollStability], MomentInertia, DeltaTime), AngularVelocityCopy });
	}
	if (bWhichForcesAndTorquesEnabled[FORCES_MAX + TORQUES_AngularDamp])
	{
		ForcesAndTorques[FORCES_MAX + TORQUES_AngularDamp] = CalcAngularDamp();
	}

	return ForcesAndTorques;
}

FVector UBalingaMovement::FGlideArgs::CalcLift(float LeftOrRightLiftScale, FVector FlowVelocity) const
{
	TArray<FVector> WingLifts = CalcLifts(LeftOrRightLiftScale, FlowVelocity);

	return WingLifts[0] + WingLifts[1];
}
TArray<FVector> UBalingaMovement::FGlideArgs::CalcLifts(float LeftOrRightLiftScale, FVector FlowVelocity) const
{
	// Don't count velocity coming from the right direction
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(ActorRight);
	float VelocityDiff = FlowVelocity.Size() - RightIndependentVelocity.Size();
	//GEngine->AddOnScreenDebugMessage(12, 100.0f, FColor::Black, FString::Printf(TEXT("Lift velocity difference: [%s]"), *FString::SanitizeFloat(VelocityDiff)));

	FVector LiftDirection = FVector::CrossProduct(RightIndependentVelocity.GetSafeNormal(), ActorRight);
	FVector Lift = FMath::Square(FlowVelocity.Size()) * LiftDirection * LiftScale * 0.5;

	// Clamping AoA gives us the lift curve we want
	float MaxStallAoa = CriticalAoa * 2;
	float ClampedAoa;
	if (FMath::Abs(AngleOfAttack) > FMath::DegreesToRadians(MaxStallAoa))
	{
		ClampedAoa = FMath::DegreesToRadians(MaxStallAoa) * FMath::Sign(AngleOfAttack);
	}
	else
	{
		ClampedAoa = AngleOfAttack;
	}

	float LiftCoefficient = FMath::Sin(ClampedAoa * (180/MaxStallAoa));
	Lift *= LiftCoefficient;

	FVector LiftAccel = CalcForceAccel(Lift, Mass, DeltaTime);
	
	if (Lift.Size() != 0) // Prevents NaN from zero divisor
	{
		// Check if the next Aoa sign is different to this one, if so overshoot the Aoa less or not at all
		float AssumedAoaSign = CalcAoaSign(FlowVelocity + LiftAccel, WingDirection, ActorRight, ActorUp);
	
		if (AssumedAoaSign != FMath::Sign(AngleOfAttack))
		{
			LiftAccel = CalcForceAccel(Lift / LiftCoefficient, Mass, DeltaTime);
	
			FVector AoaDifference = (WingDirection - RightIndependentVelocity.GetSafeNormal());
			float AoaLiftProjectionScale = (AoaDifference.Dot(LiftAccel) / LiftAccel.Size());
			LiftCoefficient = FMath::Abs(LiftCoefficient * AoaLiftProjectionScale) * FMath::Sign(AngleOfAttack);
			LiftAccel *= LiftCoefficient;
		}
	}

	DebugLiftCoefficient = LiftCoefficient;
	// Distribute lift to allow for torque

	Lift = CalcAccelForce(LiftAccel, Mass, DeltaTime);
	FVector LeftWingLift = Lift / 2;
	FVector RightWingLift = Lift / 2;

	if (FMath::Sign(LeftOrRightLiftScale) == 1 && LeftOrRightLiftScale != 0)
	{
		RightWingLift *= 1 - LeftOrRightLiftScale;
	}
	else if (LeftOrRightLiftScale != 0)
	{
		LeftWingLift *= 1 - LeftOrRightLiftScale;
	}

	Lift = LeftWingLift + RightWingLift;

	// Lift can be greater than velocity IRL (accel needs to be bigger than vel, to even start moving), 
	// but at high speeds that would normally break the wing of the bird/plane
	LiftAccel = CalcForceAccel(Lift, Mass, DeltaTime);
	if (LiftAccel.Size() > FlowVelocity.Size())
	{
		float LiftClampScale = (FlowVelocity.Size() / LiftAccel.Size());
		LeftWingLift *= LiftClampScale;
		RightWingLift *= LiftClampScale;
	}

	return TArray<FVector>{LeftWingLift, RightWingLift};
}

FVector UBalingaMovement::FGlideArgs::CalcDrag(FVector FlowVelocity) const
{
	FVector FlowDirection = FlowVelocity.GetSafeNormal();
	FVector Drag = FMath::Square(FlowVelocity.Size()) * FlowDirection * 0.5 * -1.0 * DragScale;

	float DragCoefficient = (1 - FMath::Cos(AngleOfAttack * DragCoefficientAoaScale)) + ParasiticDrag;
	Drag *= DragCoefficient;

	DebugDragCoefficient = DragCoefficient;

	float VelocitySize = FlowVelocity.Size();
	float DragAccelSize = CalcForceAccel(Drag, Mass, DeltaTime).Size();
	
	if (DragAccelSize > VelocitySize)
	{
		float DragClampScale = (VelocitySize / DragAccelSize);
 		Drag *= DragClampScale;
	}

	return Drag;
}

FVector UBalingaMovement::FGlideArgs::CalcLiftRoll(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity) const
{
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(ActorRight); // Forgot the exact effect of this, understand and test later
	FVector LiftDirection = FVector::CrossProduct(RightIndependentVelocity.GetSafeNormal(), ActorRight).GetSafeNormal().GetAbs();
	// This will mean if we add mass to an object, rotation wont care (not exactly what i want, figure out later)
	FVector MinRollForce = CalcAccelForce((LiftDirection * MinRollForceAccel) * DeltaTime, Mass, DeltaTime);

	// Add a BonusLiftRollScale * LiftSize amount of magnitude to MinRollForce's magnitude
	// Account for zeroed MinRollForce, in which case it's replaced with the lift direction (mag of 1)
	//float LiftSize = FMath::Max(WingLifts[0].Size(), WingLifts[1].Size());
	//float LiftToMinRollForceRatio = LiftSize / FMath::Max(MinRollForce.Size(), 1);
	//FVector RollForce = FVector::Max(MinRollForce, LiftDirection) * 1 + (LiftToMinRollForceRatio * BonusLiftRollScale);
	TArray<FVector> RollForces = {MinRollForce, MinRollForce};

	if (FMath::Sign(AimerPercentComponent) == 1 && AimerPercentComponent != 0)
	{
		RollForces[0] *= 1 - FMath::Abs(AimerPercentComponent);
	}
	else if (AimerPercentComponent != 0)
	{
		RollForces[1] *= 1 - FMath::Abs(AimerPercentComponent);
	}

	TArray<FVector> WingLiftTorques =	
	{
		CalcTorqueFromForceAtPos(RollForces[0], ActorRight * -1),
		CalcTorqueFromForceAtPos(RollForces[1], ActorRight)
	};

	// The closer the torque is to the forward axis, the stronger it'll be 
	// float LiftRollProjectionScale = FVector::DotProduct((WingLiftTorques[0] + WingLiftTorques[1]).GetSafeNormal(), ActorForward);
	// float LiftYawProjectionScale = FVector::DotProduct((WingLiftTorques[0] + WingLiftTorques[1]).GetSafeNormal(), ActorUp);
	// LiftRollProjectionScale = (FMath::IsNaN(LiftRollProjectionScale)) ? 0 : LiftRollProjectionScale;
	// LiftYawProjectionScale = (FMath::IsNaN(LiftYawProjectionScale)) ? 0 : LiftYawProjectionScale;
	float LiftTorqueProjectionScale = 1 * FMath::Sign(AimerPercentComponent);
	DebugTrueLiftRoll = WingLiftTorques[0] + WingLiftTorques[1];
	// FVector FinalLiftRoll = ActorForward * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftRollProjectionScale;
	// FVector FinalLiftYaw = ActorUp * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftYawProjectionScale;
	FVector FinalLiftTorque = ActorForward * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftTorqueProjectionScale;
	// FVector FinalLiftTorque = FinalLiftRoll + FinalLiftYaw;

	float RollToOppVelocityDot = FinalLiftTorque.GetSafeNormal().Dot(FlowVelocity.GetSafeNormal() * -1);
	RollToOppVelocityDot = (RollToOppVelocityDot == -1) ? 0 : RollToOppVelocityDot;
	float RollOppVelocityScale = FMath::Max(RollToOppVelocityDot * MaxRollOppVelocityScale, 1);
	FinalLiftTorque *= RollOppVelocityScale;

	return FinalLiftTorque;
}

FVector UBalingaMovement::FGlideArgs::CalcLiftPitch(float AimerPercentComponent, TArray<FVector> WingLifts, FVector FlowVelocity) const
{
	//FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(ActorRight);
	FVector LiftDirection = FVector::CrossProduct(FlowVelocity.GetSafeNormal(), ActorRight).GetSafeNormal().GetAbs();
	// This will mean if we add mass to an object, rotation wont care (not exactly what i want, figure out later)
	FVector MinPitchForce = CalcAccelForce((LiftDirection * MinPitchForceAccel) * DeltaTime, Mass, DeltaTime); 

	// Add a BonusLiftPitchScale * LiftSize amount of magnitude to MinPitchForce's magnitude
	// Account for zeroed MinPitchForce, in which case it's replaced with the lift direction (mag of 1)
	//float LiftSize = FMath::Max(WingLifts[0].Size(), WingLifts[1].Size());
	//float LiftToMinPitchForceRatio = LiftSize / FMath::Max(MinPitchForce.Size(), 1);
	//FVector PitchForce = FVector::Max(MinPitchForce, LiftDirection) * 1 + (LiftToMinPitchForceRatio * BonusLiftPitchScale);
	TArray<FVector> PitchForces = { MinPitchForce, MinPitchForce };

	// Lift magnitude wouldn't match calcliftroll or calclifts' lift magnitude without this
	//if (FMath::Sign(AimerPercentComponent) == 1 && AimerPercentComponent != 0)
	//{
	//	PitchForces[0] *= 1 - FMath::Abs(AimerPercentComponent);
	//}
	//else if (AimerPercentComponent != 0)
	//{
	//	PitchForces[1] *= 1 - FMath::Abs(AimerPercentComponent);
	//}

	FVector FullPitchForce = PitchForces[0] + PitchForces[1];

	FVector Lift = (WingLifts[0] + WingLifts[1]);
	FVector FrontLift;
	FVector BackLift;

	if (FMath::Abs(AngleOfAttack) >= MinAutoPitchCl)
	{
		FrontLift = (MinPitchForce * FMath::Abs(DebugLiftCoefficient) * AutoLiftPitchScale);
		BackLift = (MinPitchForce * 0.1 * AutoLiftPitchScale);
	}
	else
	{
		return FVector::ZeroVector;
	}

	TArray<FVector> WingLiftTorques =
	{
		CalcTorqueFromForceAtPos(BackLift, ActorForward * -1),
		CalcTorqueFromForceAtPos(FrontLift, ActorForward)
	};

	// The closer the torque is to the forward axis, the stronger it'll be 
	//float LiftTorqueProjectionScale = FVector::DotProduct(WingLiftTorques[0] + WingLiftTorques[1], ActorForward) / (WingLiftTorques[0] + WingLiftTorques[1]).Size();
	//LiftTorqueProjectionScale = (FMath::IsNaN(LiftTorqueProjectionScale)) ? 0 : LiftTorqueProjectionScale;
	float LiftTorqueProjectionScale = 1 * FMath::Sign(AimerPercentComponent);
	DebugTrueLiftPitch = WingLiftTorques[0] + WingLiftTorques[1];

	FVector FinalLiftTorque = ActorRight * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftTorqueProjectionScale;
	// FVector FinalLiftTorque = (WingLiftTorques[0] + WingLiftTorques[1]).GetAbs() * LiftTorqueProjectionScale;

	float PitchToOppVelocityDot = FinalLiftTorque.GetSafeNormal().Dot(FlowVelocity.GetSafeNormal() * -1);
	PitchToOppVelocityDot = (PitchToOppVelocityDot == -1) ? 0 : PitchToOppVelocityDot;
	float PitchOppVelocityScale = FMath::Max(PitchToOppVelocityDot * MaxPitchOppVelocityScale, 1);
	FinalLiftTorque *= PitchOppVelocityScale;

	return FinalLiftTorque;
}

FVector UBalingaMovement::FGlideArgs::CalcAutoAlign(FVector Drag, FVector LiftRoll, FVector LiftPitch, FVector FlowVelocity) const
{
	// Torque produced by drag generated ahead (not sure why it's not behind) of balinga's COG
	// rotates around the same axis as the rotation between velocity and balinga's forward vector

	FVector OriToAlignTo = FQuat::MakeFromRotationVector(ActorRight * VelPitchOffset).RotateVector(FlowVelocity.GetSafeNormal());
	FVector AlignToVelRot = FQuat::FindBetweenVectors(ActorForward, OriToAlignTo).ToRotationVector();
	FVector AlignToVelAxis = AlignToVelRot.GetSafeNormal();
	DebugAutoAlignAxis = AlignToVelAxis;

	FVector InputRot = CalcTorqueAccel((LiftRoll + LiftPitch), MomentInertia, DeltaTime);
	FVector InputAxis = InputRot.GetSafeNormal();
	DebugDesiredRotAxis = InputAxis;


	// Scale alignment by similarity to input so the player doesn't feel like they have to fight the auto-align for control
	float InputScale = AlignToVelAxis.Dot(InputAxis);

	DebugTrueAutoAlignInputScale = InputScale;

	if (!(InputRot.Size() < 0.001)) // Disregard input scale if too small
	{
		// Don't reverse axis (rotation direction)
		if (FMath::Sign(InputScale) == -1)
		{
			if (FMath::Abs(InputScale) < 0.5) // Allow for small opposites
			{
				InputScale = 1;
			}
			else 
			{
				InputScale = 0;
			}
		}
		else if (FMath::Sign(InputScale) == 1) // Only scale down more than right angle away
		{
			InputScale = 1;
		}
	}
	else
	{
		InputScale = 1;
	}

	DebugAutoAlignInputScale = InputScale;

	// InputScale = (FMath::Sign(InputScale) == -1 && InputScale <= 0.5) ? InputScale * -1 : InputScale; // Don't reverse axis (rotation direction)
	// InputScale = (FMath::Sign(InputScale) == 1) ? 1 : InputScale; // Only scale down more than right angle away
	// InputScale = (InputRot.Size() < 0.001) ? 1 : InputScale; // Disregard input scale if too small
	AlignToVelRot *= InputScale * AutoAlignScale * AutoAlignActiveInputScale * AimerPercentPos.Y;

	FVector ForwardPart = AlignToVelRot.ProjectOnTo(ActorForward) * AutoAlignForwardScale;
	FVector RightPart = AlignToVelRot.ProjectOnTo(ActorRight) * AutoAlignRightScale;
	FVector UpPart = AlignToVelRot.ProjectOnTo(ActorUp) * AutoAlignUpScale;

	FVector OldAlignToVelRot = AlignToVelRot;
	AlignToVelRot = (ForwardPart + RightPart + UpPart).GetSafeNormal() * OldAlignToVelRot.Size();

	return AlignToVelRot;
}

FVector UBalingaMovement::FGlideArgs::CalcRollStability(FVector LiftRoll, FVector LiftPitch) const
{
	FVector StableRight = GravityDirection.Cross(ActorForward);
	FVector StableRightOpp = (GravityDirection * -1).Cross(ActorForward);

	FVector RotToStable = FQuat::FindBetweenVectors(ActorRight, StableRight).ToRotationVector();
	FVector RotToStableOpp = FQuat::FindBetweenVectors(ActorRight, StableRightOpp).ToRotationVector();

	if (RotToStableOpp.Size() < RotToStable.Size())
	{
		RotToStable = RotToStableOpp;
	}

	FVector ForwardPart = RotToStable.ProjectOnTo(ActorForward);

	FVector InputRot = CalcTorqueAccel((LiftRoll + LiftPitch), MomentInertia, DeltaTime);
	FVector InputAxis = InputRot.GetSafeNormal();
	// Scale alignment by similarity to input so the player doesn't feel like they have to fight the auto-align for control
	float InputScale = ForwardPart.Dot(InputAxis);
	DebugTrueRollStabilityInputScale = InputScale;

	InputScale = (FMath::Sign(InputScale) == -1) ? 0 : InputScale; // Don't reverse axis (rotation direction)
	InputScale = (FMath::Sign(InputRot.Size() < 0.001)) ? 1 : InputScale; // Disregard input scale if too small
	DebugRollStabilityInputScale = InputScale;

	ForwardPart *= InputScale * RollStabilityScale;

	return ForwardPart;
}

FVector UBalingaMovement::FGlideArgs::CalcAngularDamp() const
{
	// We lose a AngularDampScale to the power of DeltaTime fraction of speed every time step
	// We do this instead of multiplying by DT. Why? I forgot
	FVector AngularDamp = -1 * (AngularVelocity - AngularVelocity * FMath::Pow(AngularDampScale, DeltaTime)) / DeltaTime;

	// Just in case DeltaTime gets too large (this func doesn't automatically use fixedDT, it shouldn't),
	// damping should not be able to change angular vel dir past 0
	float AngularVelocitySize = AngularVelocity.Size();
	float AngularDampAccelSize = CalcTorqueAccel(AngularDamp, MomentInertia, DeltaTime).Size();
	if (AngularDampAccelSize > AngularVelocitySize)
	{
		AngularDamp *= (AngularVelocitySize / AngularDampAccelSize);
	}
	
	return AngularDamp;
}


FVector UBalingaMovement::CalcDesiredDiffDirection(FVector FlowDirection, FVector ActorForward)
{
	FVector DesiredDifference = ActorForward - FlowDirection;
	FVector DesiredDifferenceDirection = DesiredDifference.GetSafeNormal();

	// DON'T NEED TO SMOOTH ANYTHING YET (eventually smooth high speed changes since they feel snappy)
	/*std::tuple<FVector, FVector> data = FMathUtils::SmoothDamp3(LastDesiredDifference, DesiredDiffDirection, SmoothVelocity, SmoothTime);
	LastDesiredDifference = DesiredDiffDirection;
	DesiredDiffDirection = std::get<0>(data);
	SmoothVelocity = std::get<1>(data);*/

	return DesiredDifferenceDirection;
}

float UBalingaMovement::CalcAngleOfAttack(FVector GivenVelocity, FVector GivenWingDirection, FVector ActorForward, FVector ActorRight, FVector ActorUp)
{
	FVector RightIndependentVelocity = GivenVelocity - GivenVelocity.ProjectOnTo(ActorRight); // Essentially make velocity 2D

	float AoaSign = CalcAoaSign(RightIndependentVelocity, GivenWingDirection, ActorRight, ActorUp);
	float AoaDot = FMath::Abs(GivenWingDirection.Dot(RightIndependentVelocity.GetSafeNormal()));
	return (FMath::Acos(AoaDot) * AoaSign);
}
float UBalingaMovement::CalcAoaSign(FVector GivenVelocity, FVector GivenWingDirection, FVector ActorRight, FVector ActorUp)
{
	FVector RightIndependentVelocity = GivenVelocity - GivenVelocity.ProjectOnTo(ActorRight); // Essentially make velocity 2D

	FVector AoaDifference = (GivenWingDirection - RightIndependentVelocity.GetSafeNormal());
	FVector AoaDifferenceDirection = AoaDifference.GetSafeNormal();
	float AoaSign = FMath::Sign(AoaDifferenceDirection.Dot(ActorUp));

	return AoaSign;
}

FVector UBalingaMovement::CalcWingDirection(float AimerPercentComponent, FVector ActorForward, FVector ActorRight)
{
	// Wing direction only changes the pitch
	return FQuat::MakeFromRotationVector(ActorRight * FMath::DegreesToRadians(MaxWingAoaOffset) * AimerPercentComponent).RotateVector(ActorForward);
}

void UBalingaMovement::AddForceAtPos(FVector Force, FVector Position, float DeltaTime)
{
	AddForceToVel(Force, DeltaTime);

	AddTorqueToAngularVel(CalcTorqueFromForceAtPos(Force, Position), DeltaTime);
}

void UBalingaMovement::AddForceToVel(FVector Force, float DeltaTime)
{
	Velocity += CalcForceAccel(Force, DeltaTime);
}
FVector UBalingaMovement::CalcAccelForce(FVector GivenAcceleration, float DeltaTime) const
{
	return GivenAcceleration / DeltaTime * Mass;
}
FVector UBalingaMovement::CalcAccelForce(FVector GivenAcceleration, float GivenMass, float DeltaTime)
{
	return GivenAcceleration / DeltaTime * GivenMass;
}
FVector UBalingaMovement::CalcForceAccel(FVector Force, float DeltaTime) const
{
	return CalcForceAccel(Force, Mass, DeltaTime);
}
FVector UBalingaMovement::CalcForceAccel(FVector Force, float GivenMass, float DeltaTime)
{
	return Force / GivenMass * DeltaTime;
}

void UBalingaMovement::AddTorqueToAngularVel(FVector Torque, float DeltaTime)
{
	AngularVelocity += CalcTorqueAccel(Torque, DeltaTime);
}
FVector UBalingaMovement::CalcTorqueFromForceAtPos(FVector Force, FVector Position)
{	
	return Force.Cross(Position);
}
FVector UBalingaMovement::CalcAccelTorque(FVector GivenAcceleration, float DeltaTime) const
{
	return CalcAccelTorque(GivenAcceleration, MomentInertia, DeltaTime);
}
FVector UBalingaMovement::CalcAccelTorque(FVector GivenAcceleration, float GivenMomentInertia, float DeltaTime)
{
	return GivenAcceleration / DeltaTime * GivenMomentInertia;
}
FVector UBalingaMovement::CalcTorqueAccel(FVector Torque, float DeltaTime) const
{
	return CalcTorqueAccel(Torque, MomentInertia, DeltaTime);
}
FVector UBalingaMovement::CalcTorqueAccel(FVector Torque, float GivenMomentInertia, float DeltaTime)
{
	return Torque / GivenMomentInertia * DeltaTime;
}

FVector UBalingaMovement::CombineRotationVectors(TArray<FVector> Vectors)
{
	FQuat CombinedQuat = FQuat::MakeFromRotationVector(FVector::ZeroVector); // Still makes a unit quat, but all 3 axes are zeroed

	for (int i = 0; i < Vectors.Num(); i++)
	{
		CombinedQuat = FQuat::MakeFromRotationVector(Vectors[i]) * CombinedQuat;
	}
	
	return CombinedQuat.GetNormalized().ToRotationVector();
}

float UBalingaMovement::CalcLimitedSpeedScale(FVector GivenVelocity, FVector NewVelocity, float GivenMaxSpeed)
{
	// Multiplies the speed change by the maximum speed change, with some exceptions, to linearly limit speed to a maximum 
	// Max speed change gets smaller as we reach max speed so speed change gets smaller as we reach max speed too
	// Speed becomes harder to increase as max speed change reaches 0, at which point we can't increase speed anymore

	// Could use the max speed change in some easing or trigonometric function to limit speed at a non-linear rate

	float Speed = GivenVelocity.Size();
	// Prevent zero divisor nan, and act in a desired way
 	float ActualSpeed = Speed;
	if (Speed == 0)
	{
		Speed = 1;
	}

	float NewSpeedToSpeedRatio = NewVelocity.Size() / Speed;
	float SpeedChangeToSpeedRatio = NewSpeedToSpeedRatio - 1; // Speed is velocity's magnitude, velocity change (accel) and speed change can be very different

	float MaxSpeedChangeToSpeedRatio = (GivenMaxSpeed - ActualSpeed) / Speed;
	float ClampedMaxSpeedChangeToSpeedRatio = FMath::Max(0, MaxSpeedChangeToSpeedRatio); // Don't change the sign of speed change
	ClampedMaxSpeedChangeToSpeedRatio = FMath::Min(1, ClampedMaxSpeedChangeToSpeedRatio); // Don't speed up, only slow down

	float LimitedNewSpeedToSpeedRatio = 1 + SpeedChangeToSpeedRatio * ClampedMaxSpeedChangeToSpeedRatio;
	// Don't limit speed decreases
	if (!(FMath::Sign(SpeedChangeToSpeedRatio) > 0))
	{
		LimitedNewSpeedToSpeedRatio = NewSpeedToSpeedRatio;
	}
	// Clamp SpeedChange if it's going to overshoot
	if (SpeedChangeToSpeedRatio * ClampedMaxSpeedChangeToSpeedRatio > MaxSpeedChangeToSpeedRatio)
	{
		LimitedNewSpeedToSpeedRatio = 1 + MaxSpeedChangeToSpeedRatio;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Speed change to speed ratio: [%s]"), *FString::SanitizeFloat(SpeedChangeToSpeedRatio));
	// UE_LOG(LogTemp, Warning, TEXT("Speed to max speed ratio: [%s]"), *FString::SanitizeFloat(Speed / GivenMaxSpeed));
	// UE_LOG(LogTemp, Warning, TEXT("Clamped Max speed change to speed ratio: [%s]"), *FString::SanitizeFloat(ClampedMaxSpeedChangeToSpeedRatio));
	// UE_LOG(LogTemp, Warning, TEXT("Normal ratio: [%s]"), *FString::SanitizeFloat(NewSpeedToSpeedRatio));
	// UE_LOG(LogTemp, Warning, TEXT("Limited ratio: [%s]"), *FString::SanitizeFloat(LimitedNewSpeedToSpeedRatio));

	return LimitedNewSpeedToSpeedRatio;
}

void UBalingaMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
				
	if (bWantsToLand)
	{
		LandPressed();
	}

	if (!bWantsToFlap) { FlapReleased(); }
	else { FlapPressed(); }
}
bool UBalingaMovement::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

FNetworkPredictionData_Client* UBalingaMovement::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

		if (ClientPredictionData == nullptr)
		{
			UBalingaMovement* MutableThis = const_cast<UBalingaMovement*>(this);

			MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Balinga(*this);
			MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
			MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
		}

	return ClientPredictionData;
}
void UBalingaMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToLand = (Flags && FSavedMove_Balinga::FLAG_Land) != 0;
}

bool UBalingaMovement::FSavedMove_Balinga::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Balinga* NewBalingaMove = static_cast<FSavedMove_Balinga*>(NewMove.Get());

	if (saved_bWantsToLand != NewBalingaMove->saved_bWantsToLand)
	{
		return false;
	}

	if (saved_ThrustScale != NewBalingaMove->saved_ThrustScale)
	{
		return false;
	}

	if (saved_DragScale != NewBalingaMove->saved_DragScale)
	{
		return false;
	}
	if (saved_MinDragDesiredScale != NewBalingaMove->saved_MinDragDesiredScale)
	{
		return false;
	}
	if (saved_MaxDragDesiredScale != NewBalingaMove->saved_MaxDragDesiredScale)
	{
		return false;
	}

	if (saved_LiftScale != NewBalingaMove->saved_LiftScale)
	{
		return false;
	}
	if (saved_MinLiftDesiredScale != NewBalingaMove->saved_MinLiftDesiredScale)
	{
		return false;
	}
	if (saved_MaxLiftDesiredScale != NewBalingaMove->saved_MaxLiftDesiredScale)
	{
		return false;
	}

	if (saved_AngleOfAttack != NewBalingaMove->saved_AngleOfAttack)
	{
		return false;
	}
	if (saved_SurfaceArea != NewBalingaMove->saved_SurfaceArea)
	{
		return false;
	}
	if (saved_AirDensity != NewBalingaMove->saved_AirDensity)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}
void UBalingaMovement::FSavedMove_Balinga::Clear()
{
	Super::Clear();

	saved_bWantsToLand = false;

	saved_ThrustScale = 1;

	saved_DragScale = 1;
	saved_MinDragDesiredScale;
	saved_MaxDragDesiredScale = 1;

	saved_LiftScale = 1;
	saved_MinLiftDesiredScale;
	saved_MaxLiftDesiredScale = 1;
	// gravityScale is not ours

	saved_AngleOfAttack = 1;
	saved_SurfaceArea = 1;
	saved_WindVelocity = FVector::ZeroVector;
	saved_AirDensity = 1;
}
void UBalingaMovement::FSavedMove_Balinga::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UBalingaMovement* BalingaMovement = Cast<UBalingaMovement>(C->GetCharacterMovement());

	saved_bWantsToLand = BalingaMovement->bWantsToLand;

	saved_ThrustScale = BalingaMovement->ThrustScale;

	saved_DragScale = BalingaMovement->DragScale;
	saved_MaxDragDesiredScale = BalingaMovement->MaxDragDesiredScale;

	saved_LiftScale = BalingaMovement->LiftScale;
	saved_MaxLiftDesiredScale = BalingaMovement->MaxLiftDesiredScale;

	saved_SurfaceArea = BalingaMovement->SurfaceArea;
	saved_WindVelocity = BalingaMovement->WindVelocity;
	saved_AirDensity = BalingaMovement->AirDensity;
}
void UBalingaMovement::FSavedMove_Balinga::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UBalingaMovement* BalingaMovement = Cast<UBalingaMovement>(C->GetCharacterMovement());

	BalingaMovement->bWantsToLand = saved_bWantsToLand;

	BalingaMovement->ThrustScale = saved_ThrustScale;

	BalingaMovement->DragScale = saved_DragScale;
	BalingaMovement->MaxDragDesiredScale = saved_MaxDragDesiredScale;

	BalingaMovement->LiftScale = saved_LiftScale;
	BalingaMovement->MaxLiftDesiredScale = saved_MaxLiftDesiredScale;
	// gravity is not ours

	BalingaMovement->SurfaceArea = saved_SurfaceArea;
	BalingaMovement->WindVelocity = saved_WindVelocity;
	BalingaMovement->AirDensity = saved_AirDensity;
}
uint8 UBalingaMovement::FSavedMove_Balinga::GetCompressedFlags() const
{
	uint8 result = Super::GetCompressedFlags();

	if (saved_bWantsToLand) { result |= FLAG_Land; }
	return result;
}

// Idk why not declaring or defining it at all isn't an option, delgoodie did it so I shall too
UBalingaMovement::FNetworkPredictionData_Client_Balinga::FNetworkPredictionData_Client_Balinga(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}
FSavedMovePtr UBalingaMovement::FNetworkPredictionData_Client_Balinga::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Balinga());
}

