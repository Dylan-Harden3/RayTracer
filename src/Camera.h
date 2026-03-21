#pragma once

#include <vector>
#include <cmath>
#include "Vec3.h"
#include "Ray.h"

class Camera
{
public:
    Camera(const Vec3 &position, float fov) : position(position), fov(fov)
    {
        bounds = tan((fov / 2) * (PI / 180));
    }
    std::vector<Ray> getRays(int w, int h, const Vec3 &look_at, float plane_distance = 1.0f) const
    {
        Vec3 forward = look_at - position;
        forward.normalize();

        Vec3 world_up(0.0f, 1.0f, 0.0f);
        Vec3 right = forward.cross(world_up);
        right.normalize();

        Vec3 up = right.cross(forward);
        up.normalize();

        Vec3 plane_center = position + (plane_distance * forward);

        float aspect = static_cast<float>(w) / static_cast<float>(h);
        float half_height = bounds * plane_distance;
        float half_width = half_height * aspect;

        std::vector<Ray> rays;
        rays.reserve(static_cast<std::size_t>(w) * static_cast<std::size_t>(h));
        for (std::size_t x = 0; x < static_cast<std::size_t>(w); x++)
        {
            for (std::size_t y = 0; y < static_cast<std::size_t>(h); y++)
            {
                float centered_x = ((x + 0.5f) / static_cast<float>(w) * 2.0f - 1.0f) * half_width;
                float centered_y = ((y + 0.5f) / static_cast<float>(h) * 2.0f - 1.0f) * half_height;

                Vec3 pixel_point = plane_center + (centered_x * right) + (centered_y * up);
                Vec3 direction = pixel_point - position;
                direction.normalize();
                rays.push_back(Ray(Vec3(position), direction));
            }
        }
        return rays;
    }

private:
    Vec3 position;
    float fov, bounds;
    const float PI = 3.14159265358979323846;
};
