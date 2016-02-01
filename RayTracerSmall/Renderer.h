#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

#include "Sphere.h"

#if defined __linux__ || defined __APPLE__
// "Compiled for Linux
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#include <Windows.h> // Windows specific
#endif

class Renderer
{
public:
	void SimpleShrinkingExample()
	{
		m_outputFile = "simpleShrinkExample.avi";
		m_numFramesToRender = 100;
		m_frameFileName = "SSE_SPHERE_";

		m_frameWidth = 640;
		m_frameHeight = 480;

		m_maxRayDepth = 5;


		std::vector<Sphere> spheres;
		// Vector structure for Sphere (position, radius, surface color, reflectivity, transparency, emission color)

		for (int i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
				spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // The radius paramter is the value we will change
				spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
				spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));

			}
			else if (i == 1)
			{
				spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
				spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 3, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // Radius--
				spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
				spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
			}
			else if (i == 2)
			{
				spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
				spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 2, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // Radius--
				spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
				spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
			}
			else if (i == 3)
			{
				spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
				spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 1, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // Radius--
				spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
				spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
			}

			render(spheres, i);
			// Dont forget to clear the Vector holding the spheres.
			spheres.clear();

			finshRendering();
		}
	}

	void smoothScalingExample()
	{
		m_outputFile = "smoothScalingExample.avi";
		m_numFramesToRender = 100;
		m_frameFileName = "SSE_SPHERE_";

		m_frameWidth = 640;
		m_frameHeight = 480;

		m_maxRayDepth = 5;

		std::vector<Sphere> s;
		for (float r = 0; r < m_numFramesToRender; r++)
		{
			s.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
			s.push_back(Sphere(Vec3f(0.0, 0, -20), r / 100, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // Radius++ change here
			s.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
			s.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
			
			
			
			render(s, r);
			std::cout << "Rendered and saved " << m_frameFileName << FrameIndexStr(r) << ".ppm" << std::endl;
			// Dont forget to clear the Vector holding the spheres.
			s.clear();
		}
		finshRendering();
	}

	void BasicRenderExample()
	{
		m_outputFile = "BasicRenderExample.avi";
		m_numFramesToRender = 1;
		m_frameFileName = "SSE_SPHERE_";

		m_frameWidth = 640;
		m_frameHeight = 480;

		m_maxRayDepth = 5;

		std::vector<Sphere> spheres;
		// Vector structure for Sphere (position, radius, surface color, reflectivity, transparency, emission color)

		spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
		spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // The radius paramter is the value we will change
		spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
		spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));

		// This creates a file, titled 1.ppm in the current working directory
		render(spheres, 1);

		finshRendering();
	}

	void render(std::vector<Sphere> & s, int frameNumber)
	{
		Vec3f * img = new Vec3f[m_frameWidth * m_frameHeight], *px = img;

		float invWidth = 1.0f / (float)m_frameWidth, invHeight = 1.0f / (float)m_frameHeight;

		float fieldOfView = 30.0f, aspectRatio = ((float)m_frameWidth / (float)m_frameHeight);
		float angle = tanf(M_PI * 0.5f * fieldOfView / 180.0f);

		for (unsigned int y = 0; y < m_frameHeight; y++)
		{
			for (unsigned int x = 0; x < m_frameWidth; x++)
			{
				float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
				float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
				Vec3f raydir(xx, yy, -1);
				raydir.normalize();
				*px = traceRay(Vec3f(0), raydir, s, 0); // set the pixel, that px points to be the colour determined via tracing the ray
			}
		}

		// write img to file
		std::stringstream ss;

		ss << "./" << m_frameFileName << FrameIndexStr(frameNumber) << ".ppm";
		std::string tempString = ss.str();
		char* filename = (char*)tempString.c_str();

		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << m_frameWidth << " " << m_frameHeight << "\n255\n";
		for (unsigned i = 0; i < m_frameWidth * m_frameHeight; ++i)
		{
			ofs << (unsigned char)(minFunc(float(1), img[i].x) * 255) <<
				(unsigned char)(minFunc(float(1), img[i].y) * 255) <<
				(unsigned char)(minFunc(float(1), img[i].z) * 255);
		}
		ofs.close();

		delete[] img;
	}

	Vec3f traceRay(const Vec3f &rayorig,
		const Vec3f &raydir,
		const std::vector<Sphere> &spheres,
		const int &depth)
	{
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
		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < m_maxRayDepth) {
			float facingratio = -raydir.dot(nhit);
			// change the mix value to tweak the effect
			float fresneleffect = lerp(pow(1 - facingratio, 3), 1, 0.1);
			// compute reflection direction (not need to normalize because all vectors
			// are already normalized)
			Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
			refldir.normalize();
			Vec3f reflection = traceRay(phit + nhit * bias, refldir, spheres, depth + 1);
			Vec3f refraction = 0;
			// if the sphere is also transparent compute refraction ray (transmission)
			if (sphere->transparency) {
				float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
				float cosi = -nhit.dot(raydir);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
				refrdir.normalize();
				refraction = traceRay(phit - nhit * bias, refrdir, spheres, depth + 1);
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
						maxFunc(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
				}
			}
		}

		return surfaceColor + sphere->emissionColor;
	}

	std::string getOutputFile() { return m_outputFile; }
	void setOutputFile(std::string outFileName) { m_outputFile = outFileName; }
	
	unsigned int getNumFramesToRender() { return m_numFramesToRender; }
	void setNumFramesToRender(unsigned int nFrames) { m_numFramesToRender = nFrames; }
	
	std::string getFrameFileName() { return m_frameFileName; }
	void setFrameFileName(std::string frameFilename) { m_frameFileName = frameFilename; }

	unsigned int getFrameWidth() { return m_frameWidth; }
	void setFrameWidth(unsigned int frameWidth) { m_frameWidth = frameWidth; }
	
	unsigned int getFrameHeight() { return m_frameHeight; }
	void setFrameHeight(unsigned int frameHeight) { m_frameHeight = frameHeight; }

	unsigned int getMaxRayDepth() { return m_maxRayDepth; }
	void setMaxRayDepth(unsigned int maxRayDepth) { m_maxRayDepth = maxRayDepth; }

	void finshRendering()
	{
#if defined __linux__ || defined __APPLE__
		// "Compiled for Linux
#else
		// compiling for windows
		std::string ffmpegCmd = "ffmpeg -y -i ";
		ffmpegCmd += m_frameFileName;
		ffmpegCmd += "%03d.ppm ";
		ffmpegCmd += m_outputFile;

		system(ffmpegCmd.c_str());

		// system("del \"*.ppm\""); // delete the old frames
		// system(m_outputFile.c_str()); // play the video

#endif
	}

	static float minFunc(float a, float b)
	{
		if (a < b)
		{
			return a;
		}
		return b;
	}
	static float maxFunc(float a, float b)
	{
		if (a > b)
		{
			return a;
		}
		return b;
	}

	static float lerp(float a, float b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

private:
	std::string m_outputFile;
	unsigned int m_numFramesToRender; // no higher than 999
	std::string m_frameFileName;

	unsigned int m_frameWidth;
	unsigned int m_frameHeight;

	unsigned int m_maxRayDepth;

	static std::string FrameIndexStr(int index)
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

	
};
