#include "BalingaMovement.h"
#include "Logging/LogMacros.h"
#include "VisualLogger/VisualLogger.h"



UBalingaMovement::UBalingaMovement(){}

// Check if we're in the given custom movement mode

void UBalingaMovement::EnterFly()
{
	CharacterOwner->bUseControllerRotationPitch = true;
	CharacterOwner->bUseControllerRotationRoll = true;

	thrustScale = defaultThrustScale;

	dragScale = defaultDragScale;
	minDragDirectionalScale = defaultMinDragDirectionalScale;
	dragDirectionalScaleScale = defaultDragDirectionalScaleScale;

	liftScale = defaultLiftScale;
	minLiftDirectionalScale = defaultMinLiftDirectionalScale;
	liftDirectionalScaleScale = defaultLiftDirectionalScaleScale;

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
		actorForward = CharacterOwner->GetActorForwardVector();
		Velocity += ((thrustScale * actorForward) / Mass);

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
	
	FVector flowDirection = (Velocity - windVelocity).GetSafeNormal();
	FVector directionalDifference = (CharacterOwner->GetActorForwardVector() - flowDirection).GetSafeNormal();

	FVector baseForce = ((airDensity * FMath::Square((Velocity + windVelocity).GetAbs()) / 2.0f)) * surfaceArea;
	actorUp = CharacterOwner->GetActorUpVector();

	// Lift is applied perpendicular to the velocity's direction and the player's right direction (different from just up direction)
	FVector lift = (baseForce.X + baseForce.Y + baseForce.Z) * FVector::CrossProduct(flowDirection, CharacterOwner->GetActorRightVector()) * liftScale;
	float liftDirectionalDot = FVector::DotProduct(directionalDifference, lift.GetSafeNormal());
	float liftDirectionalScale = liftDirectionalDot * liftDirectionalScaleScale;
	liftDirectionalScale = FMath::Max(liftDirectionalScale, minLiftDirectionalScale);
	lift *= liftDirectionalScale;
	FVector liftAcceleration = (lift / Mass) * deltaTime;
	Velocity += liftAcceleration;

	const FVector Gravity = -GetGravityDirection() * GetGravityZ();
	Velocity += Gravity * deltaTime;

	// Drag is parallel and negative to velocity, applied last so it can oppose everything
	FVector drag = (baseForce.X + baseForce.Y + baseForce.Z) * flowDirection * -1.0f * dragScale;
	float dragDirectionalDot = FVector::DotProduct(directionalDifference, drag.GetSafeNormal());
	float dragDirectionalScale = dragDirectionalDot * dragDirectionalScaleScale;
	dragDirectionalScale = FMath::Max(dragDirectionalScale, minDragDirectionalScale);
	drag *= dragDirectionalScale;
	FVector dragAcceleration = (drag / Mass) * deltaTime;
	Velocity += dragAcceleration;

	// Applies input acceleration
	// Use acceleration to change rotation probably
	// CalcVelocity(deltaTime, 0.0f, false, 0.0f);

	if (Velocity.Size() > 9999)
	{
	//	Velocity = 9999 * Velocity.GetSafeNormal();
	}

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	// Actually moves and rotates everything
	SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit); 

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	FVector position = CharacterOwner->GetActorLocation();
	float vlogScale = 1;

	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + actorForward * thrustScale, FColor::Green, TEXT("Thrust acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + dragAcceleration * vlogScale, FColor::Red, TEXT("Drag acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + liftAcceleration * vlogScale, FColor::Yellow, TEXT("Lift acceleration"));
	UE_VLOG_ARROW(CharacterOwner->GetWorld(), LogTemp, Verbose, position, position + directionalDifference * 100, FColor::Purple, TEXT("Directional difference"));

	GEngine->AddOnScreenDebugMessage(2, 100.0f, FColor(255, 0, 0), FString::Printf(TEXT("Velocity: [%s]"), *Velocity.ToString()));
	GEngine->AddOnScreenDebugMessage(3, 100.0f, FColor::Yellow, FString::Printf(TEXT("Drag directional scale: [%s]"), *FString::SanitizeFloat(dragDirectionalScale)));
	GEngine->AddOnScreenDebugMessage(4, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift directional scale: [%s]"), *FString::SanitizeFloat(liftDirectionalScale)));

	UE_VLOG_HISTOGRAM(this, "MyGame", Verbose, "Force Accelerations", "Lift", FVector2D(GetWorld()->GetTimeSeconds(), liftAcceleration.Size()));
	UE_VLOG_HISTOGRAM(this, "MyGame", Verbose, "Force Accelerations", "Drag", FVector2D(GetWorld()->GetTimeSeconds(), dragAcceleration.Size()));	

	UE_LOG(LogTemp, Log, TEXT("Directional difference: %s"), *directionalDifference.ToString());

	UE_LOG(LogTemp, Log, TEXT("Drag direction: %s"), *drag.GetSafeNormal().ToString());
	UE_LOG(LogTemp, Log, TEXT("Drag dot: %s"), *FString::SanitizeFloat(dragDirectionalDot));

	UE_LOG(LogTemp, Log, TEXT("Lift direction: %s"), *lift.GetSafeNormal().ToString());
	UE_LOG(LogTemp, Log, TEXT("Lift dot: %s"), *FString::SanitizeFloat(liftDirectionalDot));
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
	if (saved_minDragDirectionalScale != NewBalingaMove->saved_minDragDirectionalScale)
	{
		return false;
	}
	if (saved_dragDirectionalScaleScale != NewBalingaMove->saved_dragDirectionalScaleScale)
	{
		return false;
	}

	if (saved_liftScale != NewBalingaMove->saved_liftScale)
	{
		return false;
	}
	if (saved_minLiftDirectionalScale != NewBalingaMove->saved_minLiftDirectionalScale)
	{
		return false;
	}
	if (saved_liftDirectionalScaleScale != NewBalingaMove->saved_liftDirectionalScaleScale)
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
	saved_minDragDirectionalScale;
	saved_dragDirectionalScaleScale = 1;

	saved_liftScale = 1;
	saved_minLiftDirectionalScale;
	saved_liftDirectionalScaleScale = 1;
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
	saved_minDragDirectionalScale = BalingaMovement->minDragDirectionalScale;
	saved_dragDirectionalScaleScale = BalingaMovement->dragDirectionalScaleScale;

	saved_liftScale = BalingaMovement->liftScale;
	saved_minLiftDirectionalScale = BalingaMovement->minLiftDirectionalScale;
	saved_liftDirectionalScaleScale = BalingaMovement->liftDirectionalScaleScale;

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
	BalingaMovement->minDragDirectionalScale = saved_minDragDirectionalScale;
	BalingaMovement->dragDirectionalScaleScale = saved_dragDirectionalScaleScale;

	BalingaMovement->liftScale = saved_liftScale;
	BalingaMovement->minLiftDirectionalScale = saved_minLiftDirectionalScale;
	BalingaMovement->liftDirectionalScaleScale = saved_liftDirectionalScaleScale;
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

