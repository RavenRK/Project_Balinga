#pragma once

#include "CoreMinimal.h"
#include "AutoUpdateWidget.h"
#include "AimerBorderRadiusBase.generated.h"

class UImage;
class UAimerBase;

UCLASS(Abstract)
/**
 *
 */
class UAimerBorderRadiusBase : public UAutoUpdateWidget
{
public:
	virtual void UpdateWidget() override;
	void SetAimer(TObjectPtr<UAimerBase> NewAimer);

private:
	GENERATED_BODY()
	UPROPERTY() TObjectPtr<UAimerBase> Aimer;
	UPROPERTY(EditAnywhere) float BorderRadius;

	void SetSlotSize();
	void SyncAimer();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BorderRadiusImage;
};
