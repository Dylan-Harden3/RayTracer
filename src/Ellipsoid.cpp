#include "Ellipsoid.h"
#include "MatrixStack.h"
#include <memory>

bool Ellipsoid::hit(Ray ray, float t0, float t1, Hit& hit) {

	auto M = std::make_shared<MatrixStack>();
	M->translate(this->position);
	M->scale(this->Escale);
	glm::mat4 E = M->topMatrix();
	
	glm::vec3 pPrime = glm::inverse(E) * glm::vec4(ray.p, 1);
	glm::vec3 vPrime = glm::normalize(glm::inverse(E) * glm::vec4(ray.v, 0));

	float a = glm::dot(vPrime, vPrime);
	float b = 2 * glm::dot(vPrime, pPrime);
	float c = glm::dot(pPrime, pPrime) - 1;
	float d = (b * b) - (4 * a * c);

	if (d > 0) {
		float tPrime = std::min((-b + pow(d, 0.5)) / (2 * a), (-b - pow(d, 0.5)) / (2 * a));

		glm::vec3 xPrime = pPrime + tPrime * vPrime;

		glm::vec3 x = E * glm::vec4(xPrime, 1);
		glm::vec3 n = glm::normalize(glm::transpose(glm::inverse(E)) * glm::vec4(xPrime, 0));

		float t = glm::length(x - ray.p);

		if (glm::dot(ray.v, x - ray.p) < 0) {
			t = -t;
		}
		hit = Hit(x, n, t);

		return t > 0 && t < t1;
	}

	return false;
}