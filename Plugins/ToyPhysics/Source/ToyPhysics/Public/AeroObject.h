#pragma once

// Could make other types of aerodynamic objects with this (planes, spheres), with their own variances in coefficients and other variables
class TOYPHYSICS_API FAeroObject
{
public:
	FAeroObject(){};
	virtual TArray<FVector> CalcForces(FVector WindVelocity, FVector BodyForward, FVector BodyRight, FVector BodyUp);

protected: 
	virtual FVector CalcLift(float LiftCoeff, FVector Right, FVector FlowVelocity) const;
	virtual FVector CalcLiftDirection(FVector FlowVelocity, FVector Right) const;
	virtual float CalcLiftMagnitude(float LiftCoeff, FVector FlowVelocity, FVector Right) const;
	virtual float CalcLiftCoeff(float Aoa) const;

	virtual FVector CalcDrag(float DragCoeff, FVector FlowVelocity) const;
	virtual FVector CalcDragDirection(FVector FlowVelocity) const;
	virtual float CalcDragMagnitude(float DragCoeff, FVector FlowVelocity) const;
	virtual float CalcDragCoeff(float Aoa) const;

	virtual FVector CalcInducedDrag(float InducedDragCoeff, FVector FlowVelocity) const;
	virtual FVector CalcInducedDragDirection(FVector FlowVelocity) const;
	virtual float CalcInducedDragMagnitude(float InducedDragCoeff, FVector FlowVelocity) const;
	virtual float CalcInducedDragCoeff(float Aoa) const;

	virtual FVector CalcFlowVelocity(FVector WindVelocity) const;

public:
	float CalcAngleOfAttack(FVector Forward, FVector Right, FVector Up) const;
	static float CalcAngleOfAttack(FVector FlowVelocity, FVector Forward, FVector Right, FVector Up);
	static float CalcAoaSign(FVector FlowVelocity, FVector Forward, FVector Right, FVector Up);

	FVector Position;
	FVector Velocity;
	virtual FVector CalcPosition(FVector BodyForward, FVector BodyRight, FVector BodyUp) const;
	void SetPosAndVel(FVector NewPosition);
	void SetPos(FVector NewPosition); // Set position without velocity

protected:
	float Incidence = 0;
	float Hedral = 0;
	float Sweep = 0; // May not implement its effect for a while
public:
	void SetIncidence(float NewIncidence);
	void SetHedral(float NewHedral);
	void SetSweep(float NewSweep);

	FVector CalcForward(FVector BodyForward, FVector BodyRight, FVector BodyUp) const;
	FVector CalcRight(FVector BodyRight, FVector BodyForward, FVector BodyUp) const;
	FVector CalcUp(FVector BodyUp, FVector BodyForward, FVector BodyRight) const;
};