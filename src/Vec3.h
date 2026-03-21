#pragma once

#include <cmath>

struct Vec3
{
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) {}
    Vec3 &operator=(const Vec3 &rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    Vec3 operator+(const Vec3 &rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vec3 operator-(const Vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vec3 operator*(const float c) const { return {c * x, c * y, c * z}; }
    Vec3 operator+(const float c) const { return {c + x, c + y, c + z}; }

    float dot(const Vec3 &other) const { return x * other.x + y * other.y + z * other.z; }
    Vec3 cross(const Vec3 &other) const
    {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }
    void normalize()
    {
        float magnitude = std::pow(x * x + y * y + z * z, 0.5);
        x = x / magnitude;
        y = y / magnitude;
        z = z / magnitude;
    }

    float distance(const Vec3 &other) const
    {
        return std::sqrt(std::pow(other.x - x, 2.0f) + std::pow(other.y - y, 2.0f) + std::pow(other.z - z, 2.0f));
    }
};

inline Vec3 operator*(float c, const Vec3 &v) { return v * c; }
inline Vec3 operator+(float c, const Vec3 &v) { return v + c; }