#pragma once

#include <optional>
#include <vector>
#include <memory>

#include "Vec3.h"
#include "Ray.h"
#include "Hit.h"
#include "Light.h"

struct Material
{
    Vec3 diffuse;
    Vec3 specular;
    Vec3 ambient;
    float exponent;
    Material(const Vec3 &diffuse, const Vec3 &specular, const Vec3 &ambient, float exponent)
        : diffuse(diffuse), specular(specular), ambient(ambient), exponent(exponent) {}
};

class Shape
{
public:
    virtual std::optional<Hit> intersects(const Ray &r) const = 0;
    virtual Vec3 getNormal(const Vec3 &position) const = 0;
    virtual ~Shape() = default;

    Vec3 getColor(const Ray &ray, const Hit &hit, const std::vector<Light> &lights, const std::vector<std::unique_ptr<Shape>> &scene) const;

protected:
    explicit Shape(Material material) : material(material) {}
    Material material;
};

class Sphere : public Shape
{
public:
    Sphere(const Vec3 &center, float radius, Material material)
        : Shape(material), center(center), radius(radius) {}

    std::optional<Hit> intersects(const Ray &r) const override;
    Vec3 getNormal(const Vec3 &position) const override;

private:
    Vec3 center;
    float radius;
};

class Plane : public Shape
{
public:
    Plane(const Vec3 &position, const Vec3 &normal, Material material)
        : Shape(material), position(position), normal(normal) {}

    std::optional<Hit> intersects(const Ray &r) const override;
    Vec3 getNormal(const Vec3 &position) const override;

private:
    Vec3 position, normal;
};