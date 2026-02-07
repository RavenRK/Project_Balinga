#include "ForceAndTorque.h"
FVector FForce::CalcAccelForce(FVector GivenAcceleration, float GivenMass, float DeltaTime)
{
	return GivenAcceleration / DeltaTime * GivenMass;
}
FVector FForce::CalcForceAccel(FVector Force, float GivenMass, float DeltaTime)
{
	return Force / GivenMass * DeltaTime;
}

FVector FTorque::CalcTorqueFromForceAtPos(FVector Force, FVector Position)
{
	return Position.Cross(Force);
}
FVector FTorque::CalcAccelTorque(FVector GivenAcceleration, float GivenMomentInertia, float DeltaTime)
{
	return GivenAcceleration / DeltaTime * GivenMomentInertia;
}
FVector FTorque::CalcTorqueAccel(FVector Torque, float GivenMomentInertia, float DeltaTime)
{
	return Torque / GivenMomentInertia * DeltaTime;
}