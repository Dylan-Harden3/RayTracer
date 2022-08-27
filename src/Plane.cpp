#include "Plane.h"

bool Plane::hit(Ray ray, float t0, float t1, Hit& hit) {

	float t = glm::dot(this->Pnorm, this->Pc - ray.p) / glm::dot(this->Pnorm, ray.v);

	glm::vec3 x = ray.p + t * ray.v;

	hit = Hit(x, this->Pnorm, t);

	return t > t0 && t < t1;

}
