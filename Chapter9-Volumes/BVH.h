#pragma once

#include "RTWeekEnd.h"

#include "Hittable.h"
#include "HittableList.h"

class BVHNode : public Hittable 
{
    public:
        BVHNode();

        BVHNode(const HittableList& List, float Time0, float Time1)
            : BVHNode(List.Objects, 0, List.Objects.size(), Time0, Time1)
        {}

        BVHNode(
            const std::vector<shared_ptr<Hittable>>& SrcObjects,
            size_t Start, size_t End, float Time0, float Time1);

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

        virtual bool BoundingBox(float InTime0, float InTime1, AABB& OutputBox) const override;

    public:
        shared_ptr<Hittable> Left;
        shared_ptr<Hittable> Right;
        AABB Box;
};

BVHNode::BVHNode(const std::vector<shared_ptr<Hittable>>& SrcObjects, size_t Start, size_t End, float Time0, float Time1)
{
    auto Objects = SrcObjects; // Create a modifiable array of the source scene objects

    int Axis = RandomInt(0, 2);
    auto Comparator = (Axis == 0) ? Box_X_Compare
                    : (Axis == 1) ? Box_Y_Compare
                                  : Box_Z_Compare;

    size_t ObjectSpan = End - Start;

    if (ObjectSpan == 1) 
    {
        Left = Right = Objects[Start];
    } 
    else if (ObjectSpan == 2) 
    {
        if (Comparator(Objects[Start], Objects[Start + 1])) 
        {
            Left = Objects[Start];
            Right = Objects[Start + 1];
        } 
        else 
        {
            Left = Objects[Start + 1];
            Right = Objects[Start];
        }
    } 
    else 
    {
        std::sort(Objects.begin() + Start, Objects.begin() + End, Comparator);

        auto Mid = Start + ObjectSpan / 2;
        Left = make_shared<BVHNode>(Objects, Start, Mid, Time0, Time1);
        Right = make_shared<BVHNode>(Objects, Mid, End, Time0, Time1);
    }

    AABB BoxLeft, BoxRight;

    if (  !Left->BoundingBox(Time0, Time1, BoxLeft)
       || !Right->BoundingBox(Time0, Time1, BoxRight)
    )
    {
        std::cerr << "No bounding box in BVHNode constructor.\n";
    }
        
    Box = SurroundingBox(BoxLeft, BoxRight);
}

bool BVHNode::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    if(!Box.Hit(InRay, tMin, tMax))
    {
        return false;
    }

    bool bHitLeft = Left->Hit(InRay, tMin, tMax, Record);
    bool bHitRight = Right->Hit(InRay, tMin, tMax, Record);

    return bHitLeft || bHitRight;
}

inline bool BoxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int Axis) {
    AABB BoxA;
    AABB BoxB;

    if (!a->BoundingBox(0.0f, 0.0f, BoxA) || !b->BoundingBox(0.0f, 0.0f, BoxB))
    {
        std::cerr << "No bounding box in BVHNode constructor.\n";
    }
        
    return BoxA.Min().Elements[Axis] < BoxB.Min().Elements[Axis];
}


bool Box_X_Compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 0);
}

bool Box_Y_Compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 1);
}

bool Box_Z_Compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 2);
}