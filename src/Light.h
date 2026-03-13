#pragma once

struct Light
{
    Light(const Vec3 &position, float intensity) : position(position), intensity(intensity) {}
    Vec3 position;
    float intensity;
};