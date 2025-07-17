

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FUiUtils
{
public:
	// Viewport size not resolution 
	static FVector2D GetGameViewportSize()
	{
		FVector2D Result = FVector2D(1, 1);

		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize( /*out*/Result);
		}

		return Result;
	}
};
