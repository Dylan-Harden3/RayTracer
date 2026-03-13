#include <cmath>
#include <optional>
#include <algorithm>
#include <vector>
#include <memory>

#include "Shape.h"
#include "Ray.h"

Vec3 Shape::getColor(const Ray &ray, const Hit &hit, const std::vector<Light> &lights, const std::vector<std::unique_ptr<Shape>> &scene) const
{
    Vec3 color(material.ambient);

    for (const Light &light : lights)
    {
        Vec3 l = light.position - hit.position;
        l.normalize();

        bool in_shadow = false;
        Ray shadow_ray(hit.position, l);
        for (const auto &shape : scene)
        {
            auto shadow_hit = shape->intersects(shadow_ray);
            if (shadow_hit && shadow_hit->distance > 0.001f && shadow_hit->distance < hit.position.distance(light.position))
            {
                in_shadow = true;
                break;
            }
        }

        if (!in_shadow)
        {
            Vec3 v = ray.origin - hit.position;
            v.normalize();

            Vec3 h = l + v;
            h.normalize();

            Vec3 l_diffuse = material.diffuse * light.intensity * std::max(0.0f, hit.normal.dot(l));
            float ndoth = std::max(0.0f, hit.normal.dot(h));
            Vec3 l_specular = material.specular * light.intensity * std::pow(ndoth, material.exponent);

            color = color + l_diffuse + l_specular;
        }
    }

    return color * 255.0f;
}

std::optional<Hit> Sphere::intersects(const Ray &r) const
{
    Vec3 pc = r.origin - center;
    float a = r.direction.dot(r.direction);
    float b = 2.0f * (r.direction.dot(pc));
    float c = pc.dot(pc) - radius * radius;
    float d = b * b - 4.0f * a * c;

    if (d < 0.0f)
        return std::nullopt;

    float dist1 = (-b - std::sqrt(d)) / (2.0f * a);
    float dist2 = (-b + std::sqrt(d)) / (2.0f * a);

    if (dist1 < 0.0f && dist2 < 0.0f)
        return std::nullopt;

    float dist = (dist1 > 0.0f) ? dist1 : dist2;
    Vec3 hit_point = r.at(dist);
    return Hit(hit_point, getNormal(hit_point), dist, this);
}

Vec3 Sphere::getNormal(const Vec3 &position) const
{
    Vec3 normal = (position - center);
    normal.normalize();
    return normal;
}

std::optional<Hit> Plane::intersects(const Ray &r) const
{
    bool is_parallel = r.direction.dot(normal) == 0;
    if (is_parallel)
        return std::nullopt;

    float dist = (position - r.origin).dot(normal) / (r.direction.dot(normal));
    if (dist < 0.0f)
        return std::nullopt;

    Vec3 hit_point = r.at(dist);

    return Hit(hit_point, getNormal(hit_point), dist, this);
}

Vec3 Plane::getNormal(const Vec3 &position) const
{
    return normal;
    if (normal.x != 0.0f)
    {
        return Vec3(normal.x, position.y, position.z);
    }
    else if (normal.y != 0.0f)
    {
        return Vec3(position.x, normal.y, position.z);
    }
    else
    {
        return Vec3(position.x, position.y, normal.z);
    }
}
