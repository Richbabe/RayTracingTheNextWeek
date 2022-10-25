#pragma once

#include "RTWeekend.h"

#include <iostream>

void WriteColor(std::ostream &out, const Color& PixelColor, int SamplesPerPixel) 
{
    auto R = PixelColor.X();
    auto G = PixelColor.Y();
    auto B = PixelColor.Z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto Scale = 1.0f / SamplesPerPixel;
    R = sqrt(Scale * R);
    G = sqrt(Scale * G);
    B = sqrt(Scale * B);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * Clamp(R, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * Clamp(G, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * Clamp(B, 0.0f, 0.999f)) << '\n';
}