#include <iostream>
#include <string>
#include <memory>
#include <random>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "stb_image_write.h"
#include "Image.h"
#include "Camera.h"
#include "Vec3.h"
#include "Ray.h"
#include "Shape.h"
#include "Hit.h"
#include "Light.h"

double RANDOM_COLORS[7][3] = {
	{0.0000, 0.4470, 0.7410},
	{0.8500, 0.3250, 0.0980},
	{0.9290, 0.6940, 0.1250},
	{0.4940, 0.1840, 0.5560},
	{0.4660, 0.6740, 0.1880},
	{0.3010, 0.7450, 0.9330},
	{0.6350, 0.0780, 0.1840},
};

int main(int argc, char **argv)
{
	int width = 1024, height = 1024;
	Camera c(Vec3(-12.0f, 10.0f, 12.0f), 45.0f);
	std::vector<Ray> rays = c.getRays(width, height, Vec3(0.0f, -1.0f, 0.0f));
	Image image(width, height);

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> light_pos_xz(-12.0f, 12.0f);
	std::uniform_real_distribution<float> light_pos_y(10.0f, 14.0f);
	std::uniform_real_distribution<float> light_intensity(0.1f, 0.8f);

	std::vector<Light> lights;
	for (int i = 0; i < 3; i++)
	{
		lights.push_back(Light(
			Vec3(light_pos_xz(rng), light_pos_y(rng), light_pos_xz(rng)),
			light_intensity(rng)));
	}

	std::uniform_real_distribution<float> plane_color_dist(0.1f, 1.0f);
	Vec3 plane_diffuse(plane_color_dist(rng), plane_color_dist(rng), plane_color_dist(rng));

	std::vector<std::unique_ptr<Shape>> scene;
	scene.push_back(std::make_unique<Plane>(
		Vec3(0.0f, -1.0f, 0.0f),
		Vec3(0.0f, 1.0f, 0.0f),
		Material(
			plane_diffuse,
			Vec3(0.0f, 0.0f, 0.0f),
			0.1f * plane_diffuse,
			0.0f)));

	std::uniform_real_distribution<float> pos_dist(-10.0f, 10.0f);
	std::uniform_real_distribution<float> y_dist(0.25f, 2.0f);
	std::uniform_real_distribution<float> radius_dist(0.25f, 0.75f);
	std::uniform_real_distribution<float> color_dist(0.1f, 1.0f);
	std::uniform_real_distribution<float> spec_dist(0.1f, 0.5f);
	std::uniform_real_distribution<float> ambient_scale_dist(0.03f, 0.12f);
	std::uniform_real_distribution<float> exponent_dist(15.0f, 200.0f);

	for (int i = 0; i < 75; i++)
	{
		float radius = radius_dist(rng);
		Vec3 center(pos_dist(rng), y_dist(rng), pos_dist(rng));

		Vec3 diffuse(color_dist(rng), color_dist(rng), color_dist(rng));
		float spec = spec_dist(rng);
		Vec3 specular(spec, spec, spec);
		float ambient_scale = ambient_scale_dist(rng);
		Vec3 ambient = ambient_scale * diffuse;
		float exponent = exponent_dist(rng);

		scene.push_back(std::make_unique<Sphere>(center, radius, Material(diffuse, specular, ambient, exponent)));
	}

	std::size_t r = 0;
	for (std::size_t i = 0; i < static_cast<std::size_t>(width); i++)
	{
		for (std::size_t j = 0; j < static_cast<std::size_t>(height); j++)
		{
			Ray ray = rays[r];
			std::optional<Hit> closest_hit;
			for (const auto &shape : scene)
			{
				auto hit = shape->intersects(ray);
				if (hit && (!closest_hit || hit->distance < closest_hit->distance))
				{
					closest_hit = *hit;
				}
			}
			if (closest_hit)
			{
				Vec3 color = closest_hit->shape->getColor(ray, *closest_hit, lights, scene);
				image.setPixel(i, j, color.x, color.y, color.z);
			}
			r += 1;
		}
	}

	image.writeToFile("test.png");

	// if(argc < 2) {
	// 	std::cout << "Usage: A1 meshfile" << std::endl;
	// 	return 0;
	// }
	// std::string meshName(argv[1]);

	// // Load geometry
	// std::vector<float> posBuf; // list of vertex positions
	// std::vector<float> norBuf; // list of vertex normals
	// std::vector<float> texBuf; // list of vertex texture coords
	// tinyobj::attrib_t attrib;
	// std::vector<tinyobj::shape_t> shapes;
	// std::vector<tinyobj::material_t> materials;
	// std::string errStr;
	// bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	// if(!rc) {
	// 	std::cerr << errStr << std::endl;
	// } else {
	// 	// Some OBJ files have different indices for vertex positions, normals,
	// 	// and texture coordinates. For example, a cube corner vertex may have
	// 	// three different normals. Here, we are going to duplicate all such
	// 	// vertices.
	// 	// Loop over shapes
	// 	for(size_t s = 0; s < shapes.size(); s++) {
	// 		// Loop over faces (polygons)
	// 		size_t index_offset = 0;
	// 		for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
	// 			size_t fv = shapes[s].mesh.num_face_vertices[f];
	// 			// Loop over vertices in the face.
	// 			for(size_t v = 0; v < fv; v++) {
	// 				// access to vertex
	// 				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
	// 				posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
	// 				posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
	// 				posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
	// 				if(!attrib.normals.empty()) {
	// 					norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
	// 					norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
	// 					norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
	// 				}
	// 				if(!attrib.texcoords.empty()) {
	// 					texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
	// 					texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
	// 				}
	// 			}
	// 			index_offset += fv;
	// 			// per-face material (IGNORE)
	// 			shapes[s].mesh.material_ids[f];
	// 		}
	// 	}
	// }
	// std::cout << "Number of vertices: " << posBuf.size()/3 << std::endl;

	return 0;
}
