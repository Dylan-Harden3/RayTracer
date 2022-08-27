#include "Bunny.h"
#include <math.h>
#include "MatrixStack.h"

using namespace std;

#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2];

/* code rewritten to do tests on the sign of the determinant */
/* the division is before the test of the sign of the det    */
/* and one CROSS has been moved out from the if-else if-else */
int intersect_triangle3(float orig[3], float dir[3],
    float vert0[3], float vert1[3], float vert2[3],
    float* t, float* u, float* v)
{
    float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    float det, inv_det;

    /* find vectors for two edges sharing vert0 */
    SUB(edge1, vert1, vert0);
    SUB(edge2, vert2, vert0);

    /* begin calculating determinant - also used to calculate U parameter */
    CROSS(pvec, dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    det = DOT(edge1, pvec);

    /* calculate distance from vert0 to ray origin */
    SUB(tvec, orig, vert0);
    inv_det = 1.0 / det;

    CROSS(qvec, tvec, edge1);

    if (det > EPSILON)
    {
        *u = DOT(tvec, pvec);
        if (*u < 0.0 || *u > det)
            return 0;

        /* calculate V parameter and test bounds */
        *v = DOT(dir, qvec);
        if (*v < 0.0 || *u + *v > det)
            return 0;

    }
    else if (det < -EPSILON)
    {
        /* calculate U parameter and test bounds */
        *u = DOT(tvec, pvec);
        if (*u > 0.0 || *u < det)
            return 0;

        /* calculate V parameter and test bounds */
        *v = DOT(dir, qvec);
        if (*v > 0.0 || *u + *v < det)
            return 0;
    }
    else return 0;  /* ray is parallell to the plane of the triangle */

    *t = DOT(edge2, qvec) * inv_det;
    (*u) *= inv_det;
    (*v) *= inv_det;

    return 1;
}

bool Bunny::bSphereHit(Ray ray, float t0, float t1, Hit& hit) {

	glm::vec3 pc = ray.p - this->center;

	float a = dot(ray.v, ray.v);
	float b = 2 * dot(ray.v, pc);
	float c = dot(pc, pc) - (this->scale * this->scale);
	float d = (b * b) - (4 * a * c);

	if (d > 0) {
		float t = std::min((-b - pow(d, 0.5)) / (2 * a), (-b + pow(d, 0.5)) / (2 * a));

		glm::vec3 x = ray.p + t * ray.v;
		glm::vec3 n = (x - this->center) / this->scale;
		hit = Hit(x, n, t);

		return t > t0 && t < t1;
	}
	else {
		return false;
	}
}

bool Bunny::meshHit(Ray ray, float t0, float t1, Hit& hit) {

	float actualT = 0.0;
	float actualU = 0.0;
	float actualV = 0.0;

	float* t = &actualT;
	float* u = &actualU;
	float* v = &actualV;

    auto M = std::make_shared<MatrixStack>();
    M->translate(this->position);
    M->scale(this->scale);
    M->rotate(this->rotation, 1.0, 0.0, 0.0);
    glm::mat4 E = M->topMatrix();

    glm::vec3 rp = glm::inverse(E) * glm::vec4(ray.p, 1);
    glm::vec3 rn = glm::inverse(E) * glm::vec4(ray.v, 0);

    float orig[3] = { rp.x, rp.y, rp.z };
    float dir[3] = { rn.x, rn.y, rn.z };

    vector<Hit> hitsVector;

	for (int i = 0; i < this->posBuf.size(); i += 9) {

		float vert0[3] = { this->posBuf[i], this->posBuf[i + 1], this->posBuf[i + 2] };
		float vert1[3] = { this->posBuf[i+3], this->posBuf[i + 4], this->posBuf[i + 5] };
		float vert2[3] = { this->posBuf[i+6], this->posBuf[i + 7], this->posBuf[i + 8] };
		
        bool hits = intersect_triangle3(orig, dir, vert0, vert1, vert2, t, u, v) == 1;

        if (actualT <= t0 || actualT >= t1) {
            hits = false;
        }

		float nor0[3] = { this->norBuf[i], this->norBuf[i + 1], this->norBuf[i + 2] };
		float nor1[3] = { this->norBuf[i + 3], this->norBuf[i + 4], this->norBuf[i + 5] };
		float nor2[3] = { this->norBuf[i + 6], this->norBuf[i + 7], this->norBuf[i + 8] };

		if (hits) {
            float v0B = 1 - actualU - actualV;
			glm::vec3 pos = glm::vec3(vert0[0] * v0B + vert1[0] * actualU + vert2[0] * actualV, vert0[1] * v0B + vert1[1] * actualU + vert2[1] * actualV, vert0[2] * v0B + vert1[2] * actualU + vert2[2] * actualV);
			glm::vec3 nor = glm::vec3(nor0[0] * v0B + nor1[0] * actualU + nor2[0] * actualV, nor0[1] * v0B + nor1[1] * actualU + nor2[1] * actualV, nor0[2] * v0B + nor1[2] * actualU + nor2[2] * actualV);
			
            hit = Hit(pos, normalize(nor), actualT);

            hit.x = E * glm::vec4(hit.x, 1);
            hit.n = glm::inverse(glm::transpose(E)) * glm::vec4(hit.n, 0);

            float tDist = glm::length(hit.x - ray.p);

            if (glm::dot(ray.v, hit.x - ray.p) < 0) {
                tDist = -tDist;
            }

            if (tDist > 0) hitsVector.push_back(Hit(hit));
		}
	}

    Hit minHit;
    if (hitsVector.size()) {
        minHit = hitsVector[0];
    }

    for (int i = 0; i < hitsVector.size(); i++) {
        if (hitsVector[i].t < minHit.t) {
            minHit = hitsVector[i];
        }
    }

    hit = minHit;
    return hitsVector.size();
}

bool Bunny::hit(Ray ray, float t0, float t1, Hit& hit) {

	if (this->bSphereHit(ray, t0, t1, hit)) {
		return this->meshHit(ray, t0, t1, hit);
	}
	return false;
}