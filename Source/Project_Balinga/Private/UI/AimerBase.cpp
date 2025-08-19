


#include "UI/AimerBase.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "BalingaControllerBase.h"

void UAimerBase::UpdateWidget()
{
	FollowMouse();
}

void UAimerBase::FollowMouse()
{
	if (Slot && GetOwningPlayer())
	{
		TObjectPtr<UCanvasPanelSlot> AimerSlot = Cast<UCanvasPanelSlot>(Slot);

		FVector2D Velocity;
		GetOwningPlayer()->GetInputMouseDelta(Velocity.X, Velocity.Y);

		Velocity /= ScreenScale;

		Velocity.Y *= -1;

		FVector2D AimerPosition = AimerSlot->GetPosition();

		FVector2D DesiredNextPosition = AimerPosition + Velocity;

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
	checkf(Slot, TEXT("Slot undefined."));

	TObjectPtr<UCanvasPanelSlot> AimerSlot = Cast<UCanvasPanelSlot>(Slot);

	return AimerSlot->GetPosition();
}

