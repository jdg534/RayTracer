#ifndef _FIBERS_ASSIST_H_
#define _FIBERS_ASSIST_H_

#include <sce_fiber.h>
#include <libsysmodule.h>

#include <vector>

#include "Vec3.h"
#include "Sphere.h"
#include "KeyFrame.h"

#define MAX_FIBERS 7 // calls to std::thread::hardware_concurrency (gives the number of threads available on a given platform) gave 7 as a result, so using 7 fibers
#define FIBER_CONTEXT_BUFFER_SIZE (20 * 1024) // was 2 KB, need more for raytrace


struct FiberFrameRange // this struct is passed to a fiber via a memory address
{
	unsigned int rangeStart, rangeEnd;
	LerpedFrame * lerpedFramesArray;
	std::string outputFolder;
	std::string outputFileStart;
};

struct FiberRayTraceRange // this struct is passed to a fiber via a memory address
{
	FiberRayTraceRange()
	{
		imageHeight = imageWidth = rangeStart = rangeEnd = 0;
		imagePtr = nullptr;
		spheres.clear();
	}

	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int rangeStart;
	unsigned int rangeEnd;
	Vec3f * imagePtr;
	std::vector<Sphere> spheres;
};


#endif