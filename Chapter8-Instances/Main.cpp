#include "RTWeekend.h"

#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "MovingSphere.h"
#include "AARect.h"
#include "Box.h"

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
    auto Aperture = 0.1f;
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
        default:
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
    }

    // Camera

    Vector3 Up(0.0f, 1.0f, 0.0f);
    auto DistToFocus = 10.0f;
    Camera Cam(LookFrom, LookAt, Up, FOV, AspectRatio, Aperture, DistToFocus, 0.0f, 1.0f);

    // Render

    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

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

    std::cerr << "\nDone.\n";
}