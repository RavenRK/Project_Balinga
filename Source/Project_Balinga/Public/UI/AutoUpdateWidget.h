

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AutoUpdateWidget.generated.h"

UCLASS()
/**
 * 
 */
class UAutoUpdateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Runs every time a widget property is changed in-Editor.
	virtual void UpdateWidget();
	
private:
#if WITH_EDITOR
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	virtual const FText GetPaletteCategory() override;
#endif

};
