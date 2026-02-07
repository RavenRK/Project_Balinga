#include "AeroObject.h"

TArray<FVector> FAeroObject::CalcForces(FVector WindVelocity, FVector BodyForward, FVector BodyRight, FVector BodyUp)
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

	float InducedDragCoeff = CalcInducedDragCoeff(Aoa);
	Forces[2] = CalcInducedDrag(InducedDragCoeff, FlowVelocity);

	return Forces;
}

FVector FAeroObject::CalcPosition(FVector BodyForward, FVector BodyRight, FVector BodyUp) const
{
	return FVector::ZeroVector;
}

void FAeroObject::SetPosAndVel(FVector NewPosition)
{
	Velocity = NewPosition - Position;
	Position = NewPosition;
}
void FAeroObject::SetPos(FVector NewPosition)
{
	Position = NewPosition;
}

FVector FAeroObject::CalcLift(float LiftCoeff, FVector Right, FVector FlowVelocity) const
{
	return CalcLiftDirection(FlowVelocity, Right) * CalcLiftMagnitude(LiftCoeff, FlowVelocity, Right);
}
FVector FAeroObject::CalcLiftDirection(FVector FlowVelocity, FVector Right) const
{
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(Right);

	return FlowVelocity.GetSafeNormal().Cross(Right);
}
float FAeroObject::CalcLiftMagnitude(float LiftCoeff, FVector FlowVelocity, FVector Right) const
{
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(Right);

	return (FMath::Square(RightIndependentVelocity.Size())) * 0.5 * LiftCoeff;
}
float FAeroObject::CalcLiftCoeff(float Aoa) const
{
	return 1;
}

FVector FAeroObject::CalcDrag(float DragCoeff, FVector FlowVelocity) const
{
	return CalcDragDirection(FlowVelocity) * CalcDragMagnitude(DragCoeff, FlowVelocity);
}
FVector FAeroObject::CalcDragDirection(FVector FlowVelocity) const
{
	return FlowVelocity.GetSafeNormal() * -1;
}
float FAeroObject::CalcDragMagnitude(float DragCoeff, FVector FlowVelocity) const
{
	return FMath::Square(FlowVelocity.Size()) * 0.5 * DragCoeff;
}
float FAeroObject::CalcDragCoeff(float Aoa) const
{
	return 1;
}

FVector FAeroObject::CalcInducedDrag(float InducedDragCoeff, FVector FlowVelocity) const
{
	return CalcInducedDragDirection(FlowVelocity) * CalcInducedDragMagnitude(InducedDragCoeff, FlowVelocity);
}
FVector FAeroObject::CalcInducedDragDirection(FVector FlowVelocity) const
{
	return FlowVelocity.GetSafeNormal() * -1;
}
float FAeroObject::CalcInducedDragMagnitude(float InducedDragCoeff, FVector FlowVelocity) const
{
	return FMath::Square(FlowVelocity.Size()) * 0.5 * InducedDragCoeff;
}
float FAeroObject::CalcInducedDragCoeff(float Aoa) const
{
	return 1;
}

FVector FAeroObject::CalcFlowVelocity(FVector WindVelocity) const
{
	return Velocity + WindVelocity;
}

float FAeroObject::CalcAngleOfAttack(FVector Forward, FVector Right, FVector Up) const
{
	return CalcAngleOfAttack(Velocity, Forward, Right, Up);
}
float FAeroObject::CalcAngleOfAttack(FVector FlowVelocity, FVector Forward, FVector Right, FVector Up)
{
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(Right); // Only measure pitch, not yaw or roll (can't measure roll from forward vel)

	float AoaSign = CalcAoaSign(RightIndependentVelocity, Forward, Right, Up);
	float AoaDot = FMath::Abs(Forward.Dot(RightIndependentVelocity.GetSafeNormal()));
	return (FMath::Acos(AoaDot) * AoaSign);
}
float FAeroObject::CalcAoaSign(FVector FlowVelocity, FVector Forward, FVector Right, FVector Up)
{
	FVector RightIndependentVelocity = FlowVelocity - FlowVelocity.ProjectOnTo(Right); // Only measure pitch, not yaw or roll (can't measure roll from forward vel)

	FVector AoaDifference = (Forward - RightIndependentVelocity.GetSafeNormal());
	FVector AoaDifferenceDirection = AoaDifference.GetSafeNormal();
	float AoaSign = FMath::Sign(AoaDifferenceDirection.Dot(Up));

	return AoaSign;
}

// Can't just rotate body vector by singular incidence/sweep/hedral around body axis, because they aren't the only axis that varies
// Therefore have to rotate it around the other two AeroObject axes
FVector FAeroObject::CalcForward(FVector BodyForward, FVector BodyRight, FVector BodyUp) const
{
	return FQuat::MakeFromRotationVector(BodyRight * Incidence + BodyUp * Sweep).RotateVector(BodyForward);
}
FVector FAeroObject::CalcRight(FVector BodyRight, FVector BodyForward, FVector BodyUp) const
{
	return FQuat::MakeFromRotationVector(BodyUp * Sweep + BodyForward * Hedral).RotateVector(BodyRight);
}
FVector FAeroObject::CalcUp(FVector BodyUp, FVector BodyForward, FVector BodyRight) const
{
	return FQuat::MakeFromRotationVector(BodyForward * Hedral + BodyRight * Incidence).RotateVector(BodyUp);
}

void FAeroObject::SetIncidence(float NewIncidence)
{
	Incidence = NewIncidence;
	UE_LOG(LogTemp, Warning, TEXT("True incidence: [%s]"), *FString::SanitizeFloat(NewIncidence));

}
void FAeroObject::SetHedral(float NewHedral)
{
	Hedral = NewHedral;
}
void FAeroObject::SetSweep(float NewSweep)
{
	Sweep = NewSweep;
}

