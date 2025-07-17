


#include "UI/BalingaWidget.h"

void UBalingaWidget::UpdateWidget()
{

}

#if WITH_EDITOR

// Called when using the editor designer tool (can be changed during runtime probably)
void UBalingaWidget::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	UpdateWidget();
}
// Called when widget is changed in the editor during runtime
void UBalingaWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateWidget();
}


const FText UBalingaWidget::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "Project Balinga");
}


#endif