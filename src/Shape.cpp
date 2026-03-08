#include "Shape.h"
#include "Ray.h"
#include <iostream>
#include <cmath>

bool Sphere::intersects(const Ray& r) const {
    Vec3 pc = r.origin - center;
    float a = r.direction.dot(r.direction);
    float b = 2 * (r.direction.dot(pc));
    float c = pc.dot(pc) - radius*radius;
    float d = b*b - 4*a*c;
    if (d < 0.0) {
        return false;
    }
    float dist1 = (-b + sqrt(d)) / (2*a);
    float dist2 = (-b - sqrt(d)) / (2*a);

    if (dist1 < 0.0 and dist2 < 0.0) {
        return false;
    }

    return true;
}