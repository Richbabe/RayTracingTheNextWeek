#pragma once

#include "RTWeekend.h"

#include "Hittable.h"
#include "Material.h"
#include "Texture.h"

class ConstantMedium : public Hittable
{
    public:
        ConstantMedium(shared_ptr<Hittable> b, float d, shared_ptr<Texture> a)
            : Boundary(b),
              NegInvDensity(-1/d),
              PhaseFunction(make_shared<Isotropic>(a))
            {}

        ConstantMedium(shared_ptr<Hittable> b, float d, const Color& c)
            : Boundary(b),
              NegInvDensity(-1/d),
              PhaseFunction(make_shared<Isotropic>(c))
            {}

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override
        {
            return Boundary->BoundingBox(InTime0, InTime1, OutputBox);
        }

    public:
        shared_ptr<Hittable> Boundary;
        shared_ptr<Material> PhaseFunction;
        float NegInvDensity;
};

bool ConstantMedium::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool bEnableDebug = false;
    const bool bDebugging = bEnableDebug && RandomFloat() < 0.00001f;

    HitRecord Rec1, Rec2;

    if (!Boundary->Hit(InRay, -Infinity, Infinity, Rec1))
    {
        return false;
    }
    
    if (!Boundary->Hit(InRay, Rec1.t + 0.0001f, Infinity, Rec2))
    {
        return false;
    }
        
    if (bDebugging)
    {
        std::cerr << "\ntMin=" << Rec1.t << ", tMax=" << Rec2.t << '\n';
    } 

    if (Rec1.t < tMin)
    {
        Rec1.t = tMin;
    } 

    if (Rec2.t > tMax)
    {
        Rec2.t = tMax;
    } 

    if (Rec1.t >= Rec2.t)
    {
        return false;
    }
    
    if (Rec1.t < 0)
    {
        Rec1.t = 0;
    }
        
    const auto RayLength = InRay.GetDirection().Length();
    const auto DistanceInsideBoundary = (Rec2.t - Rec1.t) * RayLength;
    const auto HitDistance = NegInvDensity * log(RandomFloat());

    if (HitDistance > DistanceInsideBoundary)
    {
        return false;
    }
        
    Record.t = Rec1.t + HitDistance / RayLength;
    Record.p = InRay.At(Record.t);

    if (bDebugging) 
    {
        std::cerr << "HitDistance = " <<  HitDistance << '\n'
                  << "Record.t = " <<  Record.t << '\n'
                  << "Record.p = " <<  Record.p << '\n';
    }

    Record.Normal = Vector3(1.0f, 0.0f, 0.0f);  // arbitrary
    Record.bFrontFace = true;                   // also arbitrary
    Record.Material = PhaseFunction;

    return true;
}