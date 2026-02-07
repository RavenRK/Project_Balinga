#pragma once
#include "AeroObject.h"

class TOYPHYSICS_API FWing : public FAeroObject
{
public:
	FWing(float CriticalAoa, float DragCoeffAoaScale, float Efficiency, float Chord, float ChordDirection, float Span, float SpanDirection)
		: CriticalAoa(CriticalAoa),
		DragCoeffAoaScale(DragCoeffAoaScale),
		Efficiency(Efficiency),
		Chord(Chord), ChordDirection(ChordDirection),
		Span(Span), SpanDirection(SpanDirection)
	{
	}

private:
	virtual TArray<FVector> CalcForces(FVector WindVelocity, FVector BodyForward, FVector BodyRight, FVector BodyUp) override;

	float CriticalAoa;
	virtual float CalcLiftMagnitude(float LiftCoeff, FVector FlowVelocity, FVector Right) const override;
	virtual float CalcLiftCoeff(float Aoa) const override;

	float DragCoeffAoaScale;
	virtual float CalcDragMagnitude(float DragCoeff, FVector FlowVelocity) const override;
	virtual float CalcDragCoeff(float Aoa) const override;

	float Efficiency;
	virtual float CalcInducedDragMagnitude(float InducedDragCoeff, FVector FlowVelocity) const override;
	virtual float CalcInducedDragCoeff(float LiftCoeff, float AspectRatio);
	// Could add camber and other really realistic stuff later (way later)
	float Chord;
	float ChordDirection;
	float Span;
	float SpanDirection;

	float CalcArea() const;
	float CalcAspectRatio(float Area) const;
	virtual FVector CalcPosition(FVector BodyForward, FVector BodyRight, FVector BodyUp) const override;
};
