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
        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override;

    public:
        Point3 Origin;
        float Radius;
        shared_ptr<Material> Material;

    private:
        static void GetSphereUV(const Point3& p, float& u, float& v) 
        {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.Y());
            auto phi = atan2(-p.Z(), p.X()) + PI;

            u = phi / (2.0f * PI);
            v = theta / PI;
        }
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
    GetSphereUV(OutwardNormal, Record.u, Record.v);
    Record.Material = Material;

    return true;
}

bool Sphere::BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const
{
    OutputBox = AABB(Origin - Vector3(Radius, Radius, Radius), Origin + Vector3(Radius, Radius, Radius));

    return true;
}
