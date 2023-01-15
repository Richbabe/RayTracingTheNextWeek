#pragma once

#include "RTWeekend.h"

#include <cmath>

using std::sqrt;

class Vector3 
{
    public:
        Vector3() : Elements{0,0,0} {}
        Vector3(float e0, float e1, float e2) : Elements{e0, e1, e2} {}

        inline float X() const { return Elements[0]; }
        inline float Y() const { return Elements[1]; }
        inline float Z() const { return Elements[2]; }

        inline Vector3 operator-() const { return Vector3(-Elements[0], -Elements[1], -Elements[2]); }
        inline float operator[](int i) const { return Elements[i]; }
        inline float& operator[](int i) { return Elements[i]; }

        Vector3& operator+=(const Vector3& v) 
        {
            Elements[0] += v.Elements[0];
            Elements[1] += v.Elements[1];
            Elements[2] += v.Elements[2];
            return *this;
        }

        Vector3& operator*=(float t) 
        {
            Elements[0] *= t;
            Elements[1] *= t;
            Elements[2] *= t;
            return *this;
        }

        Vector3& operator/=(float t) 
        {
            return *this *= 1/t;
        }

        float Length() const 
        {
            return sqrt(LengthSquared());
        }

        float LengthSquared() const 
        {
            return Elements[0] * Elements[0] + Elements[1] * Elements[1] + Elements[2] * Elements[2];
        }

        bool NearZero() const 
        {
            // Return true if the vector is close to zero in all dimensions.
            const auto s = 1e-8;
            return (fabs(Elements[0]) < s) && (fabs(Elements[1]) < s) && (fabs(Elements[2]) < s);
        }

        inline static Vector3 Random()
        {
            return Vector3(RandomFloat(), RandomFloat(), RandomFloat());
        }

        inline static Vector3 Random(float Min, float Max) 
        {
            return Vector3(RandomFloat(Min, Max), RandomFloat(Min, Max), RandomFloat(Min, Max));
        }

    public:
        float Elements[3];
};

// Type aliases for Vector3

using Point3 = Vector3;   // 3D point
using Color = Vector3;    // RGB color

// Vector3 Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vector3& v) 
{
    return out << v.Elements[0] << ' ' << v.Elements[1] << ' ' << v.Elements[2];
}

inline Vector3 operator+(const Vector3& u, const Vector3& v) 
{
    return Vector3(u.Elements[0] + v.Elements[0], u.Elements[1] + v.Elements[1], u.Elements[2] + v.Elements[2]);
}

inline Vector3 operator-(const Vector3& u, const Vector3& v) 
{
    return Vector3(u.Elements[0] - v.Elements[0], u.Elements[1] - v.Elements[1], u.Elements[2] - v.Elements[2]);
}

inline Vector3 operator*(const Vector3& u, const Vector3& v) 
{
    return Vector3(u.Elements[0] * v.Elements[0], u.Elements[1] * v.Elements[1], u.Elements[2] * v.Elements[2]);
}

inline Vector3 operator*(float t, const Vector3& v) 
{
    return Vector3(t * v.Elements[0], t * v.Elements[1], t * v.Elements[2]);
}

inline Vector3 operator*(const Vector3& v, float t) 
{
    return t * v;
}

inline Vector3 operator/(const Vector3& v, float t) 
{
    return (1/t) * v;
}

inline float Dot(const Vector3& u, const Vector3& v) 
{
    return u.Elements[0] * v.Elements[0]
         + u.Elements[1] * v.Elements[1]
         + u.Elements[2] * v.Elements[2];
}

inline Vector3 Cross(const Vector3& u, const Vector3& v) 
{
    return Vector3(u.Elements[1] * v.Elements[2] - u.Elements[2] * v.Elements[1],
                u.Elements[2] * v.Elements[0] - u.Elements[0] * v.Elements[2],
                u.Elements[0] * v.Elements[1] - u.Elements[1] * v.Elements[0]);
}

inline Vector3 UnitVector(const Vector3& v) 
{
    return v / v.Length();
}

Vector3 RandomInUnitSphere() 
{
    while (true) 
    {
        auto Point = Vector3::Random(-1.0f, 1.0f);
        if (Point.LengthSquared() >= 1.0f)
        {
            continue;
        } 
        return Point;
    }
}

Vector3 RandomUnitVector() 
{
    return UnitVector(RandomInUnitSphere());
}

Vector3 RandomInHemisphere(const Vector3& Normal) 
{
    Vector3 InUnitSphere = RandomInUnitSphere();
    if (Dot(InUnitSphere, Normal) > 0.0f) // In the same hemisphere as the normal
    {
        return InUnitSphere;
    }  
    else
    {
        return -InUnitSphere;
    }
}

Vector3 Reflect(const Vector3& v, const Vector3& n) 
{
    return v - 2.0f * Dot(v, n) * n;
}

Vector3 Refract(const Vector3& v, const Vector3& n, float EtaiOverEtat) 
{
    auto CosTheta = fmin(Dot(-v, n), 1.0f);
    Vector3 RefractPerp = EtaiOverEtat * (v + CosTheta * n);
    Vector3 RefractParallel = -sqrt(1.0f - fabs(RefractPerp.LengthSquared())) * n;
    return RefractPerp + RefractParallel;
}

Vector3 RandomInUnitDisk() 
{
    while (true) 
    {
        auto p = Vector3(RandomFloat(-1.0, 1.0f), RandomFloat(-1.0f, 1.0f), 0.0f);
        if (p.LengthSquared() >= 1.0f)
        {
            continue;
        } 
        return p;
    }
}