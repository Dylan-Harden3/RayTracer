#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Camera::Camera(int x, int y, int z, float fov): x_(x), y_(y), z_(z), fov_(fov) {
    bounds_ = tan((fov_ / 2) * (PI / 180));
};

void Camera::getRays(int w, int h,int x, int y, int z) {
    float max = 2 * bounds_;
    for (std::size_t i = 0; i < h; i++) {
        for (std::size_t j = 0; j < w; j++) {
            float centered_x = (i + 0.5f) / h * max - bounds_;
            float centered_y = (j + 0.5f) / w * max - bounds_;
            
            std::cout << centered_x - x_ << ' ' << centered_y - y_ << ' ' << z - z_ << '\n';
        }        
    }
};