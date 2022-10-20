#pragma once

#include "RTWeekend.h"

#include "Hittable.h"

class HittableList : public Hittable 
{
    public:
        HittableList() {}
        HittableList(shared_ptr<Hittable> Object) { Add(Object); }

        inline void Clear() { Objects.clear(); }
        inline void Add(shared_ptr<Hittable> Object) { Objects.emplace_back(Object); }

        virtual bool Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const override;

    public:
        std::vector<shared_ptr<Hittable>> Objects;
};

bool HittableList::Hit(const Ray& InRay, float tMin, float tMax, HitRecord& Record) const
{
    HitRecord TempRecord;
    bool bHitAnything = false;
    auto ClosestSoFar = tMax;

    for (const auto& Object : Objects) {
        if (Object->Hit(InRay, tMin, ClosestSoFar, TempRecord)) {
            bHitAnything = true;
            ClosestSoFar = TempRecord.t;
            Record = TempRecord;
        }
    }

    return bHitAnything;
}