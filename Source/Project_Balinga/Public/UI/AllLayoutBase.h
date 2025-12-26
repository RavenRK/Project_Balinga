

#pragma once

#include "CoreMinimal.h"
#include "AutoUpdateWidget.h"
#include "AllLayoutBase.generated.h"

class UAimerBase;
class UAimerBorderRadiusBase;
class UAimerScreenScaleBase;

UCLASS(Abstract)
/**
 * 
 */
class UAllLayoutBase : public UAutoUpdateWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerBase> Aimer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerBorderRadiusBase> AimerBorderRadius;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UAimerScreenScaleBase> AimerScreenScale;

	void BeginPlay();
	void UpdateAimer(float DeltaSeconds);
	FVector2D GetAimerPosition();
	FVector2D GetAimerPercentPosition();

private:
	void SetAimers();
	
};
