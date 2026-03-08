#pragma once
#include "Vec3.h"
#include "Ray.h"

class Shape {
public:
    virtual bool intersects(const Ray& r) const = 0;
    virtual ~Shape() = default;
};

class Sphere : public Shape {
public:
    Sphere(const Vec3& center, float radius): center(center), radius(radius) {}
    bool intersects(const Ray& r) const;

private:
    Vec3 center;
    float radius;
};