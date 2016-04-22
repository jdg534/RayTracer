#ifndef _RENDERING_H_
#define _RENDERING_H_

#include <vector>
#include <chrono>

#include "Vec3.h"
#include "Sphere.h"
#include "MathTypes.h"


#define MAX_RAY_DEPTH 5

// #define NUM_FRAMES_TO_RENDER 100 // not needed here

#ifdef _DEBUG
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#else
/*
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
*/
#define FRAME_WIDTH 640 // for profileing
#define FRAME_HEIGHT 480

// 1920 x 1080 = "Full HD"
#endif

#define OS_FOLDER_SEPERATOR "/" // may need to have different def based on the platform

namespace rendering
{
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

	void renderToFolderMultiThreadProfileable(std::string fileName, std::string folder, const std::vector<Sphere> & spheres, int frameNumber, std::chrono::steady_clock::time_point * startTimePoint, std::chrono::steady_clock::time_point * endTimePoint);

	void finshRenderToFolderWithCleanUp(std::string folderStr, std::string framefileNameStartStr, std::string outputFileName, unsigned int outputFps = 30, bool deletePPMFiles = true);

};

#endif