#pragma once

#include "RTWeekend.h"
#include "Hittable.h"

class MovingSphere : public Hittable 
{
    public:
        MovingSphere() {}
        MovingSphere(
            const Point3& Cen0, const Point3& Cen1, float InTime0, float InTime1, float InRadius, shared_ptr<Material> m)
            : Center0(Cen0), Center1(Cen1), Time0(InTime0), Time1(InTime1), Radius(InRadius), Material(m)
        {};

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        Point3 Center(float time) const;

    public:
        Point3 Center0, Center1;
        float Time0, Time1;
        float Radius;
        shared_ptr<Material> Material;
};

Point3 MovingSphere::Center(float Time) const 
{
    return Center0 + ((Time - Time0) / (Time1 - Time0)) * (Center1 - Center0);
}

bool MovingSphere::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    Vector3 OC = InRay.GetOrigin() - Center(InRay.GetTime());
    auto a = InRay.GetDirection().LengthSquared();
    auto Half_b = Dot(OC, InRay.GetDirection());
    auto c = Dot(OC, OC) - Radius * Radius;
    auto Discriminant = Half_b * Half_b - a * c;

    // Not hit
    if(Discriminant < 0.0f)
    {
        return false;
    }

    auto sqrtd = sqrt(Discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto Root = (-Half_b - sqrtd) / a;
    if (Root < tMin || tMax < Root) 
    {
        Root = (-Half_b + sqrtd) / a;
        if (Root < tMin || tMax < Root)
        {
            return false;
        } 
    }

    Record.t = Root;
    Record.p = InRay.At(Root); 

    Vector3 OutwardNormal = (Record.p - Center(InRay.GetTime())) / Radius;
    Record.SetFaceNormal(InRay, OutwardNormal);

    Record.Material = Material;

    return true;
}