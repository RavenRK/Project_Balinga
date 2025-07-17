

#pragma once

#include "CoreMinimal.h"
#include "BalingaWidget.h"
#include "AimerBase.h"
#include "AimerScreenScaleBase.generated.h"

class UImage;

UCLASS(Abstract)
/**
 * 
 */
class UAimerScreenScaleBase : public UBalingaWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) float screenScale;


public:
	virtual void UpdateWidget() override;
	UPROPERTY() TObjectPtr<UAimerBase> Aimer;
		
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ScreenScaleImage;
	
};
