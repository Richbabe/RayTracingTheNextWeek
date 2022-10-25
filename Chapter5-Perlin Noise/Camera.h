#pragma once

#include "RTWeekend.h"

class Camera 
{
    public:
        Camera(
            Point3 LookFrom,
            Point3 LookAt,
            Vector3 Up,
            float FOV, // vertical field-of-view in degrees
            float AspectRatio,
            float Aperture,
            float FocusDist,
            float InTime0,
            float InTime1
        ) 
        {
            auto Theta = DegreesToRadians(FOV);
            auto H = tan(Theta / 2.0f);
            auto ViewportHeight = 2.0f * H;
            auto ViewportWidth = AspectRatio * ViewportHeight;

            W = UnitVector(LookFrom - LookAt);
            U = UnitVector(Cross(Up, W));
            V = Cross(W, U);

            Origin = LookFrom;
            Horizontal = FocusDist * ViewportWidth * U;
            Vertical = FocusDist * ViewportHeight * V;
            LowerLeftCorner = Origin - Horizontal / 2.0f - Vertical / 2.0f - FocusDist * W;

            LensRadius = Aperture / 2.0f;

            Time0 = InTime0;
            Time1 = InTime1;
        }

        Ray GetRay(float u, float v) const 
        {
            Vector3 RayDirection = LensRadius * RandomInUnitDisk();
            Vector3 Offset = U * RayDirection.X() + V * RayDirection.Y();

            return Ray(Origin + Offset, 
                    LowerLeftCorner + u * Horizontal + v * Vertical - Origin - Offset,
                    RandomFloat(Time0, Time1)
                );
        }

    private:
        Point3 Origin;
        Point3 LowerLeftCorner;
        Vector3 Horizontal;
        Vector3 Vertical;
        Vector3 U, V, W;
        float LensRadius;
        float Time0, Time1;  // shutter open/close times
};