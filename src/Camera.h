#pragma once

#include <vector>
#include <cmath>
#include "Vec3.h"
#include "Ray.h"

class Camera {
public:
    Camera(const Vec3& position, float fov) : position(position), fov(fov) {
        bounds = tan((fov / 2) * (PI / 180));
    }
    std::vector<Ray> getRays(int w, int h, int plane_z) const {
        float max = 2 * bounds;
        std::vector<Ray> rays;
        rays.reserve(static_cast<std::size_t>(w) * static_cast<std::size_t>(h));
        for (std::size_t x = 0; x < static_cast<std::size_t>(w); x++) {
            for (std::size_t y = 0; y < static_cast<std::size_t>(h); y++) {
                float centered_x = (x + 0.5f) / w * max - bounds;
                float centered_y = (y + 0.5f) / h * max - bounds;
                
                Vec3 direction = Vec3(centered_x, centered_y, plane_z) - position;
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
