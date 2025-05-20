#include "BalingaMovementComponentBase.h"


UBalingaMovementComponentBase::UBalingaMovementComponentBase()
{
}

// Check if we're in the given custom movement mode
bool UBalingaMovementComponentBase::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UBalingaMovementComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	BalingaOwner = Cast<ABalingaBase>(GetOwner());
}
