#pragma once

#include "RTWeekend.h"

class AABB 
{
    public:
        AABB() {}
        AABB(const Point3& A, const Point3& B) { Minimum = A; Maximum = B;}

        Point3 Min() const {return Minimum; }
        Point3 Max() const {return Maximum; }

        bool Hit(const Ray& InRay, float tMin, float tMax) const;

        Point3 Minimum;
        Point3 Maximum;
};

inline bool AABB::Hit(const Ray& InRay, float tMin, float tMax) const 
{
    for (int i = 0; i < 3; i++) 
    {
        auto InvD = 1.0f / InRay.GetDirection()[i];
        auto t0 = (Minimum[i] - InRay.GetOrigin()[i]) * InvD;
        auto t1 = (Maximum[i] - InRay.GetOrigin()[i]) * InvD;

        // Negative direction
        if(InvD < 0.0f)
        {
            std::swap(t0, t1);
        }

        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin)
        {
            return false;
        }
    }
    return true;
}

AABB SurroundingBox(const AABB& Box0, const AABB& Box1) 
{
    Point3 Small(fmin(Box0.Min().X(), Box1.Min().X()),
                 fmin(Box0.Min().Y(), Box1.Min().Y()),
                 fmin(Box0.Min().Z(), Box1.Min().Z()));

    Point3 Big(fmax(Box0.Max().X(), Box1.Max().X()),
               fmax(Box0.Max().Y(), Box1.Max().Y()),
               fmax(Box0.Max().Z(), Box1.Max().Z()));

    return AABB(Small,Big);
}