

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BalingaHudBase.generated.h"

class UAllLayoutBase;
class UAimerBase;

UENUM()
enum class EHudViewMode : uint8
{
	None,
	All,
	Custom1,
	Custom2,
	Custom3,
	Custom4,
	Custom5,
	Custom6
};

/**
 * 
 */
UCLASS(Abstract)
class ABalingaHudBase : public AHUD
{
public:
	void SetViewMode(int viewMode);
	FVector2D GetAimerPosition();
	FVector2D GetAimerPercentPosition();

private:
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly) EHudViewMode CurrentViewMode;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAllLayoutBase> AllLayoutClass;

	// UPROP prevents garbage collection (part 6 - User Interface (B) 8:49)
	UPROPERTY()	TObjectPtr<UAllLayoutBase> AllLayoutWidget;
	
	TObjectPtr<UWorld> World;

	virtual void Tick(float deltaSeconds) override;

protected:
	virtual void BeginPlay() override;
};
