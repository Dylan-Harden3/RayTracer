#pragma once

#include <vector>
#include "Vec3.h"
#include "Light.h"

class Shape;
struct Ray;

struct Hit {
    Vec3 position, normal;
    float distance;
    const Shape* shape = nullptr;

    Hit() = default;
    Hit(const Vec3& position, const Vec3& normal, float distance, const Shape* shape);
    Hit& operator=(const Hit& rhs);

    Vec3 getColor(const Ray& ray, const std::vector<Light>& lights) const;
};