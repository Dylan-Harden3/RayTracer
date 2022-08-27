#include "Camera.h"
#include <iostream>
#include <algorithm>

using namespace glm;

Camera::Camera(glm::vec3 pos, int w, int h) : position(pos), width(w), height(h) {
	aspectRatio = 1.0;
	fov = 45.0;
	float pi = 3.14159265358979323846f;
	bounds = tan((fov / 2) * (pi / 180));

	setImagePixels();
}


float roundOff(float val, unsigned char p) {
	float p10 = pow(10.0f, (float)p);
	return round(val * p10) / p10;
}

void Camera::setImagePixels() {

	pixelPositions.clear();

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			float max = 2 * bounds;

			float y1 = roundOff(((float)i / height) * max - bounds, 4);
			float x1 = roundOff(((float)j / width) * max - bounds, 4);

			float y2 = roundOff(((float)(i + 1) / height) * max - bounds, 4);
			float x2 = roundOff(((float)(j + 1) / width) * max - bounds, 4);
			
			//std::cout << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;

			if (this->sceneEight) {
				pixelPositions.push_back(normalize(vec3(-2.0,  (y1 + y2) / 2, (x1 + x2) / 2) - this->position));
			}
			else {
				pixelPositions.push_back(normalize(vec3((x1 + x2) / 2, (y1 + y2) / 2, 4.0) - this->position));
			}

		}
	}

}
