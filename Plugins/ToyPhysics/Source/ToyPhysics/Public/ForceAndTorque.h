#pragma once

class TOYPHYSICS_API FForce
{
public:
	static FVector CalcAccelForce(FVector GivenAcceleration, float GivenMass, float DeltaTime);
	static FVector CalcForceAccel(FVector Force, float GivenMass, float DeltaTime);
};

class TOYPHYSICS_API FTorque
{
public:
	static FVector CalcTorqueFromForceAtPos(FVector Force, FVector Position);
	static FVector CalcAccelTorque(FVector GivenAcceleration, float GivenMomentInertia, float DeltaTime);
	static FVector CalcTorqueAccel(FVector Torque, float GivenMomentInertia, float DeltaTime);
};