#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
	public:
		glm::vec3 position;
		float intensity;
		Light(glm::vec3 _position, float _intensity) : position(_position), intensity(_intensity) {}
};

#endif