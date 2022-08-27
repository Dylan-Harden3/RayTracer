#pragma once
#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include "Hit.h"
#include "Ray.h"
#include <iostream>
#include <vector>

class Shape {
	public:
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		float exponent;
		float scale;
		virtual bool hit(Ray ray, float t0, float t1, Hit& hit) { std::cout << "shape.hit()" << std::endl; return false; }
		glm::vec3 Escale;
		bool isReflective;
		glm::vec3 Pnorm;
		glm::vec3 Pc;

		float rotation;
		glm::vec3 center;
		std::vector<float> posBuf;
		std::vector<float> norBuf;

		Shape() {}
};

#endif // !SHAPE_H
