#include "BalingaMovement.h"
#include "Logging/LogMacros.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLoggerUtils.h"
#include "MathUtils.h"
#include "UI/AimerBase.h"
#include "BalingaControllerBase.h"

void UBalingaMovement::PhysFly(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	AccumulatedDeltaTime += DeltaTime;

	while (AccumulatedDeltaTime >= FixedDeltaTime)
	{
		TArray<FVector> CurrentForcesAndTorques = CalcForceAndTorque(Velocity, AngularVelocity, WindVelocity, FixedDeltaTime);

		TArray CurrentForces = TArray<FVector>(&CurrentForcesAndTorques.GetData()[0], FORCES_MAX);
		FVector CurrentForce = FVector::ZeroVector;
		for (int i = 0; i < FORCES_MAX; i++) { CurrentForce += CurrentForces[i]; }

		TArray<FVector> CurrentTorques = TArray<FVector>(&CurrentForcesAndTorques.GetData()[FORCES_MAX], TORQUES_MAX);
		FVector CurrentTorque = FVector::ZeroVector;
		for (int i = 0; i < TORQUES_MAX; i++) { CurrentTorque += CurrentTorques[i]; }

		FVector PredictedVelocity = Velocity + CalcForceAccel(CurrentForce, FixedDeltaTime * PredictionScale);
		FVector PredictedAngularVelocity = AngularVelocity + CalcTorqueAccel(CurrentTorque, FixedDeltaTime * PredictionScale);
		TArray<FVector> PredictedForcesAndTorques = CalcForceAndTorque(PredictedVelocity, PredictedAngularVelocity, WindVelocity, FixedDeltaTime);

		TArray<FVector> PredictedForces = TArray<FVector>(&PredictedForcesAndTorques.GetData()[0], FORCES_MAX);
		TArray<FVector> PredictedTorques = TArray<FVector>(&PredictedForcesAndTorques.GetData()[FORCES_MAX], TORQUES_MAX);

		// Averaged forces feel smoother, but harder to debug (can't average AoA and some other stuff)
		TArray<FVector> AverageForces;
		AverageForces.Init(FVector::ZeroVector, FORCES_MAX);
		for (int i = 0; i < FORCES_MAX; i++) { AverageForces[i] = (CurrentForces[i] + PredictedForces[i]) / 2; }
		FVector AverageForce = FVector::ZeroVector;
		for (int i = 0; i < FORCES_MAX; i++) { AverageForce += AverageForces[i]; }

		TArray<FVector> AverageTorques;
		AverageTorques.Init(FVector::ZeroVector, TORQUES_MAX);
		for (int i = 0; i < TORQUES_MAX; i++) { AverageTorques[i] = (CurrentTorques[i] + PredictedTorques[i]) / 2; }
		FVector AverageTorque = FVector::ZeroVector;
		for (int i = 0; i < TORQUES_MAX; i++) { AverageTorque += AverageTorques[i]; }

		FVector AverageAccel = CalcForceAccel(AverageForce, FixedDeltaTime);
		FVector NewVelocity = Velocity + AverageAccel;
		float Speed = Velocity.Size();
		
		float LimitedSpeedScale = CalcSpeedLimiterScale(Velocity, NewVelocity);

		NewVelocity = NewVelocity / (NewVelocity / Velocity) * LimitedSpeedScale;
		
		Velocity = NewVelocity;

		// AngularVelocity += CalcTorqueAccel(AverageTorque, FixedDeltaTime);

		ThrustThisFrame = FVector::ZeroVector;

		// Prep for SafeMoveUpdatedComponent()
		bJustTeleported = false;

		FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FHitResult Hit(1.f);
		FVector AdjustedVelocity = Velocity * FixedDeltaTime;

		FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
		FVector AdjustedAngularVelocity = AngularVelocity * FixedDeltaTime;
		FQuat NewRotation = FQuat::MakeFromEuler(AdjustedAngularVelocity) * OldRotation;

		SafeMoveUpdatedComponent(AdjustedVelocity, OldRotation, true, Hit); // Actually moves and rotates everything

		// Slides if we collide with a surface
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

		// Debug

		FVector Position = UpdatedComponent->GetComponentLocation() + AdjustedVelocity;
		FVector LeftLiftPosition = BalingaOwner->LeftLiftArrow->GetComponentLocation() + Velocity * FixedDeltaTime;
		FVector RightLiftPosition = BalingaOwner->RightLiftArrow->GetComponentLocation() + Velocity * FixedDeltaTime;
		FVector DragPosition = BalingaOwner->DragArrow->GetComponentLocation() + Velocity * FixedDeltaTime;

		float VlogScale = 1;
		float CylinderRadius = 1.5;
		float ArrowHeight = 20;
		float ArrowWidth = 0.28;

		if (bShouldEnableDebug)
		{
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), LeftLiftPosition, LeftLiftPosition + CalcForceAccel(AverageForces[FORCES_LeftWingLift], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Yellow, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), RightLiftPosition, RightLiftPosition + CalcForceAccel(AverageForces[FORCES_RightWingLift], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Yellow, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Yellow);
			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), DragPosition, DragPosition + CalcForceAccel(AverageForces[FORCES_Drag], FixedDeltaTime) * VlogScale, CylinderRadius, 15, FColor::Red, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Red);

			FDebugDrawer::DrawDebugArrow(CharacterOwner->GetWorld(), LeftLiftPosition, LeftLiftPosition + WingDirection * 140, CylinderRadius, 15, FColor::Blue, false, -1, 0, 1, ArrowHeight, ArrowWidth, 15, FColor::Blue);

			GEngine->AddOnScreenDebugMessage(2, 100.0f, FColor::Green, FString::Printf(TEXT("Velocity: [%s]"), *Velocity.ToCompactString()));

			GEngine->AddOnScreenDebugMessage(3, 100.0f, FColor::Purple, FString::Printf(TEXT("Angle of Attack: [%s]"), *FString::SanitizeFloat(FMath::RadiansToDegrees(AngleOfAttack))));
			GEngine->AddOnScreenDebugMessage(4, 100.0f, FColor::Magenta, FString::Printf(TEXT("Angle of Attack dot: [%s]"), *FString::SanitizeFloat(AoaDot)));
			GEngine->AddOnScreenDebugMessage(5, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift accel: [%s]"), *(CalcForceAccel((AverageForces[EForces::FORCES_LeftWingLift] + AverageForces[FORCES_RightWingLift]), FixedDeltaTime)).ToCompactString()));
			GEngine->AddOnScreenDebugMessage(6, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift coefficent: [%s]"), *FString::SanitizeFloat(LiftCoefficient)));
			GEngine->AddOnScreenDebugMessage(7, 100.0f, FColor::Red, FString::Printf(TEXT("Drag accel: [%s]"), *(CalcForceAccel(AverageForces[FORCES_Drag], FixedDeltaTime)).ToCompactString()));
			GEngine->AddOnScreenDebugMessage(8, 100.0f, FColor::Red, FString::Printf(TEXT("Drag coefficient: [%s]"), *FString::SanitizeFloat(DragCoefficient)));

			//GEngine->AddOnScreenDebugMessage(12, 100.0f, FColor::Purple, FString::Printf(TEXT("Desired difference: [%s]"), *(FString::SanitizeFloat(DesiredDifference.X) + ", " + FString::SanitizeFloat(DesiredDifference.Y) + ", " + FString::SanitizeFloat(DesiredDifference.Z))));
			//GEngine->AddOnScreenDebugMessage(13, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift desired scale: [%s]"), *FString::SanitizeFloat(LiftDesiredScale)));
			//GEngine->AddOnScreenDebugMessage(7, 100.0f, FColor::Red, FString::Printf(TEXT("FORCES_Drag desired scale: [%s]"), *FString::SanitizeFloat(DragDesiredScale)));

			//FDebugDrawer::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", Position, Position + AngularVelocity, CylinderRadius, FColor::Green, "Angular velocity" + AngularVelocity.ToCompactString(), ArrowHeight, ArrowWidth, FColor::Green, "");

			//GEngine->AddOnScreenDebugMessage(6, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift torque projection scale: [%s]"), *FString::SanitizeFloat(LiftTorqueProjectionScale)));
			GEngine->AddOnScreenDebugMessage(9, 100.0f, FColor::Green, FString::Printf(TEXT("Angular velocity: [%s]"), *AngularVelocity.ToString()));
			GEngine->AddOnScreenDebugMessage(10, 100.0f, FColor::Silver, FString::Printf(TEXT("Aimer position: [%s]"), *AimerPosition.ToString()));
			GEngine->AddOnScreenDebugMessage(11, 100.0f, FColor::White, FString::Printf(TEXT("Wing roll: [%s]"), *CalcTorqueAccel(AverageTorques[TORQUES_LiftRoll], FixedDeltaTime).ToString()));
			//GEngine->AddOnScreenDebugMessage(12, 100.0f, FColor::Black, FString::Printf(TEXT("Right wing torque: [%s]"), *WingLiftTorqueAccels[1].ToString()));
		}

		AccumulatedDeltaTime -= FixedDeltaTime;
		Iterations++;
	}
}

TArray<FVector> UBalingaMovement::CalcForceAndTorque(FVector GivenVelocity, FVector GivenAngularVelocity, FVector GivenWindVelocity, float DeltaTime)
{
	TArray<FVector> Forces;
	Forces.Init(FVector::ZeroVector, FORCES_MAX);
	TArray<FVector> Torques;
	Torques.Init(FVector::ZeroVector, TORQUES_MAX);
	
	Forces[FORCES_Gravity] = -GetGravityDirection() * GetGravityZ() * Mass;

	Forces[FORCES_Thrust] = ThrustThisFrame;

	FVector FlowVelocity = GivenVelocity + GivenWindVelocity;
	FVector FlowDirection = FlowVelocity.GetSafeNormal();

	FVector ActorRight = CharacterOwner->GetActorRightVector();
	FVector ActorForward = CharacterOwner->GetActorForwardVector();
	FVector ActorUp = CharacterOwner->GetActorUpVector();

	FVector DesiredDifference;
	FVector DesiredDiffDirection = CalcDesiredDiffDirection(FlowDirection, ActorForward);

	WingDirection = (ActorForward /* - (AimerPosition.Y / 60 * ActorUp * AimerAoaScale)*/).GetSafeNormal();
	AngleOfAttack = CalcAngleOfAttack(GivenVelocity, ActorRight, ActorUp);

	TArray<FVector> WingLifts = CalcLifts(FlowVelocity, DesiredDifference, ActorRight, ActorForward, ActorUp, DeltaTime);
	Forces[FORCES_LeftWingLift] = WingLifts[0];
	Forces[FORCES_RightWingLift] = WingLifts[1];

	FVector Drag = CalcDrag(FlowVelocity, DesiredDiffDirection, DeltaTime);
	DragAccel = CalcForceAccel(Drag, DeltaTime);
	Forces[FORCES_Drag] = Drag;

	FVector LiftRoll;
	if (!(WingLifts[0] + WingLifts[1]).GetSafeNormal().IsZero()) // Cannot get direction from zero lift
	{
		LiftRoll = CalcLiftRoll(WingLifts, ActorForward, ActorUp);
	}
	else 
	{
		LiftRoll = CalcLiftRoll(WingLifts, FlowVelocity, ActorRight, ActorForward, ActorUp);
	}
	Torques[TORQUES_LiftRoll] = LiftRoll;

	FVector AngularDrag = GivenAngularVelocity.GetSafeNormal() * FMath::Square(GivenAngularVelocity.Size()) * -1.0f * 0.5 * AngularDragScale;
	Torques[TORQUES_AngularDrag] = AngularDrag;

	TArray<FVector> ForcesAndTorques;
	for (int i = 0; i < FORCES_MAX; i++) { ForcesAndTorques.Emplace(Forces[i]); }
	for (int i = 0; i < TORQUES_MAX; i++) { ForcesAndTorques.Emplace(Torques[i]); }

	return ForcesAndTorques;
}

TArray<FVector> UBalingaMovement::CalcLifts(FVector FlowVelocity, FVector DesiredDifference, FVector ActorRight, FVector ActorForward, FVector ActorUp, float DeltaTime)
{
	// Don't count velocity going in the right direction towards lift's velocity squared (might not work, check later)
	FVector SomethingIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnToNormal(ActorRight);

	FVector LiftDirection = FVector::CrossProduct(SomethingIndependentVelocity.GetSafeNormal(), ActorRight);
	FVector Lift = FMath::Square(SomethingIndependentVelocity.Size()) * LiftDirection * LiftScale * 0.5;

	// Clamping AoA gives us the lift curve we want
	float ClampedAoa = (FMath::Abs(AngleOfAttack) > FMath::DegreesToRadians(45)) ? FMath::DegreesToRadians(45) * FMath::Sign(AngleOfAttack) : AngleOfAttack;
	LiftCoefficient = FMath::Sin(ClampedAoa * 4);
	Lift *= LiftCoefficient;

	FVector LiftAccel = Lift / Mass * DeltaTime;

	if (Lift.Size() != 0) // Prevents NaN from zero divisor
	{
		// Check if the next Aoa sign is different to this one, if so overshoot the Aoa less or not at all
		float AssumedAoaSign = CalcAoaSign(SomethingIndependentVelocity + LiftAccel, ActorRight, ActorUp);
	
		if (AssumedAoaSign != FMath::Sign(AngleOfAttack))
		{
			LiftAccel = Lift / LiftCoefficient / Mass * DeltaTime;

			FVector AoaDifference = (WingDirection - SomethingIndependentVelocity.GetSafeNormal());
			float AoaLiftProjectionScale = (AoaDifference.Dot(LiftAccel) / LiftAccel.Size());
			LiftCoefficient = FMath::Abs(LiftCoefficient * AoaLiftProjectionScale) * FMath::Sign(AngleOfAttack);
			LiftAccel *= LiftCoefficient;
		}
	}

	// Distribute lift to allow for torque

	Lift = (LiftAccel * Mass) / DeltaTime;
	FVector LeftWingLift = Lift / 2;
	FVector RightWingLift = Lift / 2;

	AimerPosition = Cast<ABalingaControllerBase>(GetController())->GetAimerPosition();
	if (FMath::Sign(AimerPosition.X) == 1 && AimerPosition.X != 0)
	{
		RightWingLift *= 1 - AimerPosition.GetAbs().X / 60;
	}
	else if (AimerPosition.X != 0)
	{
		LeftWingLift *= 1 - AimerPosition.GetAbs().X / 60;
	}

	Lift = LeftWingLift + RightWingLift;

	// We can limit turning speed by clamping lift to some scale of velocity's size
	// Or take the component of lift that's opposite (or something) to our current direction away from some turning budget
	// Lift can be greater than velocity IRL (accel needs to be bigger than vel, to even start moving), 
	// but at high speeds that would normally break the wing of the bird/plane
	LiftAccel = CalcForceAccel(Lift, DeltaTime);
	if (LiftAccel.Size() > FlowVelocity.Size())
	{
		float LiftClampScale = (FlowVelocity.Size() / LiftAccel.Size());
		LeftWingLift *= LiftClampScale;
		RightWingLift *= LiftClampScale;
	}

	return TArray<FVector>{LeftWingLift, RightWingLift};
}

FVector UBalingaMovement::CalcDrag(FVector FlowVelocity, FVector DesiredDiffDirection, float DeltaTime)
{
	FVector FlowDirection = FlowVelocity.GetSafeNormal();
	FVector Drag = FMath::Square(FlowVelocity.Size()) * FlowDirection * 0.5 * -1.0 * DragScale;

	DragCoefficient = 1.01 - FMath::Cos(AngleOfAttack * 2);
	Drag *= DragCoefficient;
	
	float VelocitySize = FlowVelocity.Size();
	float DragAccelSize = CalcForceAccel(Drag, DeltaTime).Size();
	
	if (DragAccelSize > VelocitySize)
	{
		float DragClampScale = (VelocitySize / DragAccelSize);
 		Drag *= DragClampScale;
	}

	return Drag;
}

FVector UBalingaMovement::CalcLiftRoll(TArray<FVector> WingLifts, FVector ActorForward, FVector ActorUp)
{
	FVector MinMuscleForce = (WingLifts[0] + WingLifts[1]).GetSafeNormal() * MinWingMuscleForce;
	//GEngine->AddOnScreenDebugMessage(14, 100.0f, FColor::Yellow, FString::Printf(TEXT("Wing force direction: [%s]"), *WingLifts[0].GetSafeNormal().ToString()));

	TArray<FVector> MinMuscleForces = { MinMuscleForce, MinMuscleForce };

	float LeftWingLiftSize = WingLifts[0].Size();
	float RightWingLiftSize = WingLifts[1].Size();

	if (LeftWingLiftSize > RightWingLiftSize)
	{
		MinMuscleForces[1] *= RightWingLiftSize / LeftWingLiftSize;
	}
	else if (RightWingLiftSize > LeftWingLiftSize)
	{
		MinMuscleForces[0] *= LeftWingLiftSize / RightWingLiftSize;
	}

	TArray<FVector> WingForces = (FMath::Max(WingLifts[0].Size(), WingLifts[1].Size()) > MinMuscleForce.Size()) ? WingLifts : MinMuscleForces;
	TArray<FVector> WingLiftTorques =
	{
		CalcTorqueFromForceAtPos(WingForces[0] * WingTorqueScale, CharacterOwner->GetActorRightVector() * WingMidpointDistance * -1),
		CalcTorqueFromForceAtPos(WingForces[1] * WingTorqueScale, CharacterOwner->GetActorRightVector() * WingMidpointDistance)
	};

	// The closer the torque is to the forward axis, the stronger it'll be 
	float LiftTorqueProjectionScale = FVector::DotProduct(WingLiftTorques[0] + WingLiftTorques[1], ActorForward) / (WingLiftTorques[0] + WingLiftTorques[1]).Size();
	LiftTorqueProjectionScale = (FMath::IsNaN(LiftTorqueProjectionScale)) ? 0 : LiftTorqueProjectionScale;
	if (WingForces[0] == MinMuscleForces[0] || WingForces[1] == MinMuscleForces[1])
	{
		LiftTorqueProjectionScale = 1 * FMath::Sign(AimerPosition.X);
	}

	//GEngine->AddOnScreenDebugMessage(16, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift torque projection scale: [%s]"), *FString::SanitizeFloat(LiftTorqueProjectionScale)));


	FVector FinalLiftTorque = ActorForward * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftTorqueProjectionScale;
	return FinalLiftTorque;
}

FVector UBalingaMovement::CalcLiftRoll(TArray<FVector> WingLifts, FVector FlowVelocity, FVector ActorRight, FVector ActorForward, FVector ActorUp)
{
	FVector SomethingIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnToNormal(ActorRight);
	FVector LiftDirection = FVector::CrossProduct(FlowVelocity.GetSafeNormal(), ActorRight);
	FVector MinMuscleForce = LiftDirection * MinWingMuscleForce;
	//GEngine->AddOnScreenDebugMessage(14, 100.0f, FColor::Yellow, FString::Printf(TEXT("Wing force direction (was 0): [%s]"), *LiftDirection.ToString()));

	TArray<FVector> MinMuscleForces = { MinMuscleForce, MinMuscleForce };

	if (FMath::Sign(AimerPosition.X) == 1 && AimerPosition.X != 0)
	{
		MinMuscleForces[0] *= 1 - AimerPosition.GetAbs().X / 60;
	}
	else if (AimerPosition.X != 0)
	{
		MinMuscleForces[1] *= 1 - AimerPosition.GetAbs().X / 60;
	}

	TArray<FVector> WingForces = (FMath::Max(WingLifts[0].Size(), WingLifts[1].Size()) > MinMuscleForce.Size()) ? WingLifts : MinMuscleForces;
	TArray<FVector> WingLiftTorques =
	{
		CalcTorqueFromForceAtPos(WingForces[0] * WingTorqueScale, CharacterOwner->GetActorRightVector() * WingMidpointDistance * -1),
		CalcTorqueFromForceAtPos(WingForces[1] * WingTorqueScale, CharacterOwner->GetActorRightVector() * WingMidpointDistance)
	};

	float LiftTorqueProjectionScale = FVector::DotProduct(WingLiftTorques[0] + WingLiftTorques[1], ActorForward) / (WingLiftTorques[0] + WingLiftTorques[1]).Size();
	if (WingForces[0] == MinMuscleForces[0] || WingForces[1] == MinMuscleForces[1])
	{
		LiftTorqueProjectionScale = 1 * FMath::Sign(AimerPosition.X);
		//UE_LOG(LogTemp, Warning, TEXT("Lift size: [%s]"), *(WingLifts[0] + WingLifts[1]).ToString());
	}
	LiftTorqueProjectionScale = (FMath::IsNaN(LiftTorqueProjectionScale)) ? 0 : LiftTorqueProjectionScale;
	//GEngine->AddOnScreenDebugMessage(16, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift torque projection scale: [%s]"), *FString::SanitizeFloat(LiftTorqueProjectionScale)));

	FVector FinalLiftTorque = ActorForward * (WingLiftTorques[0] + WingLiftTorques[1]).Size() * LiftTorqueProjectionScale;
	return FinalLiftTorque;
} 

FVector UBalingaMovement::CalcDesiredDiffDirection(FVector FlowDirection, FVector ActorForward)
{
	FVector DesiredDifference = CharacterOwner->GetActorForwardVector() - FlowDirection;
	FVector DesiredDifferenceDirection = DesiredDifference.GetSafeNormal();

	// DON'T NEED TO SMOOTH ANYTHING YET (eventually smooth high speed changes since they feel snappy)
	/*std::tuple<FVector, FVector> data = FMathUtils::SmoothDamp3(LastDesiredDifference, DesiredDiffDirection, SmoothVelocity, SmoothTime);
	LastDesiredDifference = DesiredDiffDirection;
	DesiredDiffDirection = std::get<0>(data);
	SmoothVelocity = std::get<1>(data);*/

	return DesiredDifferenceDirection;
}

float UBalingaMovement::CalcAngleOfAttack(FVector GivenVelocity, FVector ActorRight, FVector ActorUp)
{
	FVector SomethingIndependentVelocity = GivenVelocity - GivenVelocity.ProjectOnToNormal(ActorRight);

	float AoaSign = CalcAoaSign(SomethingIndependentVelocity, ActorRight, ActorUp);
	AoaDot = FMath::Abs(WingDirection.Dot(SomethingIndependentVelocity.GetSafeNormal()));
	return FMath::Acos(AoaDot) * AoaSign;
}

float UBalingaMovement::CalcAoaSign(FVector GivenVelocity, FVector ActorRight, FVector ActorUp)
{
	FVector SomethingIndependentVelocity = GivenVelocity - GivenVelocity.ProjectOnToNormal(ActorRight);
	ActorUp = CharacterOwner->GetActorUpVector();

	FVector AoaDifference = (WingDirection - SomethingIndependentVelocity.GetSafeNormal());
	FVector AoaDifferenceDirection = AoaDifference.GetSafeNormal();
	float AoaSign = FMath::Sign(AoaDifferenceDirection.Dot(ActorUp));

	return AoaSign;
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
FVector UBalingaMovement::CalcForceAccel(FVector Force, float DeltaTime)
{
	return Force / Mass * DeltaTime;
}

void UBalingaMovement::AddTorqueToAngularVel(FVector Torque, float DeltaTime)
{
	AngularVelocity += CalcTorqueAccel(Torque, DeltaTime);
}
FVector UBalingaMovement::CalcTorqueAccel(FVector Torque, float DeltaTime)
{
	return Torque / MomentInertia * DeltaTime;
}
float UBalingaMovement::CalcSpeedLimiterScale(FVector GivenVelocity, FVector NewVelocity)
{
	float Speed = GivenVelocity.Size();
	// Prevent zero divisor nan, and act in an expected way
	float ActualSpeed = Speed;
	if (Speed == 0)
	{
		Speed = 1;
	}

	float NewSpeedToSpeedRatio = NewVelocity.Size() / Speed;
	float SpeedChangeToSpeedRatio = NewSpeedToSpeedRatio - 1; // Speed is velocity's magnitude, velocity change (accel) and speed change can be very different

	float MaxSpeedChangeToSpeedRatio = (MaxFlightSpeed - ActualSpeed) / Speed;
	float ClampedMaxSpeedChangeToSpeedRatio = FMath::Max(0, MaxSpeedChangeToSpeedRatio); // Should not change the sign of speed change
	ClampedMaxSpeedChangeToSpeedRatio = FMath::Min(1, ClampedMaxSpeedChangeToSpeedRatio); // Should not speed up, only slow downo

	float LimitedNewSpeedToSpeedRatio = (FMath::Sign(SpeedChangeToSpeedRatio) > 0) ? 1 + SpeedChangeToSpeedRatio * (ClampedMaxSpeedChangeToSpeedRatio) : NewSpeedToSpeedRatio;
	if (SpeedChangeToSpeedRatio * ClampedMaxSpeedChangeToSpeedRatio > ClampedMaxSpeedChangeToSpeedRatio)
	{
		LimitedNewSpeedToSpeedRatio = 1 + MaxSpeedChangeToSpeedRatio;
	}

	if (bShouldEnableDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Speed change to speed ratio: [%s]"), *FString::SanitizeFloat(SpeedChangeToSpeedRatio));
		UE_LOG(LogTemp, Warning, TEXT("Speed to max speed ratio: [%s]"), *FString::SanitizeFloat(Speed / MaxFlightSpeed));
		UE_LOG(LogTemp, Warning, TEXT("Clamped Max speed change to speed ratio: [%s]"), *FString::SanitizeFloat(ClampedMaxSpeedChangeToSpeedRatio));
		UE_LOG(LogTemp, Warning, TEXT("Normal ratio: [%s]"), *FString::SanitizeFloat(NewSpeedToSpeedRatio));
		UE_LOG(LogTemp, Warning, TEXT("Limited ratio: [%s]"), *FString::SanitizeFloat(LimitedNewSpeedToSpeedRatio));
	}

	return LimitedNewSpeedToSpeedRatio;
}
FVector UBalingaMovement::CalcTorqueFromForceAtPos(FVector Force, FVector Position)
{	
	return Force.Cross(Position);
}

void UBalingaMovement::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());

	FixedDeltaTime = 1 / FixedDeltaTimeFraction;

	ThrustScale = DefaultThrustScale;

	DragScale = DefaultDragScale;
	MinDragDesiredScale = DefaultMinDragDesiredScale;
	DragDesiredScaleScale = DefaultDragDesiredScaleScale;
	
	AngularDragScale = DefaultAngularDragScale;

	LiftScale = DefaultLiftScale;
	MinLiftDesiredScale = DefaultMinLiftDesiredScale;
	LiftDesiredScaleScale = DefaultLiftDesiredScaleScale;

	SurfaceArea = DefaultSurfaceArea;
	WindVelocity = DefaultWindVelocity;
	AirDensity = DefaultAirDensity;

	LastDesiredDifference = FVector::ZeroVector;
	SmoothVelocity = FVector::ZeroVector;

}

void UBalingaMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Fly:
		PhysFly(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode."))
	}
}

void UBalingaMovement::EnterFly()
{
	if (!bShouldSnapRotation)
	{
		CharacterOwner->bUseControllerRotationPitch = false;
		CharacterOwner->bUseControllerRotationRoll = false;
		CharacterOwner->bUseControllerRotationYaw = false;
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

	SetMovementMode(MOVE_Custom, CMOVE_Fly);
}
void UBalingaMovement::ExitFly()
{
	//CharacterOwner->bUseControllerRotationPitch = true;
	//CharacterOwner->bUseControllerRotationRoll = true;
	CharacterOwner->bUseControllerRotationYaw = true;


	SetMovementMode(MOVE_Falling);
}

void UBalingaMovement::FlapPressed()
{
	// Thrust is applied in the direction the character is facing
	if (CharacterOwner->bPressedJump)
	{
		FVector ActorForward = CharacterOwner->GetActorForwardVector();
		ThrustThisFrame = ThrustScale * ActorForward;

		CharacterOwner->bPressedJump = false;
	}
}

void UBalingaMovement::LandPressed()
{
	ExitFly();
}
void UBalingaMovement::LandReleased() {}

void UBalingaMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (bWantsToLand)
	{
		LandPressed();
	}
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
	if (saved_DragDesiredScaleScale != NewBalingaMove->saved_DragDesiredScaleScale)
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
	if (saved_LiftDesiredScaleScale != NewBalingaMove->saved_LiftDesiredScaleScale)
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
	saved_DragDesiredScaleScale = 1;

	saved_LiftScale = 1;
	saved_MinLiftDesiredScale;
	saved_LiftDesiredScaleScale = 1;
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
	saved_MinDragDesiredScale = BalingaMovement->MinDragDesiredScale;
	saved_DragDesiredScaleScale = BalingaMovement->DragDesiredScaleScale;

	saved_LiftScale = BalingaMovement->LiftScale;
	saved_MinLiftDesiredScale = BalingaMovement->MinLiftDesiredScale;
	saved_LiftDesiredScaleScale = BalingaMovement->LiftDesiredScaleScale;

	saved_AngleOfAttack = BalingaMovement->AngleOfAttack;
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
	BalingaMovement->MinDragDesiredScale = saved_MinDragDesiredScale;
	BalingaMovement->DragDesiredScaleScale = saved_DragDesiredScaleScale;

	BalingaMovement->LiftScale = saved_LiftScale;
	BalingaMovement->MinLiftDesiredScale = saved_MinLiftDesiredScale;
	BalingaMovement->LiftDesiredScaleScale = saved_LiftDesiredScaleScale;
	// gravity is not ours

	BalingaMovement->AngleOfAttack = saved_AngleOfAttack;
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

// Idk why not declaring or defining it at all isn't an option, the guy did it so I shall too
UBalingaMovement::FNetworkPredictionData_Client_Balinga::FNetworkPredictionData_Client_Balinga(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}
FSavedMovePtr UBalingaMovement::FNetworkPredictionData_Client_Balinga::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Balinga());
}

