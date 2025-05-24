#include "BalingaMovementComponentBase.h"


UBalingaMovementComponentBase::UBalingaMovementComponentBase()
{
}

// Check if we're in the given custom movement mode
bool UBalingaMovementComponentBase::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UBalingaMovementComponentBase::PhysFly(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Add root motion code here if we ever decide to use root motions

	// Thrust is applied in the direction the character is facing
	// Drag and gravity will decelerate this
	if (bFlapInput)
	{
		// actorForward only changes in the Z axis by default, we need to allow it to be changed in all during the fly mode
		// We should change the angle of the player to the camera angle basically immediately upon entering flight (for the first time)
		// Subsequent changes in angle will be handled differently, with a weaker rotational velocity

		actorForward = CharacterOwner->GetActorForwardVector();
		Velocity += ((initThrustMagnitude * actorForward) / Mass) * thrustScale * deltaTime;

		bFlapInput = false;
	}

	// Change surfaceArea using angleOfAttack (Velocity vector compared to windVelocity vector)

	// The greater the velocity, surfaceArea and airDensity the more lift and drag
	// Velocity is movement safe
	// We can combine wind vectors if there are multiple acting on balinga at the same time
	FVector baseForce = ((airDensity * FMath::Square(Velocity + windVelocity)) / 2.0f) * surfaceArea;
	
	actorUp = CharacterOwner->GetActorUpVector();
	
	// Lift is always applied in the same perpendicular vector: from balinga's perspective that vector is upwards
	// (Finish explaining) If the bird is normally lift is applied on the z axis, if it's rotated to the right lift acts on the x or y axis,
	// So we take the magnitude of baseForce and just apply it to the normalised up vector
	FVector lift = FMath::Sqrt(FMath::Square(baseForce.X + baseForce.Y + baseForce.Z)) * actorUp * liftScale; 

	// Drag is parallel and negative to baseForce
	FVector drag = baseForce * -1.0f * dragScale;

	Velocity += (lift / Mass) * deltaTime;

	Velocity += (drag / Mass) * deltaTime;

	const FVector Gravity = -GetGravityDirection() * GetGravityZ();

	Velocity += Gravity * deltaTime;

	// Change forward acceleration based on direction of mouse / wasd (something in balingacontroller)

}

void UBalingaMovementComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());
}

bool UBalingaMovementComponentBase::FSavedMove_Balinga::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Balinga* NewBalingaMove = static_cast<FSavedMove_Balinga*>(NewMove.Get());

	if (saved_bFlapInput != NewBalingaMove->saved_bFlapInput)
	{
		return false;
	}
	if (saved_initThrustMagnitude != NewBalingaMove->saved_initThrustMagnitude)
	{
		return false;
	}

	if (saved_angleOfAttack != NewBalingaMove->saved_angleOfAttack)
	{
		return false;
	}
	if (saved_surfaceArea != NewBalingaMove->saved_surfaceArea)
	{
		return false;
	}
	if (saved_airDensity != NewBalingaMove->saved_airDensity)
	{
		return false;
	}

	if (saved_actorForward != NewBalingaMove->saved_actorForward)
	{
		return false;
	}
	if (saved_actorUp != NewBalingaMove->saved_actorUp)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UBalingaMovementComponentBase::FSavedMove_Balinga::Clear()
{
	saved_bFlapInput = false;
	saved_initThrustMagnitude = saved_defaultInitThrustMagnitude;

	saved_angleOfAttack;
	saved_surfaceArea;
	saved_airDensity;

	saved_actorForward;
	saved_actorUp;

	FSavedMove_Character::Clear();
}

uint8 UBalingaMovementComponentBase::FSavedMove_Balinga::GetCompressedFlags() const
{
	return uint8();
}

void UBalingaMovementComponentBase::FSavedMove_Balinga::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
}

void UBalingaMovementComponentBase::FSavedMove_Balinga::PrepMoveFor(ACharacter* C)
{
}
