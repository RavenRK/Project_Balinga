#include "AeroStruct.h"
#include "ForceAndTorque.h"
#include "AeroObject.h"

TArray<TArray<FVector>> FAeroStruct::CalcForcesAndTorques()
{
	TArray<TArray<FVector>> ForcesAndTorquesAtPosition;
	ForcesAndTorquesAtPosition.Init({ FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector }, AeroObjects.Num());
	TArray<FVector> Forces;
	Forces.Init(FVector::ZeroVector, 3);
	TArray<FVector> Torques;
	Torques.Init(FVector::ZeroVector, 3);

	for (int i = 0; i < AeroObjects.Num(); i++)
	{
		Forces = AeroObjects[i]->CalcForces(FVector::ZeroVector, Forward, Right, Up);

		for (int j = 0; j < Forces.Num(); j++)
		{
			Torques[j] = FTorque::CalcTorqueFromForceAtPos(Forces[j], CalcWorldObjectOffset(i) * 0.000012);
		}

		ForcesAndTorquesAtPosition[i] = { Forces[0], Forces[1], Forces[2], Torques[0], Torques[1], Torques[2] };
	}

	return ForcesAndTorquesAtPosition;
}

void FAeroStruct::UpdateObjectPosAndVelFromPosition()
{
	for (int i = 0; i < AeroObjects.Num(); i++)
	{
		FVector NewPosition = AeroObjects[i]->CalcPosition(Forward, Right, Up) + CalcWorldObjectOffset(i) + Position;
		AeroObjects[i]->SetPosAndVel(NewPosition);
	}
}
void FAeroStruct::UpdateObjectPosFromPosition()
{
	for (int i = 0; i < AeroObjects.Num(); i++)
	{
		AeroObjects[i]->SetPos(AeroObjects[i]->CalcPosition(Forward, Right, Up) + CalcWorldObjectOffset(i) + Position);
	}
}

void FAeroStruct::SetPos(FVector NewPosition)
{
	Position = NewPosition;
}

void FAeroStruct::SetObjectPositions(TArray<int> ObjectIndices, TArray<FVector> NewObjectPositions)
{
	TArray<FAeroObject*> AeroObjectsToEdit;

	for (int i = 0; i < ObjectIndices.Num(); i++)
	{
		AeroObjectsToEdit[i] = AeroObjects[ObjectIndices[i]];
	}

	SetObjectPositions(AeroObjectsToEdit, NewObjectPositions);
}
void FAeroStruct::SetObjectPositions(TArray<FVector> NewObjectPositions)
{
	SetObjectPositions(AeroObjects, NewObjectPositions);
}
void FAeroStruct::SetObjectPositions(TArray<FAeroObject*> ObjectsToEdit, TArray<FVector> NewObjectPositions)
{
	if (ObjectsToEdit.Num() != NewObjectPositions.Num())
	{
		UE_LOG(LogTemp, Fatal, TEXT("Aero object array not the same number as aero object positions"));
		return;
	}

	for (int i = 0; i < ObjectsToEdit.Num(); i++)
	{
		ObjectsToEdit[i]->SetPos(NewObjectPositions[i]);
	}
}


void FAeroStruct::SetOrientation(FVector NewForward, FVector NewRight, FVector NewUp)
{
	// if (NewForward.Cross(NewUp) != NewRight)
	// {
	// 	UE_LOG(LogTemp, Fatal, TEXT("Aero object array, not the same number as aero object positions"));
	// 	return;
	// }

	Forward = NewForward;
	Right = NewRight;
	Up = NewUp;
}

void FAeroStruct::SetObjectAxisOffsets(TArray<int> ObjectIndices, TArray<TArray<float>> NewAoAxisOffsets)
{
	TArray<FAeroObject*> AeroObjectsToEdit;
	AeroObjectsToEdit.Init(new FAeroObject(), ObjectIndices.Num());

	for (int i = 0; i < ObjectIndices.Num(); i++)
	{
		AeroObjectsToEdit[i] = AeroObjects[ObjectIndices[i]];
	}

	SetObjectAxisOffsets(AeroObjectsToEdit, NewAoAxisOffsets);
}
void FAeroStruct::SetObjectAxisOffsets(TArray<TArray<float>> NewObjectAxisOffsets)
{
	SetObjectAxisOffsets(AeroObjects, NewObjectAxisOffsets);
}
void FAeroStruct::SetObjectAxisOffsets(TArray<FAeroObject*> AeroObjectsToEdit, TArray<TArray<float>> NewObjectAxisOffsets)
{
	// if (AeroObjectsToEdit.Num() != NewAoAxisOffsets.Num())
	// {
	// 	UE_LOG(LogTemp, Fatal, TEXT("Aero object array not the same number as aero object axis offsets"));
	// 	return;
	// }

	for (int i = 0; i < AeroObjectsToEdit.Num(); i++)
	{
		AeroObjectsToEdit[i]->SetIncidence(NewObjectAxisOffsets[i][0]);
		AeroObjectsToEdit[i]->SetHedral(NewObjectAxisOffsets[i][1]);
		AeroObjectsToEdit[i]->SetSweep(NewObjectAxisOffsets[i][2]);
	}
}

FVector FAeroStruct::CalcWorldObjectOffset(float ObjectIndice) const
{
	return LocalObjectOffsets[ObjectIndice].X * Forward + LocalObjectOffsets[ObjectIndice].Y * Right + LocalObjectOffsets[ObjectIndice].Z * Up;
}
