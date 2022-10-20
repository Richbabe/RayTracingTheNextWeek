#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <iostream>
#include <vector>
#include <random>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const float Infinity = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385;

// Utility Functions

inline float DegreesToRadians(float Degrees) 
{
    return Degrees * PI / 180.0f;
}

// Returns a random real in [0,1).
inline float RandomFloat() 
{
    static std::uniform_real_distribution<float> Distribution(0.0f, 1.0f);
    static std::mt19937 Generator;
    return Distribution(Generator);
}

// Returns a random real in [min,max).
inline float RandomFloat(float Min, float Max) 
{
    return Min + (Max - Min) * RandomFloat();
}

inline float Clamp(float Value, float Min, float Max) {
    if (Value < Min) return Min;
    if (Value > Max) return Max;
    return Value;
}

// Common Headers

#include "Ray.h"
#include "Vector3.h"