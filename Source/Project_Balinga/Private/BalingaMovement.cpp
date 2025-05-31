#include "BalingaMovement.h"
#include "Logging/LogMacros.h"
#include "VisualLogger/VisualLogger.h"



UBalingaMovement::UBalingaMovement()
{
}

// Check if we're in the given custom movement mode

void UBalingaMovement::EnterFly()
{
	CharacterOwner->bUseControllerRotationPitch = true;
	CharacterOwner->bUseControllerRotationRoll = true;

	thrustScale = defaultThrustScale;
	dragScale = defaultDragScale;
	liftScale = defaultLiftScale;

	angleOfAttack = defaultAngleOfAttack;
	surfaceArea = defaultSurfaceArea;
	windVelocity = defaultWindVelocity;
	airDensity = defaultAirDensity;

	SetMovementMode(MOVE_Custom, CMOVE_Fly);
}
void UBalingaMovement::ExitFly()
{
	CharacterOwner->bUseControllerRotationPitch = false;
	CharacterOwner->bUseControllerRotationRoll = false;
	
	SetMovementMode(MOVE_Falling);
}
void UBalingaMovement::DoFlap()
{
	// Thrust is applied in the direction the character is facing
	// Drag and gravity will decelerate this
	if (CharacterOwner->bPressedJump)
	{
		// actorForward only changes in the Z axis by default, we need to allow it to be changed in all during the fly mode
		// We should change the angle of the player to the camera angle basically immediately upon entering flight (for the first time)
		// Subsequent changes in angle will be handled differently, with a weaker rotational velocity

		actorForward = CharacterOwner->GetActorForwardVector();
		Velocity += ((thrustScale * actorForward) / Mass);
		UE_LOG(LogTemp, Log, TEXT("Forward vector: %s"), *actorForward.ToString());
		UE_LOG(LogTemp, Log, TEXT("Thrust scale: %s"), *FString::SanitizeFloat(thrustScale));

		CharacterOwner->bPressedJump = false;
	}
}

void UBalingaMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Fly:
		PhysFly(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode."))
	}
}
void UBalingaMovement::PhysFly(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Add root motion code here if we ever decide to use root motions

	// Change surfaceArea using angleOfAttack (Velocity vector compared to windVelocity vector)

	// The greater the velocity, surfaceArea and airDensity the more lift and drag
	// Velocity is movement safe
	// We can combine wind vectors if there are multiple acting on balinga at the same time
	FVector baseForce = ((airDensity * (Velocity + windVelocity).GetAbs() / 2.0f)) * surfaceArea;
	
	actorUp = CharacterOwner->GetActorUpVector();
	
	// Lift is always applied in the same perpendicular vector: from balinga's perspective that vector is upwards
	// (Finish explaining) If the bird is normally lift is applied on the z axis, if it's rotated to the right lift acts on the x or y axis,
	// So we take the magnitude of baseForce and just apply it to the normalised up vector
	FVector lift = (baseForce.X + baseForce.Y + baseForce.Z) * actorUp * liftScale; 
	FVector liftAcceleration = (lift / Mass) * deltaTime;
	Velocity += liftAcceleration;

	// Drag is parallel and negative to baseForce
	FVector drag = (baseForce.X + baseForce.Y + baseForce.Z) * Velocity.GetSafeNormal() * -1.0f * dragScale;
	FVector dragAcceleration = (drag / Mass) * deltaTime;
	Velocity += dragAcceleration;

	const FVector Gravity = -GetGravityDirection() * GetGravityZ();

	Velocity += Gravity * deltaTime;

	UE_LOG(LogTemp, Log, TEXT("Lift: %s"), *lift.ToString());
	UE_LOG(LogTemp, Log, TEXT("Drag: %s"), *drag.ToString());

	// Applies input acceleration
	// Will probably need to change some variables inside this function
	// CalcVelocity(deltaTime, 0.0f, false, 0.0f);

	FVector position = CharacterOwner->GetActorLocation();
	float vlogScale = 1;



	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	UE_LOG(LogTemp, Log, TEXT("Velocity1: %s"), *Velocity.ToString());
	// Actually moves and rotates everything
	SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit); 
	UE_LOG(LogTemp, Log, TEXT("Velocity2: %s"), *Velocity.ToString());

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + dragAcceleration * vlogScale, FColor::Red, TEXT("Drag acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + Gravity * vlogScale, FColor::Blue , TEXT("Gravity acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + actorForward * thrustScale, FColor::Green, TEXT("Thrust acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + (Velocity - liftAcceleration - dragAcceleration - Gravity) * vlogScale, FColor::Purple, TEXT("Other acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + baseForce, FColor::Cyan, TEXT("Base force"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + liftAcceleration * vlogScale, FColor::Yellow, TEXT("Lift acceleration"));
}

void UBalingaMovement::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());
}
void UBalingaMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (CharacterOwner)
	{

	}
}
bool UBalingaMovement::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

FNetworkPredictionData_Client* UBalingaMovement::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)
	
	if (ClientPredictionData == nullptr)
	{
		UBalingaMovement* MutableThis = const_cast<UBalingaMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Balinga(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	
	return ClientPredictionData;
}
void UBalingaMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

bool UBalingaMovement::FSavedMove_Balinga::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Balinga* NewBalingaMove = static_cast<FSavedMove_Balinga*>(NewMove.Get());

	if (saved_thrustScale != NewBalingaMove->saved_thrustScale)
	{
		return false;
	}
	if (saved_dragScale != NewBalingaMove->saved_dragScale)
	{
		return false;
	}
	if (saved_liftScale != NewBalingaMove->saved_liftScale)
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
void UBalingaMovement::FSavedMove_Balinga::Clear()
{
	Super::Clear();

	saved_thrustScale = 1;
	saved_dragScale = 1;
	saved_liftScale = 1;
	// gravityScale is not ours

	saved_angleOfAttack = 1;
	saved_surfaceArea = 1;
	saved_windVelocity = FVector::ZeroVector;
	saved_airDensity = 1;

	saved_actorForward = FVector::ZeroVector;
	saved_actorUp = FVector::ZeroVector;
}
void UBalingaMovement::FSavedMove_Balinga::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UBalingaMovement* BalingaMovement = Cast<UBalingaMovement>(C->GetCharacterMovement());

	saved_thrustScale = BalingaMovement->thrustScale;
	saved_dragScale = BalingaMovement->dragScale;
	saved_liftScale = BalingaMovement->liftScale;
	// gravityScale is not ours

	saved_angleOfAttack = BalingaMovement->angleOfAttack;
	saved_surfaceArea = BalingaMovement->surfaceArea;
	saved_windVelocity = BalingaMovement->windVelocity;
	saved_airDensity = BalingaMovement->airDensity;

	saved_actorForward = BalingaMovement->actorForward;
	saved_actorUp = BalingaMovement->actorUp;
}
void UBalingaMovement::FSavedMove_Balinga::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UBalingaMovement* BalingaMovement = Cast<UBalingaMovement>(C->GetCharacterMovement());

	BalingaMovement->thrustScale = saved_thrustScale;
	BalingaMovement->dragScale = saved_dragScale;
	BalingaMovement->liftScale = saved_liftScale;
	// gravity is not ourse

	BalingaMovement->angleOfAttack = saved_angleOfAttack;
	BalingaMovement->surfaceArea = saved_surfaceArea;
	BalingaMovement->windVelocity = saved_windVelocity;
	BalingaMovement->airDensity = saved_airDensity;

	BalingaMovement->actorForward = saved_actorForward;
	BalingaMovement->actorUp = saved_actorUp;
}
uint8 UBalingaMovement::FSavedMove_Balinga::GetCompressedFlags() const
{
	uint8 result = Super::GetCompressedFlags();

	return result;
}

// Idk why not declaring or defining it at all isn't an option, the guy did it so I shall too
UBalingaMovement::FNetworkPredictionData_Client_Balinga::FNetworkPredictionData_Client_Balinga(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}
FSavedMovePtr UBalingaMovement::FNetworkPredictionData_Client_Balinga::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Balinga());
}

