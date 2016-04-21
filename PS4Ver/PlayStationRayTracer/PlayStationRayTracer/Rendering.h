#pragma once

// this file is to handle all rendering code, until can get working renderer class

#include "Sphere.h"
#include "MathTypes.h"

#include <string>

#include <thread> // might not be available on PS4, is there :)
#include <vector>




namespace rendering
{
	// unsigned int g_targetFps = 30; // can be overriden, this is only for when converting the still frames using FFMPEG
	

	

	float mix(const float &a, const float &b, const float &mix);

	Vec3f trace(
		const Vec3f &rayorig,
		const Vec3f &raydir,
		const std::vector<Sphere> &spheres,
		const int &depth);

	void renderToFolderProfileable(std::string fileName, std::string folder, const std::vector<Sphere> & spheres, int frameNumber, std::chrono::steady_clock::time_point * startTimePoint, std::chrono::steady_clock::time_point * endTimePoint);

	Vector2D getPixelCoord(unsigned int imageWidth, unsigned int imageHeight, unsigned index);

	
	unsigned int getPixelIndex(unsigned int imageWidth, unsigned int imageHeight, Vector2D pixelCoord);


	void traceRaysInRange(Vec3f * img, unsigned int imgWidth, unsigned int imgHeight, const std::vector<Sphere> &spheres, unsigned int rangeStart, unsigned int rangeEnd);


	__attribute__((noreturn))
	void fiberTraceRaysEntryPoint(uint64_t initArgs, uint64_t runArgs);

	void renderToFolderMultiThreadProfileable(std::string fileName, std::string folder, const std::vector<Sphere> & spheres, int frameNumber, std::chrono::steady_clock::time_point * startTimePoint, std::chrono::steady_clock::time_point * endTimePoint);


	void finshRenderToFolderWithCleanUp(std::string folderStr, std::string framefileNameStartStr, std::string outputFileName, int nFrames, unsigned int outputFps = 30, bool deletePPMFiles = true);

};
