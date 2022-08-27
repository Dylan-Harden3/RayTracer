#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape {
	public:
		bool hit(Ray ray, float t0, float t1, Hit& hit);
};

#endif // !SPHERE_H
