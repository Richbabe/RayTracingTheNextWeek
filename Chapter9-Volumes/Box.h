#pragma once

#include "RTWeekend.h"

#include "AARect.h"
#include "HittableList.h"

class Box : public Hittable  
{
    public:
        Box() {}
        Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr);

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override
        {
            OutputBox = AABB(BoxMin, BoxMax);
            return true;
        }

    public:
        Point3 BoxMin;
        Point3 BoxMax;
        HittableList Sides;
};

Box::Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr) 
{
    BoxMin = p0;
    BoxMax = p1;

    Sides.Add(make_shared<XYRect>(p0.X(), p1.X(), p0.Y(), p1.Y(), p1.Z(), ptr));
    Sides.Add(make_shared<XYRect>(p0.X(), p1.X(), p0.Y(), p1.Y(), p0.Z(), ptr));

    Sides.Add(make_shared<XZRect>(p0.X(), p1.X(), p0.Z(), p1.Z(), p1.Y(), ptr));
    Sides.Add(make_shared<XZRect>(p0.X(), p1.X(), p0.Z(), p1.Z(), p0.Y(), ptr));

    Sides.Add(make_shared<YZRect>(p0.Y(), p1.Y(), p0.Z(), p1.Z(), p1.X(), ptr));
    Sides.Add(make_shared<YZRect>(p0.Y(), p1.Y(), p0.Z(), p1.Z(), p0.X(), ptr));
}

bool Box::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const 
{
    return Sides.Hit(InRay, tMin, tMax, Record);
}
