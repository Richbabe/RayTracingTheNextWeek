#pragma once

#include "Vector3.h"

class Ray 
{
    public:
        Ray() {}
        Ray(const Point3& InOrigin, const Vector3& InDir, float InTime = 0.0f)
            : Origin(InOrigin)
            , Dir(InDir)
            , Time(InTime)
        {}

        inline Point3 GetOrigin() const  { return Origin; }
        inline Vector3 GetDirection() const { return Dir; }
        inline float GetTime() const { return Time; }

        Point3 At(float t) const 
        {
            return Origin + t * Dir;
        }

    public:
        Point3 Origin;
        Vector3 Dir;
        float Time;
};
