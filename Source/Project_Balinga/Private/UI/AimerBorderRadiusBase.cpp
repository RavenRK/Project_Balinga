


#include "UI/AimerBorderRadiusBase.h"
#include "UI/UiUtils.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/AimerBase.h"


void UAimerBorderRadiusBase::UpdateWidget()
{
	SyncAimer();

	SetSlotSize();
}

void UAimerBorderRadiusBase::SetSlotSize()
{
	if (GetWorld() && Slot)
	{
		TObjectPtr<UCanvasPanelSlot> BorderRadiusSlot = Cast<UCanvasPanelSlot>(Slot);
		BorderRadiusSlot->SetSize(FVector2D(BorderRadius * 2, BorderRadius * 2));
	}
}

void UAimerBorderRadiusBase::SyncAimer()
{
	if (Aimer)
	{
		Aimer->SetBorderRadius(BorderRadius);
	}
}

void UAimerBorderRadiusBase::SetAimer(TObjectPtr<UAimerBase> NewAimer)
{
	Aimer = NewAimer;
}


