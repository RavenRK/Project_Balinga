#pragma once

class FAeroObject;

class TOYPHYSICS_API FAeroStruct
{
	TArray<FVector> LocalObjectOffsets;

public:
	TArray<FAeroObject*> AeroObjects;
	FAeroStruct(const TArray<FAeroObject*>& AeroObjects, const TArray<FVector>& AoOffsets, const FVector& Forward,
		const FVector& Right, const FVector& Up)
		: AeroObjects(AeroObjects),
		LocalObjectOffsets(AoOffsets),
		Forward(Forward),
		Right(Right),
		Up(Up)
	{
	}

	TArray<TArray<FVector>> CalcForcesAndTorques();

private:
	FVector Position = FVector::ZeroVector;
public:
	void SetPos(FVector NewPosition);
	void UpdateObjectPosAndVelFromPosition();
	void UpdateObjectPosFromPosition();
	void SetObjectPositions(TArray<FVector> NewObjectPositions); // Use whenever we want to change ao position without changing velocity
	void SetObjectPositions(TArray<int> ObjectIndices, TArray<FVector> NewObjectPositions); // Use whenever we want to change ao position without changing velocity

private:
	static void SetObjectPositions(TArray<FAeroObject*> ObjectsToEdit, TArray<FVector> NewObjectPositions);

public:
	FVector Forward;
	FVector Right;
	FVector Up;
	void SetOrientation(FVector NewForward, FVector NewRight, FVector NewUp);

	void SetObjectAxisOffsets(TArray<int> ObjectIndices, TArray<TArray<float>> NewAoAxisOffsets);
	void SetObjectAxisOffsets(TArray<TArray<float>> NewObjectAxisOffsets);
	static void SetObjectAxisOffsets(TArray<FAeroObject*> AeroObjectsToEdit, TArray<TArray<float>> NewObjectAxisOffsets);

	FVector CalcWorldObjectOffset(float ObjectIndice) const;

private:
	float Mass;
	float MomentInertia;
};
