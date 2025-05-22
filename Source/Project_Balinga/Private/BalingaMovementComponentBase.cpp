#include "BalingaMovementComponentBase.h"


UBalingaMovementComponentBase::UBalingaMovementComponentBase()
{
}

// Check if we're in the given custom movement mode
bool UBalingaMovementComponentBase::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UBalingaMovementComponentBase::PhysFly(float deltaTIme, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Add root motion code here if we ever decide to use root motions

	// Thrust is applied in the direction the character is facing
	// Unsure how deceleration works
	if (safe_bFlapInput)
	{
		// actorForward only changes in the Z axis by default, we need to allow it to be changed in all during the fly mode
		safe_actorForward = CharacterOwner->GetActorForwardVector();
		Velocity += ((safe_flapInitThrust / Mass) * safe_actorForward) * safe_thrustScale * deltaTIme;

		safe_bFlapInput = false;
	}


	safe_horizontalVelocity = FMath::Sqrt(FMath::Square(Velocity.X) + FMath::Square(Velocity.Y));

	// baseForce and airFlowVelocity might need to become vectors, depending on how we calculate drag 
	float baseForce = ((safe_airDensity * (safe_horizontalVelocity + safe_airflowVelocity)) / 2.0f) * safe_surfaceArea;

	// Need to somehow factor in angleOfAttack (actorForwardVector, might get rid of either one)
	// The closer to the airFlowDirection the greater maybe. But then upwards aiming wouldn't lift you as much
	float lift = baseForce * safe_liftScale;

	float drag = baseForce * safe_dragScale;

	Velocity += (lift / Mass) * FVector::UpVector * deltaTIme; // Lift is always applied upwards (change if we add changing gravity direction)

	// What direction should drag apply in? Both horizontal axes? Only one? The one airFlowVelocity is referring to?
	Velocity += (drag / Mass) * safe_actorForward * deltaTIme; 
}

void UBalingaMovementComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());
}

