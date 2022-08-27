#pragma once
#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include "Shape.h"

class Ellipsoid : public Shape {
public:
	bool hit(Ray ray, float t0, float t1, Hit& hit);
};

#endif // !ELLIPSOID_H
