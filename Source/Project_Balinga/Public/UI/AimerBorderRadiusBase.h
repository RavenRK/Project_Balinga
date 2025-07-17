#pragma once

#include "CoreMinimal.h"
#include "BalingaWidget.h"
#include"AimerBase.h"
#include "AimerBorderRadiusBase.generated.h"

class UImage;

UCLASS(Abstract)
/**
 *
 */
class UAimerBorderRadiusBase : public UBalingaWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) float borderRadius;

public:
	UPROPERTY() TObjectPtr<UAimerBase> Aimer;
	virtual void UpdateWidget() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BorderRadiusImage;
};
