#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <vector>

class Camera {
	public:
		glm::vec3 position;
		float fov, aspectRatio;
		int width, height;

		float bounds;

		std::vector<glm::vec3> pixelPositions;

		Camera(glm::vec3 pos, int w, int h);
		Camera() {};
		void setImagePixels();

		bool sceneEight = false;
};

#endif