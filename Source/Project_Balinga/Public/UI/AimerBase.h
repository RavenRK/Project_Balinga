

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
	virtual void UpdateWidget(float DeltaSeconds);

	void SetBorderRadius(float NewBorderRadius);
	void SetScreenScale(float NewScreenScale);

	FVector2D GetSlotPosition();
	FVector2D GetSlotPercentPosition();

private:
	GENERATED_BODY()

	float BorderRadius;
	float ScreenScale;
	FVector2D LastVelocity;

	UPROPERTY(EditAnywhere) float XSmoothTime;
	UPROPERTY(EditAnywhere) float YSmoothTime;

	void FollowMouseVelocity(float DeltaSeconds);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> AimerImage;
};
