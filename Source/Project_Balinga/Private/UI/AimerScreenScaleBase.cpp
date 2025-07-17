


#include "UI/AimerScreenScaleBase.h"
#include "UI/UiUtils.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include <Blueprint/WidgetLayoutLibrary.h>


void UAimerScreenScaleBase::UpdateWidget()
{
	if (GetWorld() && Slot)
	{
		TObjectPtr<UCanvasPanelSlot> ScreenScaleSlot = Cast<UCanvasPanelSlot>(Slot);
		FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(GetOwningPlayer());
		ScreenScaleSlot->SetSize(viewportSize * screenScale);
		//UE_LOG(LogTemp, Warning, TEXT("Viewport size: %s"), *viewportSize.ToString());
	}

	if (Aimer)
	{
		Aimer->SetScreenScale(screenScale);
	}
}

