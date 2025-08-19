


#include "UI/AimerScreenScaleBase.h"
#include "UI/UiUtils.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "UI/AimerBase.h"

bool UAimerScreenScaleBase::Initialize()
{
	FViewport::ViewportResizedEvent.AddUObject(this, &UAimerScreenScaleBase::SyncViewportSize); // Do for full screen as well, and check 

	return Super::Initialize();
}

void UAimerScreenScaleBase::UpdateWidget()
{
	SyncAimer();

	SetSlotSize();
}

void UAimerScreenScaleBase::SyncViewportSize(FViewport* Viewport, uint32 Val)
{
	ViewportSize = Viewport->GetSizeXY();
	SetSlotSize();
}


void UAimerScreenScaleBase::SetSlotSize()
{
	if (GetWorld() && Slot)
	{
		TObjectPtr<UCanvasPanelSlot> ScreenScaleSlot = Cast<UCanvasPanelSlot>(Slot);
		ScreenScaleSlot->SetSize(ViewportSize * ScreenScale);
	}
}

void UAimerScreenScaleBase::SetAimer(TObjectPtr<UAimerBase> newAimer)
{
	Aimer = newAimer;
}

void UAimerScreenScaleBase::SyncAimer()
{
	if (Aimer)
	{
		Aimer->SetScreenScale(ScreenScale);
	}
}