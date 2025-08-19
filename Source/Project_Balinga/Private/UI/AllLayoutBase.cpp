


#include "UI/AllLayoutBase.h"
#include "UI/AimerBorderRadiusBase.h"
#include "UI/AImerScreenScaleBase.h"
#include "UI/AimerBase.h"

void UAllLayoutBase::BeginPlay()
{
	SetAimers();

	AimerBorderRadius->UpdateWidget();
	AimerScreenScale->UpdateWidget();
	Aimer->UpdateWidget();
}

FVector2D UAllLayoutBase::GetAimerPosition()
{
	return Aimer->GetSlotPosition();
}

void UAllLayoutBase::SetAimers()
{
	AimerBorderRadius->SetAimer(Aimer);
	AimerScreenScale->SetAimer(Aimer);
}

void UAllLayoutBase::UpdateAimer()
{
	Aimer->UpdateWidget();
}



