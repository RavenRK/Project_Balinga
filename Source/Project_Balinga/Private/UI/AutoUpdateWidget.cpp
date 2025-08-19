


#include "UI/AutoUpdateWidget.h"

void UAutoUpdateWidget::UpdateWidget()
{

}

#if WITH_EDITOR

// Called when using the editor designer tool (can be changed during runtime probably)
void UAutoUpdateWidget::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	UpdateWidget();
}
// Called when widget is changed in the editor during runtime
void UAutoUpdateWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}


const FText UAutoUpdateWidget::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "Project Balinga");
}


#endif