#include "UI/BalingaHudBase.h"
#include "UI/AllLayoutBase.h"

void ABalingaHudBase::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	checkf(World, TEXT("Failed to get world reference."));

	checkf(AllLayoutClass, TEXT("Failed to get AllLayoutClass reference."));

	AllLayoutWidget = CreateWidget<UAllLayoutBase>(GetOwningPlayerController(), AllLayoutClass);
	AllLayoutWidget->AddToViewport();
	AllLayoutWidget->SetVisibility(ESlateVisibility::Visible);

	AllLayoutWidget->BeginPlay();
}

FVector2D ABalingaHudBase::GetAimerPosition()
{
	float test = 2.0f;

	if (AllLayoutWidget != nullptr)
	{
		AllLayoutWidget->UpdateAimer();
		return AllLayoutWidget->GetAimerPosition();
	}
	else 
	{
		return FVector2D::ZeroVector;
	}
}

void ABalingaHudBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AllLayoutWidget->UpdateAimer();
}
