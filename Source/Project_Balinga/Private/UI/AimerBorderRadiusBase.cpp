


#include "UI/AimerBorderRadiusBase.h"
#include "UI/UiUtils.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"


void UAimerBorderRadiusBase::UpdateWidget()
{
	if (GetWorld() && Slot)
	{
		TObjectPtr<UCanvasPanelSlot> BorderRadiusSlot = Cast<UCanvasPanelSlot>(Slot);
		BorderRadiusSlot->SetSize(FVector2D(borderRadius * 2, borderRadius * 2));
		//SetDesiredSizeInViewport(FVector2D(borderRadius, borderRadius));
	}

	if (Aimer)
	{
		Aimer->SetBorderRadius(borderRadius);
	}
}
