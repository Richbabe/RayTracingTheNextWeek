#pragma once

#include "RTWeekend.h"
#include "Hittable.h"

class XYRect : public Hittable 
{
    public:
        XYRect() {}

        XYRect(float _x0, float _x1, float _y0, float _y1, float _k, 
            shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), Material(mat) {};

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const
        {
            // The bounding box must have non-zero width in each dimension, so pad the Z
            // dimension a small amount.
            OutputBox = AABB(Point3(x0, y0, k - 0.0001f), Point3(x1, y1, k + 0.0001f));
            return true;
        }

    public:
        shared_ptr<Material> Material;
        float x0, x1, y0, y1, k;
};

class XZRect : public Hittable 
{
    public:
        XZRect() {}

        XZRect(float _x0, float _x1, float _z0, float _z1, float _k, 
            shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), Material(mat) {};

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const
        {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            OutputBox = AABB(Point3(x0, k - 0.0001f, z0), Point3(x1, k + 0.0001f, z1));
            return true;
        }

    public:
        shared_ptr<Material> Material;
        float x0, x1, z0, z1, k;
};

class YZRect : public Hittable 
{
    public:
        YZRect() {}

        YZRect(float _y0, float _y1, float _z0, float _z1, float _k, 
            shared_ptr<Material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), Material(mat) {};

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const
        {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            OutputBox = AABB(Point3(k - 0.0001f, y0, z0), Point3(k + 0.0001f, y1, z1));
            return true;
        }

    public:
        shared_ptr<Material> Material;
        float y0, y1, z0, z1, k;
};

bool XYRect::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const 
{
    auto t = (k - InRay.GetOrigin().Z()) / InRay.GetDirection().Z();
    if (t < tMin || t > tMax)
    {
        return false;
    }
        
    auto x = InRay.GetOrigin().X() + t * InRay.GetDirection().X();
    auto y = InRay.GetOrigin().Y() + t * InRay.GetDirection().Y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
    {
        return false;
    }
        
    Record.u = (x - x0) / (x1 - x0);
    Record.v = (y - y0) / (y1 - y0);
    Record.t = t;
    auto OutwardNormal = Vector3(0.0f, 0.0f, 1.0f);
    Record.SetFaceNormal(InRay, OutwardNormal);
    Record.Material = Material;
    Record.p = InRay.At(t);
    return true;
}

bool XZRect::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const 
{
    auto t = (k - InRay.GetOrigin().Y()) / InRay.GetDirection().Y();
    if (t < tMin || t > tMax)
    {
        return false;
    }
        
    auto x = InRay.GetOrigin().X() + t * InRay.GetDirection().X();
    auto z = InRay.GetOrigin().Z() + t * InRay.GetDirection().Z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
    {
        return false;
    }
        
    Record.u = (x - x0) / (x1 - x0);
    Record.v = (z - z0) / (z1 - z0);
    Record.t = t;
    auto OutwardNormal = Vector3(0.0f, 1.0f, 0.0f);
    Record.SetFaceNormal(InRay, OutwardNormal);
    Record.Material = Material;
    Record.p = InRay.At(t);
    return true;
}

bool YZRect::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const 
{
    auto t = (k - InRay.GetOrigin().X()) / InRay.GetDirection().X();
    if (t < tMin || t > tMax)
    {
        return false;
    }
        
    auto y = InRay.GetOrigin().Y() + t * InRay.GetDirection().Y();
    auto z = InRay.GetOrigin().Z() + t * InRay.GetDirection().Z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
    {
        return false;
    }
        
    Record.u = (y - y0) / (y1 - y0);
    Record.v = (z - z0) / (z1 - z0);
    Record.t = t;
    auto OutwardNormal = Vector3(1.0f, 0.0f, 0.0f);
    Record.SetFaceNormal(InRay, OutwardNormal);
    Record.Material = Material;
    Record.p = InRay.At(t);
    return true;
}