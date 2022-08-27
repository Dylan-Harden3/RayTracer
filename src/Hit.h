#pragma once
#ifndef HIT_H
#define HIT_H

#include <glm/glm.hpp>

class Hit
{
	public:
		Hit() : x(0), n(0), t(0) {}
		Hit(const glm::vec3& x, const glm::vec3& n, float t) { this->x = x; this->n = n; this->t = t; }
		glm::vec3 x; // position
		glm::vec3 n; // normal
		float t; // distance
};

#endif // !HIT_H
