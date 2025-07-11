#include "BalingaMovement.h"
#include "Logging/LogMacros.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLoggerUtils.h"
#include "MathUtils.h"


UBalingaMovement::UBalingaMovement()
{

}
void UBalingaMovement::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());

	thrustScale = defaultThrustScale;

	dragScale = defaultDragScale;
	minDragDesiredScale = defaultMinDragDesiredScale;
	dragDesiredScaleScale = defaultDragDesiredScaleScale;

	liftScale = defaultLiftScale;
	minLiftDesiredScale = defaultMinLiftDesiredScale;
	liftDesiredScaleScale = defaultLiftDesiredScaleScale;

	angleOfAttack = defaultAngleOfAttack;
	surfaceArea = defaultSurfaceArea;
	windVelocity = defaultWindVelocity;
	airDensity = defaultAirDensity;

	lastDesiredDifference = FVector::ZeroVector;
	smoothVelocity = FVector::ZeroVector;
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

	FVector flowDirection = (Velocity + windVelocity).GetSafeNormal();
	FVector desiredDifference = CharacterOwner->GetActorForwardVector() - flowDirection;
	FVector desiredDifferenceDirection = desiredDifference.GetSafeNormal();
	float desiredDifferenceDot = FVector::DotProduct(flowDirection, CharacterOwner->GetActorForwardVector());

	// DON'T NEED TO SMOOTH ANYTHING YET
	/*std::tuple<FVector, FVector> data = FMathUtils::SmoothDamp3(lastDesiredDifference, desiredDifferenceDirection, smoothVelocity, smoothTime);
	lastDesiredDifference = desiredDifferenceDirection;
	desiredDifferenceDirection = std::get<0>(data);
	smoothVelocity = std::get<1>(data);*/

	FVector Gravity = -GetGravityDirection() * GetGravityZ();
	Velocity += Gravity * deltaTime;

	FVector baseForce = ((airDensity * FMath::Square((Velocity + windVelocity).GetAbs()) / 2.0f)) * surfaceArea;
	actorUp = CharacterOwner->GetActorUpVector();

	// Lift is applied perpendicular to the velocity's direction and the player's right direction (different from just player up direction)
	FVector lift = baseForce.Size() * FVector::CrossProduct(flowDirection, CharacterOwner->GetActorRightVector()) * liftScale;
	float liftDesiredDot = FVector::DotProduct(desiredDifferenceDirection, lift.GetSafeNormal());
	float liftDesiredScale = liftDesiredDot;

	float trueLiftDesiredScale = liftDesiredScale;
	FVector trueLiftAcceleration = ((lift * trueLiftDesiredScale) / Mass) * deltaTime;

	liftDesiredScale = FMath::Max(liftDesiredScale, minLiftDesiredScale);
	lift = lift - (lift * liftDesiredScaleScale) + (lift * liftDesiredScaleScale * liftDesiredScale);
	FVector liftAcceleration = (lift / Mass) * deltaTime;

	FVector assumedDesiredDifference = (CharacterOwner->GetActorForwardVector() - (liftAcceleration + Velocity + windVelocity).GetSafeNormal());
	FVector assumedDesiredDifferenceDirection = assumedDesiredDifference.GetSafeNormal();
	bool bisAssumedDotDifferentSign = FMath::Sign(assumedDesiredDifferenceDirection.Dot(lift.GetSafeNormal())) != FMath::Sign(liftDesiredScale);
	if (bisAssumedDotDifferentSign)
	{
		float desiredLiftProjectionScale = FVector::DotProduct(desiredDifference * 1, liftAcceleration) / liftAcceleration.Size();
		liftAcceleration *= desiredLiftProjectionScale;
		liftDesiredScale *= desiredLiftProjectionScale;
	}

	Velocity += liftAcceleration;

	// Drag is parallel and negative to velocity, applied last so it can oppose everything
	FVector drag = baseForce.Size() * flowDirection * -1.0f * dragScale;
	float dragDesiredDot = FVector::DotProduct(desiredDifferenceDirection, drag.GetSafeNormal());
	float dragDesiredScale = dragDesiredDot;
	dragDesiredScale = FMath::Max(dragDesiredScale, minDragDesiredScale);
	drag = drag - (drag * dragDesiredScaleScale) + (drag * dragDesiredScaleScale * dragDesiredScale);
	FVector dragAcceleration = (drag / Mass) * deltaTime;
	
	// DOES WORK SMOOTH BUT POSSIBLY NOT WANTED
	//FVector dragAssumedDesiredDifference = (CharacterOwner->GetActorForwardVector() - (dragAcceleration + Velocity + windVelocity).GetSafeNormal());
	//FVector dragAssumedDesiredDifferenceDirection = dragAssumedDesiredDifference.GetSafeNormal();
	//bool bisDragAssumedDotDifferentSign = FMath::Sign(dragAssumedDesiredDifferenceDirection.Dot(Gravity.GetSafeNormal())) != FMath::Sign(dragDesiredScale);
	//if (bisDragAssumedDotDifferentSign)
	//{
	//	float desiredDragProjectionScale = FVector::DotProduct(desiredDifference * 1, dragAcceleration) / dragAcceleration.Size();
	//	dragAcceleration *= desiredDragProjectionScale;
	//	dragDesiredScale *= desiredDragProjectionScale;
	//}

	Velocity += dragAcceleration;

	// Applies input acceleration
	// Use acceleration to change rotation probably
	// CalcVelocity(deltaTime, 0.0f, false, 0.0f);

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	
	SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit); // Actually moves and rotates everything
	
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
	float cylinderRadius = 1.5;
	float arrowHeight = 20;
	float arrowWidth = 15;

	FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + actorForward * thrustScale, cylinderRadius, FColor::Green, "Thrust acceleration", arrowHeight, arrowWidth, FColor::Green, "");
	FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + trueLiftAcceleration * vlogScale, cylinderRadius, FColor::Orange, "True lift acceleration", arrowHeight, arrowWidth, FColor::Orange, "");
	FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + liftAcceleration * vlogScale, cylinderRadius, FColor::Yellow, "Lift acceleration", arrowHeight, arrowWidth, FColor::Yellow, "");
	FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + dragAcceleration * vlogScale, cylinderRadius, FColor::Red, "Drag acceleration", arrowHeight, arrowWidth, FColor::Red, "");
	//FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + Gravity * vlogScale, cylinderRadius, FColor::Blue, "Gravity acceleration", arrowHeight, arrowWidth, FColor::Blue, "");
	FVisualLoggerUtils::DrawVlogArrow(CharacterOwner->GetWorld(), "LogTemp", position, position + desiredDifferenceDirection * 100, cylinderRadius, FColor::Purple, "Desired difference" + desiredDifferenceDirection.ToCompactString(), arrowHeight, arrowWidth, FColor::Purple, "");


	GEngine->AddOnScreenDebugMessage(2, 100.0f, FColor::Green, FString::Printf(TEXT("Velocity: [%s]"), *Velocity.ToCompactString()));
	GEngine->AddOnScreenDebugMessage(3, 100.0f, FColor::Purple, FString::Printf(TEXT("Desired difference: [%s]"), *(FString::SanitizeFloat(desiredDifference.X) + ", " + FString::SanitizeFloat(desiredDifference.Y) + ", " + FString::SanitizeFloat(desiredDifference.Z))));
	GEngine->AddOnScreenDebugMessage(4, 100.0f, FColor::Magenta, FString::Printf(TEXT("Desired difference dot: [%s]"), *FString::SanitizeFloat(desiredDifferenceDot)));
	GEngine->AddOnScreenDebugMessage(5, 100.0f, FColor::Yellow, FString::Printf(TEXT("Lift desired scale: [%s]"), *FString::SanitizeFloat(liftDesiredScale)));
	GEngine->AddOnScreenDebugMessage(6, 100.0f, FColor::Orange, FString::Printf(TEXT("True lift desired scale: [%s]"), *FString::SanitizeFloat(trueLiftDesiredScale)));
	GEngine->AddOnScreenDebugMessage(7, 100.0f, FColor::Red, FString::Printf(TEXT("Drag desired scale: [%s]"), *FString::SanitizeFloat(dragDesiredScale)));

	UE_VLOG_HISTOGRAM(this, "MyGame", Verbose, "Force Accelerations", "Lift", FVector2D(GetWorld()->GetTimeSeconds(), liftAcceleration.Size()));
	UE_VLOG_HISTOGRAM(this, "MyGame", Verbose, "Force Accelerations", "Drag", FVector2D(GetWorld()->GetTimeSeconds(), dragAcceleration.Size()));	

	//UE_LOG(LogTemp, Log, TEXT("Desired difference: %s"), *desiredDifferenceDirection.ToString());

	//UE_LOG(LogTemp, Log, TEXT("Drag direction: %s"), *drag.GetSafeNormal().ToString());
	//UE_LOG(LogTemp, Log, TEXT("Drag dot: %s"), *FString::SanitizeFloat(dragDesiredDot));

	//UE_LOG(LogTemp, Log, TEXT("Lift direction: %s"), *lift.GetSafeNormal().ToString());
	//UE_LOG(LogTemp, Log, TEXT("Lift dot: %s"), *FString::SanitizeFloat(liftDesiredDot));
}

void UBalingaMovement::EnterFly()
{
	CharacterOwner->bUseControllerRotationPitch = true;
	CharacterOwner->bUseControllerRotationRoll = true;

	SetMovementMode(MOVE_Custom, CMOVE_Fly);
}
void UBalingaMovement::ExitFly()
{	
	CharacterOwner->bUseControllerRotationPitch = false;
	CharacterOwner->bUseControllerRotationRoll = false;

	SetMovementMode(MOVE_Falling);
}

void UBalingaMovement::FlapPressed()
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

void UBalingaMovement::LandPressed()
{
	ExitFly();
}
void UBalingaMovement::LandReleased(){}

void UBalingaMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	
	if (bWantsToLand)
	{
		LandPressed();
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

	bWantsToLand = (Flags && FSavedMove_Balinga::FLAG_Land) != 0;
}

bool UBalingaMovement::FSavedMove_Balinga::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Balinga* NewBalingaMove = static_cast<FSavedMove_Balinga*>(NewMove.Get());

	if (saved_bWantsToLand != NewBalingaMove->saved_bWantsToLand)
	{
		return false;
	}

	if (saved_thrustScale != NewBalingaMove->saved_thrustScale)
	{
		return false;
	}

	if (saved_dragScale != NewBalingaMove->saved_dragScale)
	{
		return false;
	}
	if (saved_minDragDesiredScale != NewBalingaMove->saved_minDragDesiredScale)
	{
		return false;
	}
	if (saved_dragDesiredScaleScale != NewBalingaMove->saved_dragDesiredScaleScale)
	{
		return false;
	}

	if (saved_liftScale != NewBalingaMove->saved_liftScale)
	{
		return false;
	}
	if (saved_minLiftDesiredScale != NewBalingaMove->saved_minLiftDesiredScale)
	{
		return false;
	}
	if (saved_liftDesiredScaleScale != NewBalingaMove->saved_liftDesiredScaleScale)
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

	saved_bWantsToLand = false;

	saved_thrustScale = 1;

	saved_dragScale = 1;
	saved_minDragDesiredScale;
	saved_dragDesiredScaleScale = 1;

	saved_liftScale = 1;
	saved_minLiftDesiredScale;
	saved_liftDesiredScaleScale = 1;
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

	saved_bWantsToLand = BalingaMovement->bWantsToLand;

	saved_thrustScale = BalingaMovement->thrustScale;

	saved_dragScale = BalingaMovement->dragScale;
	saved_minDragDesiredScale = BalingaMovement->minDragDesiredScale;
	saved_dragDesiredScaleScale = BalingaMovement->dragDesiredScaleScale;

	saved_liftScale = BalingaMovement->liftScale;
	saved_minLiftDesiredScale = BalingaMovement->minLiftDesiredScale;
	saved_liftDesiredScaleScale = BalingaMovement->liftDesiredScaleScale;

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

	BalingaMovement->bWantsToLand = saved_bWantsToLand;

	BalingaMovement->thrustScale = saved_thrustScale;

	BalingaMovement->dragScale = saved_dragScale;
	BalingaMovement->minDragDesiredScale = saved_minDragDesiredScale;
	BalingaMovement->dragDesiredScaleScale = saved_dragDesiredScaleScale;

	BalingaMovement->liftScale = saved_liftScale;
	BalingaMovement->minLiftDesiredScale = saved_minLiftDesiredScale;
	BalingaMovement->liftDesiredScaleScale = saved_liftDesiredScaleScale;
	// gravity is not ours

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

	if (saved_bWantsToLand) { result |= FLAG_Land; }
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

