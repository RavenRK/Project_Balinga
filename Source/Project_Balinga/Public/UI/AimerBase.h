

#pragma once

#include "CoreMinimal.h"
#include "BalingaWidget.h"
#include "AimerBase.generated.h"

class UImage;
class ABalingaControllerBase;

UCLASS(Abstract)
class UAimerBase : public UBalingaWidget
{

	GENERATED_BODY()

	float borderRadius;
	float screenScale;

	FVector2D lastMousePosition;

	TObjectPtr<ABalingaControllerBase> BalingaController;

	void FollowMouse();

public:
	virtual void UpdateWidget() override;

	void SetBorderRadius(float newBorderRadius);
	void SetScreenScale(float newScreenScale);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> AimerImage;
};
