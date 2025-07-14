#pragma once

// Specifically not marked as DEBUG_API, it's fine implementing all the classes in the debug module for now
class FVisualLoggerUtils
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
};