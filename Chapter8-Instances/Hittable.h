#pragma once

#include "RTWeekend.h"
#include "AABB.h"

class Material;

struct HitRecord
{
    Point3 p;
    Vector3 Normal;
    shared_ptr<Material> Material;
    float t;
    float u;
    float v;
    bool bFrontFace;

    inline void SetFaceNormal(const Ray& Ray, const Vector3& OutwardNormal)
    {
        bFrontFace =  Dot(Ray.GetDirection(), OutwardNormal) < 0.0f;
        Normal = bFrontFace ? OutwardNormal : -OutwardNormal;
    }
};

class Hittable 
{
    public:
        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Recordord) const = 0;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const = 0;
};

class Translate : public Hittable 
{
    public:
        Translate(shared_ptr<Hittable> p, const Vector3& Displacement)
            : Ptr(p), Offset(Displacement) {}

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Recordord) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override;

    public:
        shared_ptr<Hittable> Ptr;
        Vector3 Offset;
};

bool Translate::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Recordord) const
{
    Ray MovedRay(InRay.GetOrigin() - Offset, InRay.GetDirection(), InRay.GetTime());
    if (!Ptr->Hit(MovedRay, tMin, tMax, Recordord))
    {
        return false;
    }
        
    Recordord.p += Offset;
    Recordord.SetFaceNormal(MovedRay, Recordord.Normal);

    return true;
}

bool Translate::BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const
{
    if (!Ptr->BoundingBox(InTime0, InTime1, OutputBox))
    {
        return false;
    }
        
    OutputBox = AABB(
        OutputBox.Min() + Offset,
        OutputBox.Max() + Offset);

    return true;
}

class RotateY : public Hittable 
{
    public:
        RotateY(shared_ptr<Hittable> p, float Angle);

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Recordord) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override
        {
            OutputBox = BBox;
            return bHasBox;
        }

    public:
        shared_ptr<Hittable> Ptr;
        float SinTheta;
        float CosTheta;
        bool bHasBox;
        AABB BBox;
};

RotateY::RotateY(shared_ptr<Hittable> p, float Angle) 
    : Ptr(p) 
{
    auto Radians = DegreesToRadians(Angle);
    SinTheta = sin(Radians);
    CosTheta = cos(Radians);
    bHasBox = Ptr->BoundingBox(0.0f, 1.0f, BBox);

    Point3 Min( Infinity,  Infinity,  Infinity);
    Point3 Max(-Infinity, -Infinity, -Infinity);

    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            for (int k = 0; k < 2; k++)
            {
                auto x = i * BBox.Max().X() + (1 - i) * BBox.Min().X();
                auto y = j * BBox.Max().Y() + (1 - j) * BBox.Min().Y();
                auto z = k * BBox.Max().Z() + (1 - k) * BBox.Min().Z();

                auto newx =  CosTheta * x + SinTheta * z;
                auto newz = -SinTheta * x + CosTheta * z;

                Vector3 Tester(newx, y, newz);

                for (int c = 0; c < 3; c++) 
                {
                    Min[c] = fmin(Min[c], Tester[c]);
                    Max[c] = fmax(Max[c], Tester[c]);
                }
            }
        }
    }

    BBox = AABB(Min, Max);
}

bool RotateY::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    auto Origin = InRay.GetOrigin();
    auto DiRecordtion = InRay.GetDirection();

    Origin[0] = CosTheta*InRay.GetOrigin()[0] - SinTheta*InRay.GetOrigin()[2];
    Origin[2] = SinTheta*InRay.GetOrigin()[0] + CosTheta*InRay.GetOrigin()[2];

    DiRecordtion[0] = CosTheta*InRay.GetDirection()[0] - SinTheta*InRay.GetDirection()[2];
    DiRecordtion[2] = SinTheta*InRay.GetDirection()[0] + CosTheta*InRay.GetDirection()[2];

    Ray RotatedRay(Origin, DiRecordtion, InRay.GetTime());

    if (!Ptr->Hit(RotatedRay, tMin, tMax, Record))
    {
        return false;
    }

    auto p = Record.p;
    auto Normal = Record.Normal;

    p[0] =  CosTheta*Record.p[0] + SinTheta*Record.p[2];
    p[2] = -SinTheta*Record.p[0] + CosTheta*Record.p[2];

    Normal[0] =  CosTheta*Record.Normal[0] + SinTheta*Record.Normal[2];
    Normal[2] = -SinTheta*Record.Normal[0] + CosTheta*Record.Normal[2];

    Record.p = p;
    Record.SetFaceNormal(RotatedRay, Normal);

    return true;
}