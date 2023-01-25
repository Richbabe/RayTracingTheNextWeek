#include "RTWeekend.h"

#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "MovingSphere.h"
#include "AARect.h"
#include "Box.h"
#include "ConstantMedium.h"
#include "BVH.h"
#include "ParallelFor.h"
#include <chrono>

Color RayColor(const Ray& InRay, const Color& Background, const HittableList& World, int Depth) 
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (Depth <= 0)
    {
        return Color(0.0f, 0.0f, 0.0f);
    }
        
    // Object
    HitRecord Record;
    // If the ray hits nothing, return the background color.
    if(!World.Hit(InRay, 0.001f, Infinity, Record)) // Use 0.001f as t to instead of 0.0f to avoid shadow acne cause by super near intersection
    {
       return Background;
    }

    Ray Scattered;
    Color Attenuation;
    Color Emitted = Record.Material->Emitted(Record.u, Record.v, Record.p);

    if(!Record.Material->Scatter(InRay, Record, Attenuation, Scattered))
    {
        return Emitted;
    }

    return Emitted + Attenuation * RayColor(Scattered, Background, World, Depth - 1);
}

HittableList RandomScene() 
{
    HittableList World;

    auto checker = make_shared<CheckerTexture>(Color(0.2f, 0.3f, 0.1f), Color(0.9f, 0.9f, 0.9f));
    auto GroundMaterial = make_shared<Lambertian>(checker);
    World.Add(make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, GroundMaterial));

    for (int a = -11; a < 11; a++) 
    {
        for (int b = -11; b < 11; b++) 
        {
            auto ChooseMat = RandomFloat();
            Point3 Center(a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat());

            if ((Center - Point3(4.0f, 0.2f, 0.0f)).Length() > 0.9f) 
            {
                shared_ptr<Material> SphereMaterial;

                if (ChooseMat < 0.8f) 
                {
                    // diffuse
                    auto Albedo = Color::Random() * Color::Random();
                    SphereMaterial = make_shared<Lambertian>(Albedo);

                    auto Center2 = Center + Vector3(0.0f, RandomFloat(0.0f, 0.5f), 0.0f);
                    World.Add(make_shared<MovingSphere>(
                        Center, Center2, 0.0f, 1.0f, 0.2f, SphereMaterial));
                } 
                else if (ChooseMat < 0.95f) 
                {
                    // metal
                    auto Albedo = Color::Random(0.5f, 1.0f);
                    auto Fuzz = RandomFloat(0.0f, 0.5f);
                    SphereMaterial = make_shared<Metal>(Albedo, Fuzz);
                    World.Add(make_shared<Sphere>(Center, 0.2f, SphereMaterial));
                } 
                else 
                {
                    // glass
                    SphereMaterial = make_shared<Dielectric>(1.5f);
                    World.Add(make_shared<Sphere>(Center, 0.2f, SphereMaterial));
                }
            }
        }
    }

    auto Material1 = make_shared<Dielectric>(1.5f);
    World.Add(make_shared<Sphere>(Point3(0.0f, 1.0f, 0.0f), 1.0f, Material1));

    auto material2 = make_shared<Lambertian>(Color(0.4f, 0.2f, 0.1f));
    World.Add(make_shared<Sphere>(Point3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

    auto material3 = make_shared<Metal>(Color(0.7f, 0.6f, 0.5f), 0.0f);
    World.Add(make_shared<Sphere>(Point3(4.0f, 1.0f, 0.0f), 1.0f, material3));

    return World;
}

HittableList TwoSpheres() 
{
    HittableList Objects;

    auto Checker = make_shared<CheckerTexture>(Color(0.2f, 0.3f, 0.1f), Color(0.9f, 0.9f, 0.9f));

    Objects.Add(make_shared<Sphere>(Point3(0.0f, -10.0f, 0.0f), 10.0f, make_shared<Lambertian>(Checker)));
    Objects.Add(make_shared<Sphere>(Point3(0.0f, 10.0f, 0.0f), 10.0f, make_shared<Lambertian>(Checker)));

    return Objects;
}

HittableList TwoPerlinSpheres() 
{
    HittableList Objects;

    auto PerTexture = make_shared<NoiseTexture>(4.0f);
    Objects.Add(make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<Lambertian>(PerTexture)));
    Objects.Add(make_shared<Sphere>(Point3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<Lambertian>(PerTexture)));

    return Objects;
}

HittableList Earth() 
{
    auto EarthTexture = make_shared<ImageTexture>("earthmap.jpg");
    auto EarthSurface = make_shared<Lambertian>(EarthTexture);
    auto Globe = make_shared<Sphere>(Point3(0.f, 0.f, 0.f), 2.0f, EarthSurface);

    return HittableList(Globe);
}

HittableList SimpleLight() 
{
    HittableList Objects;

    auto PerTexture = make_shared<NoiseTexture>(4.0f);
    Objects.Add(make_shared<Sphere>(Point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<Lambertian>(PerTexture)));
    Objects.Add(make_shared<Sphere>(Point3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<Lambertian>(PerTexture)));

    auto DiffLight = make_shared<DiffuseLight>(Color(4.0f, 4.0f, 4.0f));
    Objects.Add(make_shared<XYRect>(3.0f, 5.0f, 1.0f, 3.0f, -2.0f, DiffLight));

    return Objects;
}

HittableList CornellBox() 
{
    HittableList Objects;

    auto Red   = make_shared<Lambertian>(Color(.65f, .05f, .05f));
    auto White = make_shared<Lambertian>(Color(.73f, .73f, .73f));
    auto Green = make_shared<Lambertian>(Color(.12f, .45f, .15f));
    auto Light = make_shared<DiffuseLight>(Color(15.0f, 15.0f, 15.0f));

    Objects.Add(make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, Green));
    Objects.Add(make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, Red));
    Objects.Add(make_shared<XZRect>(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, Light));
    Objects.Add(make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, White));
    Objects.Add(make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, White));
    Objects.Add(make_shared<XYRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, White));

    // Instances
    shared_ptr<Hittable> Box1 = make_shared<Box>(Point3(0.f, 0.f, 0.f), Point3(165.0f, 330.0f, 165.0f), White);
    Box1 = make_shared<RotateY>(Box1, 15.0f);
    Box1 = make_shared<Translate>(Box1, Vector3(265.0f, 0.0f, 295.0f));
    Objects.Add(Box1);

    shared_ptr<Hittable> Box2 = make_shared<Box>(Point3(0.0f, 0.0f, 0.0f), Point3(165.0f, 165.0f, 165.0f), White);
    Box2 = make_shared<RotateY>(Box2, -18.0f);
    Box2 = make_shared<Translate>(Box2, Vector3(130.0f, 0.0f, 65.0f));
    Objects.Add(Box2);

    return Objects;
}

HittableList CornellSmoke() 
{
    HittableList Objects;

    auto Red   = make_shared<Lambertian>(Color(.65f, .05f, .05f));
    auto White = make_shared<Lambertian>(Color(.73f, .73f, .73f));
    auto Green = make_shared<Lambertian>(Color(.12f, .45f, .15f));
    auto Light = make_shared<DiffuseLight>(Color(7.0f, 7.0f, 7.0f));

    Objects.Add(make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, Green));
    Objects.Add(make_shared<YZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, Red));
    Objects.Add(make_shared<XZRect>(113.0f, 443.0f, 127.0f, 432.0f, 554.0f, Light));
    Objects.Add(make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, White));
    Objects.Add(make_shared<XZRect>(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, White));
    Objects.Add(make_shared<XYRect>(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, White));

    // Instances
    shared_ptr<Hittable> Box1 = make_shared<Box>(Point3(0.f, 0.f, 0.f), Point3(165.0f, 330.0f, 165.0f), White);
    Box1 = make_shared<RotateY>(Box1, 15.0f);
    Box1 = make_shared<Translate>(Box1, Vector3(265.0f, 0.0f, 295.0f));

    shared_ptr<Hittable> Box2 = make_shared<Box>(Point3(0.0f, 0.0f, 0.0f), Point3(165.0f, 165.0f, 165.0f), White);
    Box2 = make_shared<RotateY>(Box2, -18.0f);
    Box2 = make_shared<Translate>(Box2, Vector3(130.0f, 0.0f, 65.0f));

    Objects.Add(make_shared<ConstantMedium>(Box1, 0.01f, Color(0.0f, 0.0f, 0.0f)));
    Objects.Add(make_shared<ConstantMedium>(Box2, 0.01f, Color(1.0f, 1.0f, 1.0f)));

    return Objects;
}

HittableList FinalScene() 
{
    HittableList Boxes1;
    auto Ground = make_shared<Lambertian>(Color(0.48f, 0.83f, 0.53f));

    const int BoxesPerSide = 20;
    for (int i = 0; i < BoxesPerSide; i++) 
    {
        for (int j = 0; j < BoxesPerSide; j++) 
        {
            auto w = 100.0f;
            auto x0 = -1000.0f + i * w;
            auto z0 = -1000.0f + j * w;
            auto y0 = 0.0f;
            auto x1 = x0 + w;
            auto y1 = RandomFloat(1.0f, 101.0f);
            auto z1 = z0 + w;

            Boxes1.Add(make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), Ground));
        }
    }
    
    HittableList Objects;

    Objects.Add(make_shared<BVHNode>(Boxes1, 0.0f, 1.0f));

    auto Light = make_shared<DiffuseLight>(Color(7.0f, 7.0f, 7.0f));
    Objects.Add(make_shared<XZRect>(123.0f, 423.0f, 147.0f, 412.0f, 554.0f, Light));

    auto Center1 = Point3(400.0f, 400.0f, 200.0f);
    auto Center2 = Center1 + Vector3(30.0f, 0.0f, 0.0f);
    auto MovingSphereMaterial = make_shared<Lambertian>(Color(0.7f, 0.3f, 0.1f));
    Objects.Add(make_shared<MovingSphere>(Center1, Center2, 0.0f, 1.0f, 50.0f, MovingSphereMaterial));

    Objects.Add(make_shared<Sphere>(Point3(260.0f, 150.0f, 45.0f), 50.0f, make_shared<Dielectric>(1.5f)));
    Objects.Add(make_shared<Sphere>(
        Point3(0.0f, 150.0f, 145.0f), 50.0f, make_shared<Metal>(Color(0.8f, 0.8f, 0.9f), 1.0f)
    ));

    auto Boundary = make_shared<Sphere>(Point3(360.0f, 150.0f, 145.0f), 70.0f, make_shared<Dielectric>(1.5f));
    Objects.Add(Boundary);
    Objects.Add(make_shared<ConstantMedium>(Boundary, 0.2f, Color(0.2f, 0.4f, 0.9f)));
    Boundary = make_shared<Sphere>(Point3(0.0f, 0.0f, 0.0f), 5000.0f, make_shared<Dielectric>(1.5f));
    Objects.Add(make_shared<ConstantMedium>(Boundary, .0001f, Color(1.0f, 1.0f, 1.0f)));

    auto EMat = make_shared<Lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
    Objects.Add(make_shared<Sphere>(Point3(400.0f, 200.0f, 400.0f), 100.0f, EMat));
    auto Pertext = make_shared<NoiseTexture>(0.1f);
    Objects.Add(make_shared<Sphere>(Point3(220.0f, 280.0f, 300.0f), 80.0f, make_shared<Lambertian>(Pertext)));

    HittableList Boxes2;
    auto White = make_shared<Lambertian>(Color(.73f, .73f, .73f));
    int ns = 1000;
    for (int j = 0; j < ns; j++) 
    {
        Boxes2.Add(make_shared<Sphere>(Point3::Random(0.0f, 165.0f), 10.0f, White));
    }

    Objects.Add(make_shared<Translate>(
        make_shared<RotateY>(
            make_shared<BVHNode>(Boxes2, 0.0f, 1.0f), 15.0f),
            Vector3(-100.0f, 270.0f, 395.0f)
        )
    );

    return Objects;
}

int main()
{
    // Image

    auto AspectRatio = 16.0f / 9.0f;
    int ImageWidth = 400;
    int ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
    int SamplesPerPixel = 100;
    const int MaxDepth = 50;

    // World
    
    HittableList World;

    Point3 LookFrom;
    Point3 LookAt;
    auto FOV = 40.0f;
    auto Aperture = 0.0f;
    Color Background(0.f, 0.f, 0.f);

    switch(0)
    {
        case 1:
            World = RandomScene();
            Background = Color(0.70f, 0.80f, 1.00f);
            LookFrom = Point3(13.0f, 2.0f, 3.0f);
            LookAt = Point3(0.0f, 0.0f, 0.0f);
            FOV = 20.0f;
            Aperture = 0.1f;
            break;
        case 2:
            World = TwoSpheres();
            Background = Color(0.70f, 0.80f, 1.00f);
            LookFrom = Point3(13.0f, 2.0f, 3.0f);
            LookAt = Point3(0.0f, 0.0f, 0.0f);
            FOV = 20.0;
            break;
        case 3:
            World = TwoPerlinSpheres();
            Background = Color(0.70f, 0.80f, 1.00f);
            LookFrom = Point3(13.0f, 2.0f, 3.0f);
            LookAt = Point3(0.0f, 0.0f, 0.0f);
            FOV = 20.0f;
            break;
        case 4:
            World = Earth();
            Background = Color(0.70f, 0.80f, 1.00f);
            LookFrom = Point3(13.0f, 2.0f, 3.0f);
            LookAt = Point3(0.0f, 0.0f, 0.0f);
            FOV = 20.0f;
            break;
        case 5:
            World = SimpleLight();
            SamplesPerPixel = 400;
            Background = Color(0.0f, 0.0f, 0.0f);
            LookFrom = Point3(26.0f, 3.0f, 6.0f);
            LookAt = Point3(0.0f, 2.0f, 0.0f);
            FOV = 20.0f;
            break;
        case 6:
            World = CornellBox();
            AspectRatio = 1.0f;
            ImageWidth = 600;
            ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
            SamplesPerPixel = 200;
            Background = Color(0.0f, 0.0f, 0.0f);
            LookFrom = Point3(278.0f, 278.0f, -800.0f);
            LookAt = Point3(278.0f, 278.0f, 0.0f);
            FOV = 40.0f;
            break;
        case 7:
            World = CornellSmoke();
            AspectRatio = 1.0f;
            ImageWidth = 600;
            ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
            SamplesPerPixel = 200;
            Background = Color(0.0f, 0.0f, 0.0f);
            LookFrom = Point3(278.0f, 278.0f, -800.0f);
            LookAt = Point3(278.0f, 278.0f, 0.0f);
            FOV = 40.0f;
            break;
        default:
        case 8:
            World = FinalScene();
            AspectRatio = 1.0f;
            ImageWidth = 800;
            ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
            SamplesPerPixel = 10000;
            Background = Color(0.0f, 0.0f, 0.0f);
            LookFrom = Point3(478.0f, 278.0f, -600.0f);
            LookAt = Point3(278.0f, 278.0f, 0.0f);
            FOV = 40.0f;
            break;
    }

    // Camera

    Vector3 Up(0.0f, 1.0f, 0.0f);
    auto DistToFocus = 10.0f;
    Camera Cam(LookFrom, LookAt, Up, FOV, AspectRatio, Aperture, DistToFocus, 0.0f, 1.0f);

    // Render

    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    auto StartTime = std::chrono::system_clock::now();

#define MT 1
#if MT
    Color** PixelData = new Color*[ImageHeight];
    for(int i = 0; i < ImageHeight; i++)
    {
        PixelData[i] = new Color[ImageWidth];
    }

    const int PixelNums = ImageHeight * ImageWidth;
    std::atomic<int> FinishedPixelNums = 0;
    
    auto CalculatePixelJob = [PixelData, SamplesPerPixel, PixelNums, ImageWidth, ImageHeight, &Cam, &Background, &World, &FinishedPixelNums](int Start, int End)
    {
        for(int Index = Start; Index < End; Index++)
        {
            int i = Index % ImageWidth;
            int j = Index / ImageWidth;

            Color PixelColor(0.0f, 0.0f, 0.0f);
            // Do antialiasing by random super sampling
            for (int s = 0; s < SamplesPerPixel; ++s) 
            {
                auto u = (i + RandomFloat()) / (ImageWidth - 1);
                auto v = (j + RandomFloat()) / (ImageHeight - 1);
                Ray r = Cam.GetRay(u, v);
                PixelColor += RayColor(r, Background, World, MaxDepth);
            }

            PixelData[i][j] = PixelColor;

            FinishedPixelNums++;
            std::cerr << "\rProgress: " << (FinishedPixelNums * 1.0f / PixelNums) * 100.0f << ' ' << std::flush;
        }
    };

    ParallelFor(PixelNums, CalculatePixelJob, true);

    for (int j = ImageHeight - 1; j >= 0; --j) 
    {
        for (int i = 0; i < ImageWidth; ++i) 
        {
            WriteColor(std::cout, PixelData[i][j], SamplesPerPixel);
        }
    }

    for(int i = 0;i < ImageHeight;i++)
    {
        delete []PixelData[i];
    }
    delete []PixelData;
#else
    for (int j = ImageHeight - 1; j >= 0; --j) 
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < ImageWidth; ++i) 
        {
            Color PixelColor(0.0f, 0.0f, 0.0f);
            // Do antialiasing by random super sampling
            for (int s = 0; s < SamplesPerPixel; ++s) 
            {
                auto u = (i + RandomFloat()) / (ImageWidth - 1);
                auto v = (j + RandomFloat()) / (ImageHeight - 1);
                Ray r = Cam.GetRay(u, v);
                PixelColor += RayColor(r, Background, World, MaxDepth);
            }
            WriteColor(std::cout, PixelColor, SamplesPerPixel);
        }
    }
#endif

    auto EndTime = std::chrono::system_clock::now();
    auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);

    std::cerr << "\nDone.\n";
    std::cerr << "Time Cost: "
              << double(Duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
              << " s\n";
}