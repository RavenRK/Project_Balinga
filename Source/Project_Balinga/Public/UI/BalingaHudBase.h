

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AllLayoutBase.h"
#include "BalingaHudBase.generated.h"

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

private:
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly) EHudViewMode currentViewMode;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAllLayout> AllLayoutClass;

	// UPROP prevents garbage collection (part 6 - User Interface (B) 8:49)
	UPROPERTY()	TObjectPtr<UAllLayout> AllLayoutWidget;
	
	TObjectPtr<UWorld> World;

	int tickCount = 0;
	bool bupdatedScreenScalePastViewportSetupTime = false;

	virtual void Tick(float deltaSeconds) override;

protected:
	virtual void BeginPlay() override;
};
