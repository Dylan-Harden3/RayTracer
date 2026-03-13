#include "Hit.h"
#include "Shape.h"
#include "Ray.h"

Hit::Hit(const Vec3 &position, const Vec3 &normal, float distance, const Shape *shape)
    : position(position), normal(normal), distance(distance), shape(shape) {}

Hit &Hit::operator=(const Hit &rhs)
{
    position = rhs.position;
    normal = rhs.normal;
    distance = rhs.distance;
    shape = rhs.shape;
    return *this;
}
