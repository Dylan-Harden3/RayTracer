#pragma once

#include "Vec3.h"

struct Ray
{
    Ray(const Vec3 &origin, const Vec3 &direction) : origin(origin), direction(direction) {}
    Vec3 origin, direction;
    Vec3 at(float distance) const { return origin + direction * distance; }
};