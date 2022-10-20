#pragma once

#include "Hittable.h"
#include "Vector3.h"

class Sphere : public Hittable 
{
    public:
        Sphere() {}
        Sphere(const Point3& Ori, float r, shared_ptr<Material> InMaterial) 
        : Origin(Ori), Radius(r), Material(InMaterial) {};

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

    public:
        Point3 Origin;
        float Radius;
        shared_ptr<Material> Material;
};

bool Sphere::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    Vector3 OC = InRay.GetOrigin() - Origin;
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

    Vector3 OutwardNormal = (Record.p - Origin) / Radius;
    Record.SetFaceNormal(InRay, OutwardNormal);

    Record.Material = Material;

    return true;
}
