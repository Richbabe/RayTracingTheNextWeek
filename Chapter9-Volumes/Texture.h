#pragma once

#include "RTWeekend.h"
#include "rtw_stb_image.h"
#include "Perlin.h"

class Texture
{
    public:
        virtual Color Value(float u, float v, const Point3& p) const = 0;
};
 
class SolidColor : public Texture 
{
    public:
        SolidColor() {}
        SolidColor(const Color& Color) : ColorValue(Color) {}

        SolidColor(float Red, float Green, float Blue)
            : SolidColor(Color(Red, Green, Blue)) {}

        virtual Color Value(float u, float v, const Point3& p) const override 
        {
            return ColorValue;
        }

    private:
        Color ColorValue;
};

class CheckerTexture : public Texture 
{
    public:
        CheckerTexture() {}

        CheckerTexture(shared_ptr<Texture> InEven, shared_ptr<Texture> InOdd)
            : Even(InEven), Odd(InOdd) {}

        CheckerTexture(const Color& Color1, const Color& Color2)
            : Even(make_shared<SolidColor>(Color1)) , Odd(make_shared<SolidColor>(Color2)) {}

        virtual Color Value(float u, float v, const Point3& p) const override  
        {
            auto sines = sin(10.0f * p.X()) * sin(10.0f * p.Y()) * sin(10.0f * p.Z());
            if (sines < 0.0f)
            {
                return Odd->Value(u, v, p);
            }
            else
            {
                return Even->Value(u, v, p);
            } 
        }

    public:
        shared_ptr<Texture> Odd;
        shared_ptr<Texture> Even;
};

class NoiseTexture : public Texture 
{
    public:
        NoiseTexture() {}
        NoiseTexture(float InScale) : Scale(InScale) {}

        virtual Color Value(float u, float v, const Point3& p) const override  
        {
            return Color(1.0f, 1.0f, 1.0f) * 0.5f * (1.0f + sin(Scale * p.Z() + 10.0f * Noise.Turb(p)));
        }

    public:
        Perlin Noise;
        float Scale;
};

class ImageTexture : public Texture 
{
    public:
        const static int BytesPerPixel = 3;

        ImageTexture()
          : Data(nullptr), Width(0), Height(0), BytesPerScanline(0) {}

        ImageTexture(const char* FileName) 
        {
            auto ComponentsPerPixel = BytesPerPixel;

            Data = stbi_load(
                FileName, &Width, &Height, &ComponentsPerPixel, ComponentsPerPixel);

            if (!Data) 
            {
                std::cerr << "ERROR: Could not load texture image file '" << FileName << "'.\n";
                Width = Height = 0;
            }

            BytesPerScanline = BytesPerPixel * Width;
        }

        ~ImageTexture() 
        {
            delete Data;
        }

        virtual Color Value(float u, float v, const Point3& p) const override  
        {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (Data == nullptr)
            {
                return Color(0.0f, 1.0f, 1.0f);
            }
            
            // Clamp input texture coordinates to [0,1] x [1,0]
            u = Clamp(u, 0.0f, 1.0f);
            v = 1.0 - Clamp(v, 0.0f, 1.0f);  // Flip V to image coordinates

            auto i = static_cast<int>(u * Width);
            auto j = static_cast<int>(v * Height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= Width)
            {
                i = Width - 1;
            }  
            if (j >= Height)
            {
                j = Height - 1;
            } 

            const auto ColorScale = 1.0f / 255.0f;
            auto pixel = Data + j * BytesPerScanline + i * BytesPerPixel;

            return Color(ColorScale * pixel[0], ColorScale * pixel[1], ColorScale * pixel[2]);
        }

    private:
        unsigned char *Data;
        int Width, Height;
        int BytesPerScanline;
};