

#pragma once

#include "CoreMinimal.h"
#include "AutoUpdateWidget.h"
#include "AimerBase.generated.h"

class UImage;
class ABalingaControllerBase;

UCLASS(Abstract)
class UAimerBase : public UAutoUpdateWidget
{
public:
	virtual void UpdateWidget() override;

	void SetBorderRadius(float NewBorderRadius);
	void SetScreenScale(float NewScreenScale);

	FVector2D GetSlotPosition();

private:
	GENERATED_BODY()

	float BorderRadius;
	float ScreenScale;

	void FollowMouse();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> AimerImage;
};
