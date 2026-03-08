#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <vector>
#include <cmath>

class Camera {
    public:
        Camera(const Vec3& position, float fov) : position(position), fov(fov) {
            bounds = tan((fov / 2) * (PI / 180));
        }
        std::vector<Ray> getRays(int w, int h, int plane_z) const {
            float max = 2 * bounds;
            std::vector<Ray> rays;
            for (std::size_t i = 0; i < h; i++) {
                for (std::size_t j = 0; j < w; j++) {
                    float centered_x = (i + 0.5f) / h * max - bounds;
                    float centered_y = (j + 0.5f) / w * max - bounds;
                    
                    rays.push_back(Ray(Vec3(position), Vec3(centered_x, centered_y, plane_z) - position));
                }        
            }
            return rays;
        }
    private:
        Vec3 position;
        float fov, bounds;
        const float PI = 3.14159265358979323846; 
};
