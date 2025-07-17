


#include "UI/BalingaHudBase.h"

void ABalingaHudBase::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	checkf(World, TEXT("Failed to get world reference."));	

	checkf(AllLayoutClass, TEXT("Failed to get AllLayoutClass reference."));

	AllLayoutWidget = CreateWidget<UAllLayout>(GetOwningPlayerController(), AllLayoutClass);
	AllLayoutWidget->AddToViewport();
	AllLayoutWidget->SetVisibility(ESlateVisibility::Visible);

	AllLayoutWidget->AimerBorderRadius->Aimer = AllLayoutWidget->Aimer;
	AllLayoutWidget->AimerScreenScale->Aimer = AllLayoutWidget->Aimer;

	AllLayoutWidget->Aimer->UpdateWidget();
	AllLayoutWidget->AimerBorderRadius->UpdateWidget();
	AllLayoutWidget->AimerScreenScale->UpdateWidget();
}

void ABalingaHudBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AllLayoutWidget->Aimer->UpdateWidget();

	if (!bupdatedScreenScalePastViewportSetupTime)
	{
		if (tickCount < 4)
		{
			tickCount++;
		}
		else
		{
			AllLayoutWidget->AimerScreenScale->UpdateWidget();
			bupdatedScreenScalePastViewportSetupTime = true;
		}
	}
}
