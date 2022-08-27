#pragma once
#ifndef BUNNY_H
#define BUNNY_H

#include "Shape.h"

class Bunny : public Shape {
	public:
		bool hit(Ray ray, float t0, float t1, Hit& hit);
		bool bSphereHit(Ray ray, float t0, float t1, Hit& hit);
		bool meshHit(Ray ray, float t0, float t1, Hit& hit);
};

#endif 
