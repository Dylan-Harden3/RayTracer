#include <iostream>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"

#include "Camera.h"
#include "Shape.h"
#include "Light.h"
#include "Hit.h"
#include "Sphere.h"
#include "Ellipsoid.h"
#include "Plane.h"

#include "Bunny.h"
#include <fstream>

#include <algorithm>
#include <numeric>

using namespace std;
using namespace glm;

double RANDOM_COLORS[7][3] = {
	{0.0000,    0.4470,    0.7410},
	{0.8500,    0.3250,    0.0980},
	{0.9290,    0.6940,    0.1250},
	{0.4940,    0.1840,    0.5560},
	{0.4660,    0.6740,    0.1880},
	{0.3010,    0.7450,    0.9330},
	{0.6350,    0.0780,    0.1840},
};

std::vector<Light> lights;
std::vector<Shape*> shapes;
std::vector<glm::vec3> pixelColors;

vec3 getDiffuse(Light light, Hit hit, Shape* shape) {

	vec3 l = normalize(light.position - hit.x);
	return shape->diffuse * std::max(dot(l, normalize(hit.n)), 0.0f);

}

vec3 getSpecular(Ray ray, Light light, Hit hit, Shape* shape) {

	vec3 l = normalize(light.position - hit.x);
	vec3 e = normalize(ray.p - hit.x);
	vec3 h = normalize(l + e);

	return shape->specular * pow(std::max(dot(h, normalize(hit.n)), 0.0f), shape->exponent);

}

bool hit(Ray ray, float t0, float t1, Hit& hit, Shape*& shape) {

	Hit h = Hit();
	h.t = t1;

	bool hasHit = false;

	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i]->hit(ray, t0, t1, hit)) {
			hasHit = true;
			if (hit.t < h.t) {
				h = hit;
				shape = shapes[i];
			}
		}
	}

	hit = h;
	return hasHit;
}

vec3 getBPColor(Shape*& shape, Hit& rec, Ray& curRay) {

	vec3 color = shape->ambient;
	for (int l = 0; l < lights.size(); l++) {
		Ray sRay = Ray(vec3(rec.x), normalize(lights[l].position - rec.x));
		Hit sRec;
		Shape* fillShape;
		if (!hit(sRay, 0.05, length(lights[l].position - rec.x), sRec, fillShape)) {
			vec3 diffuse = getDiffuse(lights[l], rec, shape);
			vec3 specular = getSpecular(curRay, lights[l], rec, shape);
			//cout << "siffues: " << diffuse.x << " " << diffuse.y << " " << diffuse.z << " specular: " << specular.x << " " << specular.y << " " << specular.z << endl;
			color = color + lights[l].intensity * (diffuse + specular);
		}
	}
	return color;
}

vec3 computeColor(Shape*& shape, Hit& rec, Ray& curRay, int depth) {
	vec3 color = vec3(0.0, 0.0, 0.0);
	bool done_reflecting = false;

	if (!shape->isReflective) {
		color += getBPColor(shape, rec, curRay);
	}
	else {
		int c = 1;
		curRay.v = reflect(curRay.v, rec.n);
		curRay.p = rec.x;

		while (hit(curRay, 0.005, std::numeric_limits<float>::infinity(), rec, shape) && c < 10) {
			if (shape->isReflective) {
				color += getBPColor(shape, rec, curRay);
				curRay.v = reflect(curRay.v, rec.n);
				curRay.p = rec.x;
				c++;
			}
			else {
				color += getBPColor(shape, rec, curRay);
				break;
			}
		}
	}

	color.r = std::clamp(color.r, 0.0f, 1.0f);
	color.g = std::clamp(color.g, 0.0f, 1.0f);
	color.b = std::clamp(color.b, 0.0f, 1.0f);

	color *= 255.0f;
	return color;
}

void shootRays(Camera& camera, shared_ptr<Image> image, bool blur = false, int cycleNum = 0) {
	for (int i = 0; i < camera.pixelPositions.size(); i++) {
		Ray curRay = Ray(camera.position, camera.pixelPositions[i]);
		Hit rec;
		glm::vec3 color = vec3(0.0, 0.0, 0.0);
		Shape* shape;

		int x = i % image->getWidth();
		int y = i / image->getHeight();

		if (hit(curRay, 0.0, std::numeric_limits<float>::infinity(), rec, shape)) {
			//cout << "Ray: " << curRay.p.x << " " << curRay.p.y << " " << curRay.p.z << "->" << curRay.v.x << " " << curRay.v.y << " " << curRay.v.z << endl;
			
			color = computeColor(shape, rec, curRay, 0);
			image->setPixel(x, y, color.r, color.g, color.b);
		}
		else {
			image->setPixel(x, y, 0, 0, 0);
		}
	}
}

void sceneOne(int imageSize, string filename) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(0, 0, 5), imageSize, imageSize);

	lights.push_back(Light(vec3(-2.0, 1.0, 1.0), 1.0));

	Shape* redSphere = new Sphere();
	redSphere->position = vec3(-0.5, -1.0, 1.0);
	redSphere->scale = 1.0;
	redSphere->diffuse = vec3(1.0, 0.0, 0.0);
	redSphere->specular = vec3(1.0, 1.0, 0.5);
	redSphere->ambient = vec3(0.1, 0.1, 0.1);
	redSphere->exponent = 100.0;
	shapes.push_back(redSphere);

	Shape* greenSphere = new Sphere();
	greenSphere->position = vec3(0.5, -1.0, -1.0);
	greenSphere->scale = 1.0;
	greenSphere->diffuse = vec3(0.0, 1.0, 0.0);
	greenSphere->specular = vec3(1.0, 1.0, 0.5);
	greenSphere->ambient = vec3(0.1, 0.1, 0.1);
	greenSphere->exponent = 100.0;
	shapes.push_back(greenSphere);

	Shape* blueSphere = new Sphere();
	blueSphere->position = vec3(0.0, 1.0, 0.0);
	blueSphere->scale = 1.0;
	blueSphere->diffuse = vec3(0.0, 0.0, 1.0);
	blueSphere->specular = vec3(1.0, 1.0, 0.5);
	blueSphere->ambient = vec3(0.1, 0.1, 0.1);
	blueSphere->exponent = 100.0;
	shapes.push_back(blueSphere);

	shootRays(camera, image);
	image->writeToFile(filename);
}

void sceneThree(int imageSize, string fileName) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(0, 0, 5), imageSize, imageSize);

	lights.push_back(Light(vec3(1.0, 2.0, 2.0), 0.5));
	lights.push_back(Light(vec3(-1.0, 2.0, -1.0), 0.5));

	Shape* redEllipsoid = new Ellipsoid();
	redEllipsoid->position = vec3(0.5, 0.0, 0.5);
	redEllipsoid->Escale = vec3(0.5, 0.6, 0.2);
	redEllipsoid->diffuse = vec3(1.0, 0.0, 0.0);
	redEllipsoid->specular = vec3(1.0, 1.0, 0.5);
	redEllipsoid->ambient = vec3(0.1, 0.1, 0.1);
	redEllipsoid->exponent = 100.0;
	shapes.push_back(redEllipsoid);

	Shape* greenSphere = new Sphere();
	greenSphere->position = vec3(-0.5, 0.0, -0.5);
	greenSphere->scale = 1.0;
	greenSphere->diffuse = vec3(0.0, 1.0, 0.0);
	greenSphere->specular = vec3(1.0, 1.0, 0.5);
	greenSphere->ambient = vec3(0.1, 0.1, 0.1);
	greenSphere->exponent = 100.0;
	shapes.push_back(greenSphere);

	Shape* plane = new Plane();
	plane->Pc = vec3(0.0, -1.0, 0.0);
	plane->Pnorm = vec3(0, 1, 0);
	plane->diffuse = vec3(1.0, 1.0, 1.0);
	plane->specular = vec3(0.0, 0.0, 0.0);
	plane->ambient = vec3(0.1, 0.1, 0.1);
	plane->exponent = 0.0;
	shapes.push_back(plane);

	shootRays(camera, image);
	image->writeToFile(fileName);

}

void sceneFour(int imageSize, string fileName) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(0, 0, 5), imageSize, imageSize);

	lights.push_back(Light(vec3(-1.0, 2.0, 1.0), 0.5));
	lights.push_back(Light(vec3(0.5, -0.5, 0.0), 0.5));

	Shape* redSphere = new Sphere();
	redSphere->position = vec3(0.5, -0.7, 0.5);
	redSphere->scale = 0.3;
	redSphere->diffuse = vec3(1.0, 0.0, 0.0);
	redSphere->specular = vec3(1.0, 1.0, 0.5);
	redSphere->ambient = vec3(0.1, 0.1, 0.1);
	redSphere->exponent = 100.0;
	shapes.push_back(redSphere);

	Shape* blueSphere = new Sphere();
	blueSphere->position = vec3(1.0, -0.7, 0.0);
	blueSphere->scale = 0.3;
	blueSphere->diffuse = vec3(0.0, 0.0, 1.0);
	blueSphere->specular = vec3(1.0, 1.0, 0.5);
	blueSphere->ambient = vec3(0.1, 0.1, 0.1);
	blueSphere->exponent = 100.0;
	shapes.push_back(blueSphere);

	Shape* floor = new Plane();
	floor->Pc = vec3(0.0, -1.0, 0.0);
	floor->Pnorm = vec3(0, 1, 0);
	floor->diffuse = vec3(1.0, 1.0, 1.0);
	floor->specular = vec3(0.0, 0.0, 0.0);
	floor->ambient = vec3(0.1, 0.1, 0.1);
	floor->exponent = 0.0;
	shapes.push_back(floor);

	Shape* wall = new Plane();
	wall->Pc = vec3(0.0, 0.0, -3.0);
	wall->Pnorm = vec3(0.0, 0.0, 1.0);
	wall->diffuse = vec3(1.0, 1.0, 1.0);
	wall->specular = vec3(0);
	wall->ambient = vec3(0.1);
	wall->exponent = 0.0;
	shapes.push_back(wall);

	Shape* rSphere1 = new Sphere();
	rSphere1->position = vec3(-0.5, 0.0, -0.5);
	rSphere1->scale = 1.0;
	rSphere1->isReflective = true;
	shapes.push_back(rSphere1);

	Shape* rSphere2 = new Sphere();
	rSphere2->position = vec3(1.5, 0.0, -1.5);
	rSphere2->scale = 1.0;
	rSphere2->isReflective = true;
	shapes.push_back(rSphere2);

	shootRays(camera, image);
	image->writeToFile(fileName);

}

void sceneSix(int imageSize, string fileName, float radius, vec3 center, const vector<float>& norBuf, const vector<float>& posBuf) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(0, 0, 5), imageSize, imageSize);

	lights.push_back(Light(vec3(-1.0, 1.0, 1.0), 1.0));

	Shape* bunny = new Bunny();
	bunny->diffuse = vec3(0.0, 0.0, 1.0);
	bunny->specular = vec3(1.0, 1.0, 0.5);
	bunny->ambient = vec3(0.1, 0.1, 0.1);
	bunny->exponent = 100;
	bunny->position = vec3(0.0);
	bunny->center = center;
	bunny->scale = radius;
	bunny->posBuf = posBuf;
	bunny->norBuf = norBuf;

	shapes.push_back(bunny);

	shootRays(camera, image);
	image->writeToFile(fileName);

}

void sceneSeven(int imageSize, string fileName, float radius, vec3 center, const vector<float>& norBuf, const vector<float>& posBuf) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(0, 0, 5), imageSize, imageSize);

	lights.push_back(Light(vec3(1.0, 1.0, 2.0), 1.0));



	Shape* bunny = new Bunny();
	bunny->diffuse = vec3(0.0, 0.0, 1.0);
	bunny->specular = vec3(1.0, 1.0, 0.5);
	bunny->ambient = vec3(0.1, 0.1, 0.1);
	bunny->exponent = 100;
	bunny->position = vec3(0.3, -1.5, 0.0);
	bunny->rotation = 20.0 / 180 * 3.14;
	bunny->scale = 1.5;

	center = center + bunny->position;


	bunny->posBuf = posBuf;
	bunny->norBuf = norBuf;

	shapes.push_back(bunny);

	shootRays(camera, image);
	image->writeToFile(fileName);

}

void sceneEight(int imageSize, string fileName) {

	auto image = make_shared<Image>(imageSize, imageSize);

	Camera camera = Camera(vec3(-3, 0, 0), imageSize, imageSize);
	camera.fov = 60.0;
	float pi = 3.14159265358979323846f;
	camera.bounds = tan((camera.fov / 2) * (pi / 180));
	camera.sceneEight = true;
	camera.setImagePixels();

	lights.push_back(Light(vec3(-2.0, 1.0, 1.0), 1.0));

	Shape* redSphere = new Sphere();
	redSphere->position = vec3(-0.5, -1.0, 1.0);
	redSphere->scale = 1.0;
	redSphere->diffuse = vec3(1.0, 0.0, 0.0);
	redSphere->specular = vec3(1.0, 1.0, 0.5);
	redSphere->ambient = vec3(0.1, 0.1, 0.1);
	redSphere->exponent = 100.0;
	shapes.push_back(redSphere);

	Shape* greenSphere = new Sphere();
	greenSphere->position = vec3(0.5, -1.0, -1.0);
	greenSphere->scale = 1.0;
	greenSphere->diffuse = vec3(0.0, 1.0, 0.0);
	greenSphere->specular = vec3(1.0, 1.0, 0.5);
	greenSphere->ambient = vec3(0.1, 0.1, 0.1);
	greenSphere->exponent = 100.0;
	shapes.push_back(greenSphere);

	Shape* blueSphere = new Sphere();
	blueSphere->position = vec3(0.0, 1.0, 0.0);
	blueSphere->scale = 1.0;
	blueSphere->diffuse = vec3(0.0, 0.0, 1.0);
	blueSphere->specular = vec3(1.0, 1.0, 0.5);
	blueSphere->ambient = vec3(0.1, 0.1, 0.1);
	blueSphere->exponent = 100.0;
	shapes.push_back(blueSphere);

	shootRays(camera, image);
	image->writeToFile(fileName);

}

float getBoundingRadius(const vec3& avgPoint, const vector<float>& posBuf) {
	float maxLength = length(vec3(posBuf[0], posBuf[1], posBuf[2]) - avgPoint);
	for (int i = 0; i < posBuf.size(); i += 3) {
		vec3 curPoint = vec3(posBuf[i], posBuf[i + 1], posBuf[i + 2]);
		if (length(curPoint - avgPoint) > maxLength) {
			maxLength = length(curPoint - avgPoint);
		}
	}
	return maxLength;
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Usage: A1 meshfile" << endl;
		return 0;
	}

	int scene = atoi(argv[1]);
	int imageSize = atoi(argv[2]);
	string fileName = argv[3];

	string meshName(argv[4]);

	// Load geometry
	vector<float> posBuf; // list of vertex positions
	vector<float> norBuf; // list of vertex normals
	vector<float> texBuf; // list of vertex texture coords
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	vector<float> xs;
	vector<float> ys;
	vector<float> zs;

	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes

		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					float x = attrib.vertices[3 * idx.vertex_index + 0];
					float y = attrib.vertices[3 * idx.vertex_index + 1];
					float z = attrib.vertices[3 * idx.vertex_index + 2];

					xs.push_back(x);
					ys.push_back(y);
					zs.push_back(z);

					posBuf.push_back(x);
					posBuf.push_back(y);
					posBuf.push_back(z);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}

	if (scene == 6 || scene == 7) {
		float avgX = accumulate(xs.begin(), xs.end(), 0.0) / xs.size();
		float avgY = accumulate(ys.begin(), ys.end(), 0.0) / ys.size();
		float avgZ = accumulate(zs.begin(), zs.end(), 0.0) / zs.size();
		vec3 avgPoint = vec3(avgX, avgY, avgZ);
		float r = getBoundingRadius(avgPoint, posBuf);
		if (scene == 6) sceneSix(imageSize, fileName, r, avgPoint, norBuf, posBuf);
		if (scene == 7) sceneSeven(imageSize, fileName, r, avgPoint, norBuf, posBuf);
	}

	if (scene == 1) sceneOne(imageSize, fileName);
	if (scene == 2) sceneOne(imageSize, fileName);
	if (scene == 3) sceneThree(imageSize, fileName);
	if (scene == 4) sceneFour(imageSize, fileName);
	if (scene == 5) sceneFour(imageSize, fileName);
	if (scene == 8) sceneEight(imageSize, fileName);

	return 0;
}
