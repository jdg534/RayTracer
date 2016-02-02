#pragma once

// this file is to handle all rendering code, until can get working renderer class

#include "Sphere.h"

#include <string> // might not be available on PS4


//[comment]
// This variable controls the maximum recursion depth
//[/comment]
#define MAX_RAY_DEPTH 5

#define NUM_FRAMES_TO_RENDER 100 // not needed here

namespace rendering
{
	unsigned int g_targetFps = 30; // can be overriden


	float mix(const float &a, const float &b, const float &mix)
	{
		return b * mix + a * (1 - mix);
	}

	//[comment]
	// This is the main trace function. It takes a ray as argument (defined by its origin
	// and direction). We test if this ray intersects any of the geometry in the scene.
	// If the ray intersects an object, we compute the intersection point, the normal
	// at the intersection point, and shade this point using this information.
	// Shading depends on the surface property (is it transparent, reflective, diffuse).
	// The function returns a color for the ray. If the ray intersects an object that
	// is the color of the object at the intersection point, otherwise it returns
	// the background color.
	//[/comment]
	Vec3f trace(
		const Vec3f &rayorig,
		const Vec3f &raydir,
		const std::vector<Sphere> &spheres,
		const int &depth)
	{
		//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
		float tnear = INFINITY;
		const Sphere* sphere = NULL;
		// find intersection of this ray with the sphere in the scene
		for (unsigned i = 0; i < spheres.size(); ++i) {
			float t0 = INFINITY, t1 = INFINITY;
			if (spheres[i].intersect(rayorig, raydir, t0, t1)) {
				if (t0 < 0) t0 = t1;
				if (t0 < tnear) {
					tnear = t0;
					sphere = &spheres[i];
				}
			}
		}
		// if there's no intersection return black or background color
		if (!sphere) return Vec3f(2);
		Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
		Vec3f phit = rayorig + raydir * tnear; // point of intersection
		Vec3f nhit = phit - sphere->center; // normal at the intersection point
		nhit.normalize(); // normalize normal direction
						  // If the normal and the view direction are not opposite to each other
						  // reverse the normal direction. That also means we are inside the sphere so set
						  // the inside bool to true. Finally reverse the sign of IdotN which we want
						  // positive.
		float bias = 1e-4; // add some bias to the point from which we will be tracing
		bool inside = false;
		if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true;
		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
			float facingratio = -raydir.dot(nhit);
			// change the mix value to tweak the effect
			float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
			// compute reflection direction (not need to normalize because all vectors
			// are already normalized)
			Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
			refldir.normalize();
			Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1);
			Vec3f refraction = 0;
			// if the sphere is also transparent compute refraction ray (transmission)
			if (sphere->transparency) {
				float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
				float cosi = -nhit.dot(raydir);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
				refrdir.normalize();
				refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
			}
			// the result is a mix of reflection and refraction (if the sphere is transparent)
			surfaceColor = (
				reflection * fresneleffect +
				refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
		}
		else {
			// it's a diffuse object, no need to raytrace any further
			for (unsigned i = 0; i < spheres.size(); ++i) {
				if (spheres[i].emissionColor.x > 0) {
					// this is a light
					Vec3f transmission = 1;
					Vec3f lightDirection = spheres[i].center - phit;
					lightDirection.normalize();
					for (unsigned j = 0; j < spheres.size(); ++j) {
						if (i != j) {
							float t0, t1;
							if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) {
								transmission = 0;
								break;
							}
						}
					}
					surfaceColor += sphere->surfaceColor * transmission *
						std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}

		return surfaceColor + sphere->emissionColor;
	}

	std::string FrameIndexStr(int index)
	{
		// just dealing with three Decimal places, for now
		std::stringstream ss;
		if (index < 10)
		{
			ss << "00";
		}
		else if (index < 100)
		{
			ss << "0";
		}
		ss << index;
		return ss.str();
	}

	//[comment]
	// Main rendering function. We compute a camera ray for each pixel of the image
	// trace it and return a color. If the ray hits a sphere, we return the color of the
	// sphere at the intersection point, else we return the background color.
	//[/comment]
	void render(const std::vector<Sphere> &spheres, int iteration)
	{
		// quick and dirty
		unsigned width = 640, height = 480;
		// Recommended Testing Resolution
		//unsigned width = 640, height = 480;

		// Recommended Production Resolution
		//unsigned width = 1920, height = 1080;
		Vec3f *image = new Vec3f[width * height], *pixel = image;
		float invWidth = 1 / float(width), invHeight = 1 / float(height);
		float fov = 30, aspectratio = width / float(height);
		float angle = tan(M_PI * 0.5 * fov / 180.);
		// Trace rays
		for (unsigned y = 0; y < height; ++y) {
			for (unsigned x = 0; x < width; ++x, ++pixel) {
				float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
				float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
				Vec3f raydir(xx, yy, -1);
				raydir.normalize();
				*pixel = trace(Vec3f(0), raydir, spheres, 0);
			}
		}
		// Save result to a PPM image (keep these flags if you compile under Windows)
		std::stringstream ss;
		ss << "./spheres" << FrameIndexStr(iteration) << ".ppm";
		std::string tempString = ss.str();
		char* filename = (char*)tempString.c_str();

		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << width << " " << height << "\n255\n";
		for (unsigned i = 0; i < width * height; ++i) {
			ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
				(unsigned char)(std::min(float(1), image[i].y) * 255) <<
				(unsigned char)(std::min(float(1), image[i].z) * 255);
		}
		ofs.close();
		delete[] image;
	}

	void renderToFolderAndFileName(std::string folderStr, std::string fileStr)
	{
		// code this!!!
	}
};
