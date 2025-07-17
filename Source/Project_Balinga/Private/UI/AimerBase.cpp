


#include "UI/AimerBase.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "BalingaControllerBase.h"


void UAimerBase::FollowMouse()
{

	// Use mouse velocity instead, so we can have a square screen scale 
	FVector2D mousePosition = FVector2D::ZeroVector;

	UWidgetLayoutLibrary::GetMousePositionScaledByDPI(GetOwningPlayer(), mousePosition.X, mousePosition.Y);
	
	FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(GetOwningPlayer());

	mousePosition -= viewportSize / 2; // Get rid of offset on Play In Editor window

	FVector2D velocity = (mousePosition - lastMousePosition) / screenScale;

	lastMousePosition = mousePosition;

	FVector2D nextPosition = FVector2D::ZeroVector;

	//FVector2D mousePositionPercentage = mousePosition / viewportSize;

	//nextPosition = mousePositionPercentage * borderRadius;


	//if (mousePosition.GetAbs() * screenScale < (mousePosition.GetAbs()).GetSafeNormal() * borderRadius)
	//{
	//	nextPosition = mousePosition * screenScale;
	//}
	//else
	//{
	//	//nextPosition = mousePosition.GetSafeNormal() * borderRadius;
	//	nextPosition = mousePosition * screenScale;
	//}

	if (Slot && GetOwningPlayer())
	{
		TObjectPtr<UCanvasPanelSlot> AimerSlot = Cast<UCanvasPanelSlot>(Slot);

		GetOwningPlayer()->GetInputMouseDelta(velocity.X, velocity.Y);

		velocity /= screenScale;

		velocity.Y *= -1;

		FVector2D aimerPosition = AimerSlot->GetPosition();

		FVector2D desiredNextPosition = aimerPosition + velocity;

		nextPosition = (desiredNextPosition.Size() > borderRadius) ? desiredNextPosition.GetSafeNormal() * borderRadius : desiredNextPosition;

		UE_LOG(LogTemp, Warning, TEXT("nextPosition: %s"), *nextPosition.ToString());
		UE_LOG(LogTemp, Warning, TEXT("velocity: %s"), *velocity.ToString());


		AimerSlot->SetPosition(nextPosition);
	}
}

void UAimerBase::UpdateWidget()
{
	FollowMouse();
}

void UAimerBase::SetBorderRadius(float newBorderRadius)
{
	borderRadius = newBorderRadius;
}

void UAimerBase::SetScreenScale(float newScreenScale)
{
	screenScale = newScreenScale;
}

