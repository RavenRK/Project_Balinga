

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BalingaWidget.generated.h"

UCLASS()
/**
 * 
 */
class UBalingaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateWidget();
	
private:
#if WITH_EDITOR
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	virtual const FText GetPaletteCategory() override;
#endif

};
