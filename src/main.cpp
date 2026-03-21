#include <iostream>
#include <string>
#include <memory>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <limits>
#include <iomanip>

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

struct FloatRange
{
	float min = 0.0f;
	float max = 0.0f;
};

struct Options
{
	Vec3 cameraPosition = Vec3(-12.0f, 10.0f, 12.0f);
	float cameraFov = 45.0f;
	std::string outputFile = "test.png";

	FloatRange lightPosX = {-12.0f, 12.0f};
	FloatRange lightPosY = {10.0f, 14.0f};
	FloatRange lightPosZ = {-12.0f, 12.0f};
	FloatRange lightIntensity = {0.1f, 0.8f};
	int numLights = 3;

	FloatRange planeColor = {0.1f, 1.0f};

	FloatRange spherePosXZ = {-10.0f, 10.0f};
	FloatRange sphereY = {0.25f, 2.0f};
	FloatRange sphereRadius = {0.25f, 0.75f};
	FloatRange sphereColor = {0.1f, 1.0f};
	FloatRange sphereSpec = {0.1f, 0.5f};
	FloatRange sphereAmbientScale = {0.03f, 0.12f};
	FloatRange sphereExponent = {15.0f, 200.0f};
	int numSpheres = 75;
};

static float parseFloatOrThrow(const char *value, const char *what)
{
	try
	{
		return std::stof(std::string(value));
	}
	catch (...)
	{
		throw std::runtime_error(std::string("Invalid ") + what + ": '" + value + "'");
	}
}

static int parseIntOrThrow(const char *value, const char *what)
{
	try
	{
		return std::stoi(std::string(value));
	}
	catch (...)
	{
		throw std::runtime_error(std::string("Invalid ") + what + ": '" + value + "'");
	}
}

static FloatRange parseRangeOrThrow(int argc, char **argv, int &i, const char *what)
{
	if (i + 2 >= argc)
	{
		throw std::runtime_error(std::string("Missing values for ") + what + " (expected: <min> <max>)");
	}
	FloatRange r;
	r.min = parseFloatOrThrow(argv[i + 1], "range min");
	r.max = parseFloatOrThrow(argv[i + 2], "range max");
	i += 2;
	return r;
}

static Vec3 parseVec3OrThrow(int argc, char **argv, int &i, const char *what)
{
	if (i + 3 >= argc)
	{
		throw std::runtime_error(std::string("Missing values for ") + what + " (expected: <x> <y> <z>)");
	}
	float x = parseFloatOrThrow(argv[i + 1], "x");
	float y = parseFloatOrThrow(argv[i + 2], "y");
	float z = parseFloatOrThrow(argv[i + 3], "z");
	i += 3;
	return Vec3(x, y, z);
}

static void validateRangeOrThrow(const FloatRange &r, const char *what)
{
	if (r.min > r.max)
	{
		throw std::runtime_error(std::string("Invalid ") + what + " range: min > max");
	}
}

static void printUsage(const char *exe)
{
	std::cout
		<< "Usage: " << exe << " [options]\n"
		<< "\n"
		<< "Output:\n"
		<< "  --out <filename>                 Output image filename (default: test.png)\n"
		<< "\n"
		<< "Camera:\n"
		<< "  --cam-pos <x> <y> <z>            Camera position (default: -12 10 12)\n"
		<< "  --cam-fov <degrees>              Camera vertical FOV in degrees (default: 45)\n"
		<< "\n"
		<< "Lights (randomized):\n"
		<< "  --num-lights <n>                 Number of point lights (default: 3)\n"
		<< "  --light-pos-x <min> <max>        Light X position range (default: -12 12)\n"
		<< "  --light-pos-y <min> <max>        Light Y position range (default: 10 14)\n"
		<< "  --light-pos-z <min> <max>        Light Z position range (default: -12 12)\n"
		<< "  --light-intensity <min> <max>    Light intensity range (default: 0.1 0.8)\n"
		<< "\n"
		<< "Plane (randomized):\n"
		<< "  --plane-color <min> <max>        Plane diffuse RGB component range (default: 0.1 1.0)\n"
		<< "\n"
		<< "Spheres (randomized):\n"
		<< "  --num-spheres <n>                Number of spheres (default: 75)\n"
		<< "  --sphere-pos-xz <min> <max>      Sphere X/Z position range (default: -10 10)\n"
		<< "  --sphere-y <min> <max>           Sphere Y position range (default: 0.25 2.0)\n"
		<< "  --sphere-radius <min> <max>      Sphere radius range (default: 0.25 0.75)\n"
		<< "  --sphere-color <min> <max>       Sphere diffuse RGB component range (default: 0.1 1.0)\n"
		<< "  --sphere-spec <min> <max>        Sphere specular RGB component range (default: 0.1 0.5)\n"
		<< "  --sphere-ambient-scale <min> <max> Ambient = scale * diffuse (default: 0.03 0.12)\n"
		<< "  --sphere-exponent <min> <max>    Phong exponent range (default: 15 200)\n"
		<< "\n"
		<< "Other:\n"
		<< "  -h, --help                       Show this help text\n";
}

struct ParseOutcome
{
	Options options;
	bool showHelp = false;
};

static ParseOutcome parseArgsOrThrow(int argc, char **argv)
{
	ParseOutcome outcome;

	using Handler = std::function<void(ParseOutcome &, int, char **, int &)>;
	std::unordered_map<std::string, Handler> handlers;

	handlers.emplace("--out", [](ParseOutcome &o, int argc, char **argv, int &i) {
		if (i + 1 >= argc)
		{
			throw std::runtime_error("Missing value for --out (expected: <filename>)");
		}
		o.options.outputFile = std::string(argv[i + 1]);
		i += 1;
	});

	handlers.emplace("--cam-pos", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.cameraPosition = parseVec3OrThrow(argc, argv, i, "--cam-pos");
	});
	handlers.emplace("--cam-fov", [](ParseOutcome &o, int argc, char **argv, int &i) {
		if (i + 1 >= argc)
		{
			throw std::runtime_error("Missing value for --cam-fov (expected: <degrees>)");
		}
		o.options.cameraFov = parseFloatOrThrow(argv[i + 1], "camera fov");
		i += 1;
	});

	handlers.emplace("--light-pos-x", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.lightPosX = parseRangeOrThrow(argc, argv, i, "--light-pos-x");
	});
	handlers.emplace("--light-pos-y", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.lightPosY = parseRangeOrThrow(argc, argv, i, "--light-pos-y");
	});
	handlers.emplace("--light-pos-z", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.lightPosZ = parseRangeOrThrow(argc, argv, i, "--light-pos-z");
	});
	handlers.emplace("--light-intensity", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.lightIntensity = parseRangeOrThrow(argc, argv, i, "--light-intensity");
	});
	handlers.emplace("--num-lights", [](ParseOutcome &o, int argc, char **argv, int &i) {
		if (i + 1 >= argc)
		{
			throw std::runtime_error("Missing value for --num-lights (expected: <n>)");
		}
		o.options.numLights = parseIntOrThrow(argv[i + 1], "number of lights");
		i += 1;
	});

	handlers.emplace("--plane-color", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.planeColor = parseRangeOrThrow(argc, argv, i, "--plane-color");
	});

	handlers.emplace("--sphere-pos-xz", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.spherePosXZ = parseRangeOrThrow(argc, argv, i, "--sphere-pos-xz");
	});
	handlers.emplace("--sphere-y", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereY = parseRangeOrThrow(argc, argv, i, "--sphere-y");
	});
	handlers.emplace("--sphere-radius", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereRadius = parseRangeOrThrow(argc, argv, i, "--sphere-radius");
	});
	handlers.emplace("--sphere-color", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereColor = parseRangeOrThrow(argc, argv, i, "--sphere-color");
	});
	handlers.emplace("--sphere-spec", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereSpec = parseRangeOrThrow(argc, argv, i, "--sphere-spec");
	});
	handlers.emplace("--sphere-ambient-scale", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereAmbientScale = parseRangeOrThrow(argc, argv, i, "--sphere-ambient-scale");
	});
	handlers.emplace("--sphere-exponent", [](ParseOutcome &o, int argc, char **argv, int &i) {
		o.options.sphereExponent = parseRangeOrThrow(argc, argv, i, "--sphere-exponent");
	});
	handlers.emplace("--num-spheres", [](ParseOutcome &o, int argc, char **argv, int &i) {
		if (i + 1 >= argc)
		{
			throw std::runtime_error("Missing value for --num-spheres (expected: <n>)");
		}
		o.options.numSpheres = parseIntOrThrow(argv[i + 1], "number of spheres");
		i += 1;
	});

	for (int i = 1; i < argc; i++)
	{
		const std::string arg(argv[i]);
		if (arg == "--help" || arg == "-h")
		{
			outcome.showHelp = true;
			continue;
		}

		auto it = handlers.find(arg);
		if (it == handlers.end())
		{
			throw std::runtime_error("Unknown argument: " + arg);
		}
		it->second(outcome, argc, argv, i);
	}

	if (outcome.options.numLights <= 0)
	{
		throw std::runtime_error("Invalid --num-lights (must be > 0)");
	}
	if (outcome.options.numSpheres < 0)
	{
		throw std::runtime_error("Invalid --num-spheres (must be >= 0)");
	}
	if (outcome.options.outputFile.empty())
	{
		throw std::runtime_error("Invalid --out (filename must not be empty)");
	}

	validateRangeOrThrow(outcome.options.lightPosX, "light-pos-x");
	validateRangeOrThrow(outcome.options.lightPosY, "light-pos-y");
	validateRangeOrThrow(outcome.options.lightPosZ, "light-pos-z");
	validateRangeOrThrow(outcome.options.lightIntensity, "light-intensity");
	validateRangeOrThrow(outcome.options.planeColor, "plane-color");
	validateRangeOrThrow(outcome.options.spherePosXZ, "sphere-pos-xz");
	validateRangeOrThrow(outcome.options.sphereY, "sphere-y");
	validateRangeOrThrow(outcome.options.sphereRadius, "sphere-radius");
	validateRangeOrThrow(outcome.options.sphereColor, "sphere-color");
	validateRangeOrThrow(outcome.options.sphereSpec, "sphere-spec");
	validateRangeOrThrow(outcome.options.sphereAmbientScale, "sphere-ambient-scale");
	validateRangeOrThrow(outcome.options.sphereExponent, "sphere-exponent");

	return outcome;
}

int main(int argc, char **argv)
{
	int width = 1024, height = 1024;
	Options options;

	try
	{
		ParseOutcome parsed = parseArgsOrThrow(argc, argv);
		if (parsed.showHelp)
		{
			printUsage(argv[0]);
			return 0;
		}
		options = parsed.options;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << "\n\n";
		printUsage(argv[0]);
		return 1;
	}

	Camera c(options.cameraPosition, options.cameraFov);
	std::vector<Ray> rays = c.getRays(width, height, Vec3(0.0f, -1.0f, 0.0f));
	Image image(width, height);

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> light_pos_x(options.lightPosX.min, options.lightPosX.max);
	std::uniform_real_distribution<float> light_pos_y(options.lightPosY.min, options.lightPosY.max);
	std::uniform_real_distribution<float> light_pos_z(options.lightPosZ.min, options.lightPosZ.max);
	std::uniform_real_distribution<float> light_intensity(options.lightIntensity.min, options.lightIntensity.max);

	std::vector<Light> lights;
	for (int i = 0; i < options.numLights; i++)
	{
		lights.push_back(Light(
			Vec3(light_pos_x(rng), light_pos_y(rng), light_pos_z(rng)),
			light_intensity(rng)));
	}

	std::uniform_real_distribution<float> plane_color_dist(options.planeColor.min, options.planeColor.max);
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

	std::uniform_real_distribution<float> pos_dist(options.spherePosXZ.min, options.spherePosXZ.max);
	std::uniform_real_distribution<float> y_dist(options.sphereY.min, options.sphereY.max);
	std::uniform_real_distribution<float> radius_dist(options.sphereRadius.min, options.sphereRadius.max);
	std::uniform_real_distribution<float> color_dist(options.sphereColor.min, options.sphereColor.max);
	std::uniform_real_distribution<float> spec_dist(options.sphereSpec.min, options.sphereSpec.max);
	std::uniform_real_distribution<float> ambient_scale_dist(options.sphereAmbientScale.min, options.sphereAmbientScale.max);
	std::uniform_real_distribution<float> exponent_dist(options.sphereExponent.min, options.sphereExponent.max);

	for (int i = 0; i < options.numSpheres; i++)
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

	using Clock = std::chrono::steady_clock;
	using MicrosecondsD = std::chrono::duration<double, std::micro>;

	double min_pixel_us = std::numeric_limits<double>::infinity();
	double max_pixel_us = 0.0;
	double sum_pixel_us = 0.0;
	std::size_t pixel_count = 0;

	const auto render_start = Clock::now();
	std::size_t r = 0;
	for (std::size_t i = 0; i < static_cast<std::size_t>(width); i++)
	{
		for (std::size_t j = 0; j < static_cast<std::size_t>(height); j++)
		{
			const auto pixel_start = Clock::now();
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

			const double pixel_us = MicrosecondsD(Clock::now() - pixel_start).count();
			min_pixel_us = std::min(min_pixel_us, pixel_us);
			max_pixel_us = std::max(max_pixel_us, pixel_us);
			sum_pixel_us += pixel_us;
			pixel_count += 1;

			r += 1;
		}
	}
	const auto render_end = Clock::now();
	const double render_ms = std::chrono::duration<double, std::milli>(render_end - render_start).count();

	image.writeToFile(options.outputFile);

	if (pixel_count > 0)
	{
		const double avg_pixel_us = sum_pixel_us / static_cast<double>(pixel_count);
		std::cerr << std::fixed << std::setprecision(3);
		std::cerr << "Pixel timing (us): min=" << min_pixel_us
				  << " max=" << max_pixel_us
				  << " avg=" << avg_pixel_us
				  << " (n=" << pixel_count << ")\n";
	}
	std::cerr << std::fixed << std::setprecision(3);
	std::cerr << "Total render time (ms): " << render_ms << "\n";

	return 0;
}
