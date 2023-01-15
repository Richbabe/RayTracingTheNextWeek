#pragma once

#include "RTWeekend.h"

class Perlin 
{
    public:
        Perlin() 
        {
            RanVec = new Vector3[PointCount];
            for (int i = 0; i < PointCount; ++i) 
            {
                RanVec[i] = UnitVector(Vector3::Random(-1.0f, 1.0f));
            }

            PermX = PerlinGeneratePerm();
            PermY = PerlinGeneratePerm();
            PermZ = PerlinGeneratePerm();
        }

        ~Perlin() 
        {
            delete[] RanVec;
            delete[] PermX;
            delete[] PermY;
            delete[] PermZ;
        }

        float Noise(const Point3& p) const 
        {
            auto u = p.X() - floor(p.X());
            auto v = p.Y() - floor(p.Y());
            auto w = p.Z() - floor(p.Z());

            // Hermite cubic to round off the interpolation
            // Get rid of Mach bands
            u = u * u * (3.0f - 2.0f * u);
            v = v * v * (3.0f - 2.0f * v);
            w = w * w * (3.0f - 2.0f * w);

            auto i = static_cast<int>(floor(p.X()));
            auto j = static_cast<int>(floor(p.Y()));
            auto k = static_cast<int>(floor(p.Z()));
            Vector3 c[2][2][2];

            for (int di = 0; di < 2; di++)
            {
                for(int dj = 0; dj < 2; dj++)
                {
                     for (int dk = 0; dk < 2; dk++)
                     {
                        c[di][dj][dk] = RanVec[
                            PermX[(i + di) & 255] ^
                            PermX[(j + dj) & 255] ^
                            PermX[(k + dk) & 255]
                        ];
                     }
                }
            }

            return PerlinInterp(c, u, v, w);
        }

        float Turb(const Point3& p, int Depth=7) const 
        {
            auto Accum = 0.0f;
            auto TempP = p;
            auto Weight = 1.0f;

            for (int i = 0; i < Depth; i++) 
            {
                Accum += Weight * Noise(TempP);
                Weight *= 0.5;
                TempP *= 2;
            }

            return fabs(Accum);
        }

    private:
        static const int PointCount = 256;
        Vector3* RanVec;
        int* PermX;
        int* PermY;
        int* PermZ;

        static int* PerlinGeneratePerm() 
        {
            auto p = new int[PointCount];

            for (int i = 0; i < Perlin::PointCount; i++)
            {
                p[i] = i;
            }
                
            Permute(p, PointCount);

            return p;
        }

        static void Permute(int* p, int n) 
        {
            for (int i = n - 1; i > 0; i--) 
            {
                int Target = RandomInt(0, i);
                int Temp = p[i];
                p[i] = p[Target];
                p[Target] = Temp;
            }
        }

        static float TrilinearInterp(float c[2][2][2], float u, float v, float w) 
        {
            auto Accum = 0.0f;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    for (int k = 0; k < 2; k++)
                    {
                        Accum += (i * u + (1 - i) * (1 - u)) *
                                (j * v + (1 - j) * (1 - v)) *
                                (k * w + (1 - k) * (1 - w)) * c[i][j][k];
                    }
                }
            }
                
            return Accum;
        }

        static float PerlinInterp(Vector3 c[2][2][2], float u, float v, float w) 
        {
            auto uu = u * u * (3.0f - 2.0f * u);
            auto vv = v * v * (3.0f - 2.0f * v);
            auto ww = w * w * (3.0f - 2.0f * w);
            auto Accum = 0.0f;

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    for (int k = 0; k < 2; k++)
                    {
                        Vector3 Weight(u - i, v - j, w - k);
                        Accum += (i * uu + (1 - i) * (1 - uu))
                               * (j * vv + (1 - j)*(1 - vv))
                               * (k*ww + (1 - k)*(1 - ww))
                               * Dot(c[i][j][k], Weight);
                    }
                }
            }

            return Accum;
        }
};