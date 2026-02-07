#pragma once

// Specifically not marked as DEBUG_API, it's fine implementing all the classes in the debug module for now
class FDebugDrawer
{
public:
	static void DrawVlogArrow
	(
		const UObject* LogOwner, const FName& CategoryName,
		const FVector& CyStart, const FVector& CyEndCoStart, const float CyRadius, const FColor& CyColor, const FString& CyFmt,
		const float CoLength, const float CoAngle, const FColor& CoColor, const FString& CoFmt
	)
	{
		UE_VLOG_CYLINDER(LogOwner, CategoryName, Verbose, CyStart, CyEndCoStart, CyRadius, CyColor, TEXT("%s"), *CyFmt);

		FVector CoDirection = (CyEndCoStart - CyStart).GetSafeNormal();

		UE_VLOG_CONE(LogOwner, CategoryName, Verbose, CyEndCoStart + (CoDirection * CoLength), -CoDirection, CoLength, CoAngle, CoColor, TEXT("%s"), *CoFmt);
	}

	static void DrawDebugArrow
	(
		const UWorld* World,
		const FVector& CyStart, const FVector& CyEndCoStart, const float CyRadius, int32 CySegments, const FColor& CyColor, 
		bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness,
		const float CoLength, const float CoAngle, const float CoNumSides, const FColor& CoColor
	)
	{
		DrawDebugCylinder(World, CyStart, CyEndCoStart, CyRadius, CySegments, CyColor, bPersistentLines, LifeTime, DepthPriority, Thickness);

		FVector CoDirection = (CyEndCoStart - CyStart).GetSafeNormal();

		DrawDebugCone(World, CyEndCoStart + (CoDirection * CoLength), -CoDirection, CoLength, CoAngle, CoAngle, CoNumSides, CoColor, bPersistentLines, LifeTime, DepthPriority, Thickness);
	}

	static void DrawOnScreenDebugMessage(uint64 Key, float TimeToDisplay, FColor DisplayColor, const FString& DebugMessage, bool bNewerOnTop, const FVector2D& TextScale)
	{
		if (Key > 0)
		{
			GEngine->AddOnScreenDebugMessage(Key, TimeToDisplay, DisplayColor, DebugMessage, bNewerOnTop, TextScale);
		}
	}
};