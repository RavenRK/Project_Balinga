


#include "UI/AimerBase.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "BalingaControllerBase.h"


void UAimerBase::UpdateWidget(float DeltaSeconds)
{
	FollowMouseVelocity(DeltaSeconds);
}

void UAimerBase::FollowMouseVelocity(float DeltaSeconds)
{
	if (Slot && GetOwningPlayer())
	{
		TObjectPtr<UCanvasPanelSlot> AimerSlot = Cast<UCanvasPanelSlot>(Slot);

		FVector2D Velocity;
		GetOwningPlayer()->GetInputMouseDelta(Velocity.X, Velocity.Y);

		Velocity /= ScreenScale;

		Velocity.Y *= -1;

		FVector2D Accel = Velocity - LastVelocity;

		FMath::CriticallyDampedSmoothing(Velocity.X, Accel.X, LastVelocity.X, Accel.X * 1, DeltaSeconds * 1000, XSmoothTime);
		FMath::CriticallyDampedSmoothing(Velocity.Y, Accel.Y, LastVelocity.Y, Accel.Y * 1, DeltaSeconds * 1000, YSmoothTime);

		LastVelocity = Velocity;

		FVector2D AimerPosition = AimerSlot->GetPosition();

		FVector2D DesiredNextPosition = Velocity;

		FVector2D NextPosition = (DesiredNextPosition.Size() > BorderRadius) ? DesiredNextPosition.GetSafeNormal() * BorderRadius : DesiredNextPosition;

		AimerSlot->SetPosition(NextPosition);
	}
}

void UAimerBase::SetBorderRadius(float NewBorderRadius)
{
	BorderRadius = NewBorderRadius;
}

void UAimerBase::SetScreenScale(float NewScreenScale)
{
	ScreenScale = NewScreenScale;
}

FVector2D UAimerBase::GetSlotPosition()
{
	checkf(Slot, TEXT("Aimer canvas slot undefined."));

	TObjectPtr<UCanvasPanelSlot> AimerSlot = Cast<UCanvasPanelSlot>(Slot);

	return AimerSlot->GetPosition();
}

FVector2D UAimerBase::GetSlotPercentPosition()
{
	return GetSlotPosition() / BorderRadius;
}

