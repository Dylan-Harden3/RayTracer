#pragma once

#include <optional>
#include <vector>
#include "Vec3.h"
#include "Ray.h"
#include "Hit.h"
#include "Light.h"

class Shape {
public:
    virtual std::optional<Hit> intersects(const Ray& r) const = 0;
    virtual Vec3 getNormal(const Vec3& position) const = 0;
    virtual Vec3 getColor(const Ray& ray, const Hit& hit, const std::vector<Light>& lights) const = 0;
    virtual ~Shape() = default;
};

class Sphere : public Shape {
public:
    Sphere(const Vec3& center,
           float radius,
           const Vec3& diffuse,
           const Vec3& specular,
           const Vec3& ambient,
           float exponent)
        : center(center), radius(radius), diffuse(diffuse), specular(specular), ambient(ambient), exponent(exponent) {}

    Sphere(const Sphere& other)
        : center(other.center),
          radius(other.radius),
          diffuse(other.diffuse),
          specular(other.specular),
          ambient(other.ambient),
          exponent(other.exponent) {}

    std::optional<Hit> intersects(const Ray& r) const override;
    Vec3 getNormal(const Vec3& position) const override;
    Vec3 getColor(const Ray& ray, const Hit& hit, const std::vector<Light>& lights) const override;

private:
    Vec3 center;
    float radius;
    Vec3 diffuse, specular, ambient;
    float exponent;
};