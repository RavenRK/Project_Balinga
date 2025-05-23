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
	// Drag and gravity will decelerate this
	if (safe_bFlapInput)
	{
		// actorForward only changes in the Z axis by default, we need to allow it to be changed in all during the fly mode
		safe_actorForward = CharacterOwner->GetActorForwardVector();
		Velocity += ((safe_flapInitThrust / Mass) * safe_actorForward) * safe_thrustScale * deltaTIme;

		safe_bFlapInput = false;
	}

	// Change surfaceArea using angleOfAttack (Velocity vector compared to windVelocity vector)

	// Velocity is movement safe
	FVector baseForce = ((safe_airDensity * FMath::Square(Velocity + safe_windVelocity)) / 2.0f) * safe_surfaceArea;
		
	// Lift is one of the perpendicular vectors to baseForce
	// The greater the velocity, surfaceArea and airDensity the more lift and drag
	FVector lift = baseForce * safe_liftScale; // Find out how to get the right perpendicular 3d vector

	// Drag is parallel and negative to baseForce
	FVector drag = baseForce * -1.0f * safe_dragScale;

	Velocity += (lift / Mass) * deltaTIme;

	Velocity += (drag / Mass) * deltaTIme;

	// Apply gravity
}

void UBalingaMovementComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());
}

