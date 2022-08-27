#pragma once
#ifndef RAY_H
#define RAY_H
#include <glm/glm.hpp>

class Ray {
	public:
		Ray(glm::vec3 _p, glm::vec3 _v) : p(_p), v(_v) {}
		glm::vec3 p; // position
		glm::vec3 v; // direction
};

#endif // !RAY_H
