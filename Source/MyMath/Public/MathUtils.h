

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FMathUtils
{
private:
    static inline const float nearInfinity = 9999999;

public:

    // Returns the next smoothed value after the current value given
    static std::tuple<float, float> SmoothDamp(float currentPos, float targetPos, float currentVelocity, float smoothTime, float maxSpeed = nearInfinity)
    {
        // Based on Game Programming Gems 4 Chapter 1.10
        smoothTime = FMath::Max(0.0001, smoothTime);
        float omega = 2 / smoothTime;

        float exponent = 1 / (1 + omega + 0.48 * omega * omega + 0.235 * omega * omega * omega);
        float change = currentPos - targetPos;
        float originalTo = targetPos;

        // Clamp maximum speed
        float maxChange = maxSpeed * smoothTime;
        change = FMath::Clamp(change, -maxChange, maxChange);
        targetPos = currentPos - change;

        float temp = (currentVelocity + omega * change);
        currentVelocity = (currentVelocity - omega * temp) * exponent;
        float nextPos = targetPos + (change + temp) * exponent;

        // Prevent overshooting (for some reason it was wrong before)
        if ((currentPos < originalTo and nextPos > originalTo) || (currentPos > originalTo and nextPos < originalTo))
        {
            nextPos = originalTo;
            currentVelocity = (nextPos - originalTo);
        }

        return { nextPos, currentVelocity };
    }

    static std::tuple<FVector, FVector> SmoothDamp3(FVector currentPos, FVector targetPos, FVector currentVelocity, FVector smoothTime, FVector maxSpeed = FVector(nearInfinity, nearInfinity, nearInfinity))
    {
        auto [nextPosX, currentVelocityX] = SmoothDamp(currentPos.X, targetPos.X, currentVelocity.X, smoothTime.X, maxSpeed.X);
        auto [nextPosY, currentVelocityY] = SmoothDamp(currentPos.Y, targetPos.Y, currentVelocity.Y, smoothTime.Y, maxSpeed.Y);
        auto [nextPosZ, currentVelocityZ] = SmoothDamp(currentPos.Z, targetPos.Z, currentVelocity.Z, smoothTime.Z, maxSpeed.Z);

        return {FVector(nextPosX, nextPosY, nextPosZ), FVector(currentVelocityX, currentVelocityY, currentVelocityZ)};

    }
};
