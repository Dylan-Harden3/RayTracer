#include <cmath>
#include <optional>
#include <algorithm>

#include <vector>
#include "Shape.h"

std::optional<Hit> Sphere::intersects(const Ray& r) const {
    Vec3 pc = r.origin - center;
    float a = r.direction.dot(r.direction);
    float b = 2.0f * (r.direction.dot(pc));
    float c = pc.dot(pc) - radius * radius;
    float d = b * b - 4.0f * a * c;

    if (d < 0.0f) return std::nullopt;

    float dist1 = (-b - std::sqrt(d)) / (2.0f * a);
    float dist2 = (-b + std::sqrt(d)) / (2.0f * a);

    if (dist1 < 0.0f && dist2 < 0.0f) return std::nullopt;

    float dist = (dist1 > 0.0f) ? dist1 : dist2;
    Vec3 hit_point = r.at(dist);
    return Hit(hit_point, getNormal(hit_point), dist, this);
}

Vec3 Sphere::getNormal(const Vec3& position) const {
    Vec3 normal = (position - center);
    normal.normalize();
    return normal;
}

Vec3 Sphere::getColor(const Ray& ray, const Hit& hit, const std::vector<Light>& lights) const {
    Vec3 color(ambient);

    for (const Light& light : lights) {
        Vec3 l = light.position - hit.position;
        l.normalize();

        Vec3 v = ray.origin - hit.position;
        v.normalize();

        Vec3 h = l + v;
        h.normalize();

        Vec3 l_diffuse  = diffuse  * light.intensity * std::max(0.0f, hit.normal.dot(l));
        Vec3 l_specular = specular * light.intensity * std::max(0.0f, std::pow(hit.normal.dot(h), exponent));

        color = color + l_diffuse + l_specular;
    }
    
    return color * 255.0f;
}
