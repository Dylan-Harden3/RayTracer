#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "Shape.h"

class Plane : public Shape {
public:
	bool hit(Ray ray, float t0, float t1, Hit& hit);
};

#endif // !PLANE_H
