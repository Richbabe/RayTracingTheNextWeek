#pragma once

#include "RTWeekend.h"
#include "Hittable.h"
#include "Texture.h"

class Material 
{
    public:
        virtual Color Emitted(float u, float v, const Point3& p) const 
        {
            return Color(0.f, 0.f, 0.f);
        }

        virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const = 0;
};

class Lambertian : public Material 
{
    public:
        Lambertian(const Color& InAlbedo) : Albedo(make_shared<SolidColor>(InAlbedo)) {}
        Lambertian(shared_ptr<Texture> InAlbedo) : Albedo(InAlbedo) {}

        virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const override 
        {
            auto ScatterDirection = Record.Normal + RandomUnitVector();

            // Catch degenerate scatter direction
            if (ScatterDirection.NearZero())
            {
                ScatterDirection = Record.Normal;
            }
                
            Scattered = Ray(Record.p, ScatterDirection, InRay.GetTime());
            Attenuation = Albedo->Value(Record.u, Record.v, Record.p);
            return true;
        }

    public:
        shared_ptr<Texture> Albedo;
};

class Metal : public Material 
{
    public:
        Metal(const Color& InAlbedo, float InFuzz) 
        : Albedo(InAlbedo),
          Fuzz(Clamp(InFuzz, 0.0f, 1.0f))
        {}

         virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const override 
        {
            Vector3 Reflected = Reflect(UnitVector(InRay.GetDirection()), Record.Normal);
            Scattered = Ray(Record.p, Reflected + Fuzz * RandomInUnitSphere(), InRay.GetTime());
            Attenuation = Albedo;
            return (Dot(Scattered.GetDirection(), Record.Normal) > 0);
        }

    public:
        Color Albedo;
        float Fuzz;
};

class Dielectric : public Material 
{
    public:
        Dielectric(float InRefractionIndex) : RefractionIndex(InRefractionIndex) {}

       virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const override 
        {
            Attenuation = Color(1.0f, 1.0f, 1.0f); // The glass surface absorbs nothing.
            float RefractionRatio = Record.bFrontFace ? (1.0f / RefractionIndex) : RefractionIndex;

            Vector3 UnitDirection = UnitVector(InRay.GetDirection());
            float CosTheta = fmin(Dot(-UnitDirection, Record.Normal), 1.0f);
            float SinTheta = sqrt(1.0f - CosTheta * CosTheta);

            bool bCanRefract = RefractionRatio * SinTheta <= 1.0f; // Sinθ' should <= 1.0f, else do reflect instead of refract
            Vector3 Direction;
            if(!bCanRefract || Reflectance(CosTheta, RefractionRatio) > RandomFloat())
            {
                 // Must Reflect
                Direction = Reflect(UnitDirection, Record.Normal);
            }
            else
            {
               // Can Refract
                Direction = Refract(UnitDirection, Record.Normal, RefractionRatio);
            }
            Scattered = Ray(Record.p, Direction, InRay.GetTime());
            return true;
        }

    public:
        float RefractionIndex; // Index of Refraction

    private:
        static float Reflectance(float Cosine, float ReflectionIndex)
        {
            // Use Schlick's approximation for reflectance.
            auto R0 = (1.0f - ReflectionIndex) / (1.0f + ReflectionIndex); // R0 == F0
            R0 = R0 * R0;
            return R0 + (1.0f - R0) * pow((1.0f - Cosine), 5.0f);
        }
};

class DiffuseLight : public Material  
{
    public:
        DiffuseLight(shared_ptr<Texture> a) : Emit(a) {}
        DiffuseLight(const Color& c) : Emit(make_shared<SolidColor>(c)) {}

        virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const override 
        {
            return false;
        }

        virtual Color Emitted(float u, float v, const Point3& p) const override 
        {
            return Emit->Value(u, v, p);
        }

    public:
        shared_ptr<Texture> Emit;
};

class Isotropic : public Material 
{
    public:
        Isotropic(const Color& c) : Albedo(make_shared<SolidColor>(c)) {}
        Isotropic(shared_ptr<Texture> a) : Albedo(a) {}

        virtual bool Scatter(
            const Ray& InRay, const HitRecord& Record, Color& Attenuation, Ray& Scattered
        ) const override {
            Scattered = Ray(Record.p, RandomInUnitSphere(), InRay.GetTime());
            Attenuation = Albedo->Value(Record.u, Record.v, Record.p);
            return true;
        }

    public:
        shared_ptr<Texture> Albedo;
};