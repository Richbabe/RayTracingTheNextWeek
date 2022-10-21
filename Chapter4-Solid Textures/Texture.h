 #pragma once

 #include "RTWeekend.h"

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