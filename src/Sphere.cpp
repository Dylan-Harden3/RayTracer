#include "Sphere.h"
#include <algorithm> // std::min
#include <iostream>
#include <glm/glm.hpp>

bool Sphere::hit(Ray ray, float t0, float t1, Hit& hit) {

	glm::vec3 pc = ray.p - this->position;

	float a = dot(ray.v, ray.v);
	float b = 2 * dot(ray.v, pc);
	float c = dot(pc, pc) - (this->scale * this->scale);
	float d = (b * b) - (4 * a * c);

	if (d > 0) {
		float t = std::min((-b - pow(d, 0.5)) / (2 * a), (-b + pow(d, 0.5)) / (2 * a));

		glm::vec3 x = ray.p + t * ray.v;
		glm::vec3 n = (x - this->position) / this->scale;
		hit = Hit(x, n, t);
	
		return t > 0 && t < t1;
	}
	else {
		return false;
	}

}