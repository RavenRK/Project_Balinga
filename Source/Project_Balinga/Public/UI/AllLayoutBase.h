

#pragma once

#include "CoreMinimal.h"
#include "BalingaWidget.h"
#include "AimerBase.h"
#include "AimerBorderRadiusBase.h"
#include "AimerScreenScaleBase.h"
#include "AllLayoutBase.generated.h"

UCLASS(Abstract)
/**
 * 
 */
class UAllLayout : public UBalingaWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerBase> Aimer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerBorderRadiusBase> AimerBorderRadius;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerScreenScaleBase> AimerScreenScale;
	
};
