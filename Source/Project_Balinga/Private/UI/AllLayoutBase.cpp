


#include "UI/AllLayoutBase.h"
#include "UI/AimerBorderRadiusBase.h"
#include "UI/AImerScreenScaleBase.h"
#include "UI/AimerBase.h"

void UAllLayoutBase::BeginPlay()
{
	SetAimers();

	AimerBorderRadius->UpdateWidget();
	AimerScreenScale->UpdateWidget();
}

FVector2D UAllLayoutBase::GetAimerPosition()
{
	return Aimer->GetSlotPosition();
}

FVector2D UAllLayoutBase::GetAimerPercentPosition()
{
	return Aimer->GetSlotPercentPosition();
}

void UAllLayoutBase::SetAimers()
{
	AimerBorderRadius->SetAimer(Aimer);
	AimerScreenScale->SetAimer(Aimer);
}

void UAllLayoutBase::UpdateAimer(float DeltaSeconds)
{
	Aimer->UpdateWidget(DeltaSeconds);
}



