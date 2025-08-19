

#pragma once

#include "CoreMinimal.h"
#include "AutoUpdateWidget.h"
#include "AimerScreenScaleBase.generated.h"

class UImage;
class UAimerBase;

UCLASS(Abstract)
/**
 * 
 */
class UAimerScreenScaleBase : public UAutoUpdateWidget
{
public:
	virtual bool Initialize() override;
	void UpdateWidget();
	void SetAimer(TObjectPtr<UAimerBase> NewAimer);
	
private:
	GENERATED_BODY()
	UPROPERTY() TObjectPtr<UAimerBase> Aimer;
	void SyncAimer();

	UPROPERTY(EditAnywhere) float ScreenScale;
	FVector2D ViewportSize;
	void SyncViewportSize(FViewport* Viewport, uint32 Val);
	void SetSlotSize();


protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ScreenScaleImage;
	
};
