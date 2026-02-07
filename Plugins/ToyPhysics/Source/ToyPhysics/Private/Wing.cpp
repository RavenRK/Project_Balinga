#include "Wing.h"

TArray<FVector> FWing::CalcForces(FVector WindVelocity, FVector BodyForward, FVector BodyRight, FVector BodyUp)
{
	TArray<FVector> Forces;
	Forces.Init(FVector::ZeroVector, 3);
	FVector FlowVelocity = Velocity + WindVelocity;

	FVector Forward = CalcForward(BodyForward, BodyRight, BodyUp);
	FVector Right = CalcRight(BodyRight, BodyForward, BodyUp);
	FVector Up = CalcUp(BodyUp, BodyForward, BodyRight);

	float Aoa = CalcAngleOfAttack(FlowVelocity, Forward, Right, Up);

	float LiftCoeff = CalcLiftCoeff(Aoa);
	Forces[0] = CalcLift(LiftCoeff, Right, FlowVelocity);

	float DragCoeff = CalcDragCoeff(Aoa);
	Forces[1] = CalcDrag(DragCoeff, FlowVelocity);

	float Area = CalcArea();
	float AspectRatio = CalcAspectRatio(Area);
	float InducedDragCoeff = CalcInducedDragCoeff(LiftCoeff, AspectRatio);
	Forces[2] = CalcInducedDrag(InducedDragCoeff, FlowVelocity);

	return Forces;
}

float FWing::CalcDragMagnitude(float DragCoeff, FVector FlowVelocity) const
{
	return FAeroObject::CalcDragMagnitude(DragCoeff, FlowVelocity) * CalcArea();
}

float FWing::CalcLiftMagnitude(float LiftCoeff, FVector FlowVelocity, FVector Right) const
{
	return FAeroObject::CalcLiftMagnitude(LiftCoeff, FlowVelocity, Right) * CalcArea() * 1.5;
}

float FWing::CalcLiftCoeff(float Aoa) const
{
	float MaxStallAoa = CriticalAoa * 2;
	float ClampedAoa;
	if (FMath::Abs(Aoa) > FMath::DegreesToRadians(MaxStallAoa))
	{
		ClampedAoa = FMath::DegreesToRadians(MaxStallAoa) * FMath::Sign(Aoa);
	}
	else
	{
		ClampedAoa = Aoa;
	}

	float LiftCoefficient = FMath::Sin(ClampedAoa * (180 / MaxStallAoa));

	return LiftCoefficient;
}

float FWing::CalcDragCoeff(float Aoa) const
{
	return (1 - FMath::Cos(Aoa * DragCoeffAoaScale));
}

float FWing::CalcInducedDragMagnitude(float InducedDragCoeff, FVector FlowVelocity) const
{
	return FAeroObject::CalcInducedDragMagnitude(InducedDragCoeff, FlowVelocity) * CalcArea() / 2;
}

float FWing::CalcInducedDragCoeff(float LiftCoeff, float AspectRatio)
{
	return FMath::Square(LiftCoeff) / (AspectRatio * UE_PI * Efficiency);
}

float FWing::CalcArea() const
{
	return Chord * Span;
}
float FWing::CalcAspectRatio(float Area) const
{
	return FMath::Square(Span) / Area;
}
FVector FWing::CalcPosition(FVector BodyForward, FVector BodyRight, FVector BodyUp) const
{
	FVector Right = CalcRight(BodyRight, BodyForward, BodyUp);
	FVector Forward = CalcForward(BodyForward, BodyRight, BodyUp);

	return Right * Span * SpanDirection * 0.5 + Forward * Chord * ChordDirection * 0.5;
}
