#include "Rendering.h"

#include "allocator.h"

#include "MathTypes.h"
#include "PS4FolderPath.h"
#include "FibersAssist.h"
#include "Fios2Wrapper.h"

#include <sstream>
#include <fstream>
#include <cassert>
#include <iostream>

#define MAX_RAY_DEPTH 5



#ifdef _DEBUG
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#else

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080

// 1920 x 1080 = "Full HD"
#endif

#define OS_FOLDER_SEPERATOR "/" // may need to have different def based on the platform



static const size_t onionMemSz = 64 * 1024 * 1024; // 64 MB, chunks


float rendering::mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
}

Vec3f rendering::trace(
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

void rendering::renderToFolderProfileable(std::string fileName, std::string folder, const std::vector<Sphere> & spheres, int frameNumber, std::chrono::steady_clock::time_point * startTimePoint, std::chrono::steady_clock::time_point * endTimePoint)
{
	*startTimePoint = std::chrono::steady_clock::now();
	// quick and dirty
	unsigned width = FRAME_WIDTH, height = FRAME_HEIGHT;
	// Recommended Testing Resolution
	//unsigned width = 640, height = 480;

	// Recommended Production Resolution
	//unsigned width = 1920, height = 1080;

	unsigned int memNeededForImage = width * height * sizeof(Vec3f);

	LinearAllocator onionAllocator;

	int onionAllocatorInitVal = onionAllocator.initialize(
		onionMemSz,
		SCE_KERNEL_WB_ONION,
		SCE_KERNEL_PROT_CPU_RW | SCE_KERNEL_PROT_GPU_ALL);

	if (onionAllocatorInitVal != SCE_OK)
	{
		// something went wrong,
		// the initial example didn't do anything as a responce
	}

	// init it first

	void * buffer = onionAllocator.allocate(
		memNeededForImage, sce::Gnm::kAlignmentOfBufferInBytes);

	// Vec3f *image = new Vec3f[width * height], *pixel = image; // the windows def

	Vec3f *image = reinterpret_cast<Vec3f *>(buffer);

	// Vec3f *image = new Vec3f[width * height];

	Vec3f *pixel = image;


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

	*endTimePoint = std::chrono::steady_clock::now();

	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::stringstream ss;
	// ss << "./spheres" << FrameIndexStr(iteration) << ".ppm";

	// ss << folder << OS_FOLDER_SEPERATOR << fileName << frameNumber << ".ppm"; // had to be changed for ORBIS

	ss << PS4_VISUAL_STUDIO_DIR << folder << OS_FOLDER_SEPERATOR << fileName << frameNumber << ".ppm"; // had to be changed for ORBIS

	std::string tempString = ss.str();
	char* filename = (char*)tempString.c_str();

	std::ofstream ofs(filename, std::ios::out | std::ios::binary); // not using FIOS2 for this, need to save time
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(float(1), image[i].x) * 255) <<
			(unsigned char)(std::min(float(1), image[i].y) * 255) <<
			(unsigned char)(std::min(float(1), image[i].z) * 255);
	}
	ofs.close();
	// delete[] image;

	onionAllocator.terminate();
}

Vector2D rendering::getPixelCoord(unsigned int imageWidth, unsigned int imageHeight, unsigned index)
{
	unsigned int rowsDown = index / imageWidth;
	unsigned int pixelsAccross = index - (rowsDown * imageWidth);
	Vector2D rv;
	rv.x = (float)pixelsAccross;
	rv.y = (float)rowsDown;
	return rv;
}


unsigned int rendering::getPixelIndex(unsigned int imageWidth, unsigned int imageHeight, Vector2D pixelCoord)
{
	unsigned int rv = 0;
	rv += imageWidth * pixelCoord.y;
	rv += floorf(pixelCoord.x);
	return rv;
}


void rendering::traceRaysInRange(Vec3f * img, unsigned int imgWidth, unsigned int imgHeight, const std::vector<Sphere> &spheres, unsigned int rangeStart, unsigned int rangeEnd)
{
	// Vec3f *startPtr = img + rangeStart;
	float invWidth = 1 / float(imgWidth), invHeight = 1 / float(imgHeight);
	float fov = 30, aspectratio = imgWidth / float(imgHeight);
	float angle = tan(M_PI * 0.5 * fov / 180.);


	// there's no guarentee that won't just have bits of lines
	for (int i = rangeStart; i < rangeEnd; i++)
	{
		Vector2D pxCoord = getPixelCoord(imgWidth, imgHeight, i);

		float xx = (2 * ((pxCoord.x + 0.5) * invWidth) - 1) * angle * aspectratio;
		float yy = (1 - 2 * ((pxCoord.y + 0.5) * invHeight)) * angle;
		Vec3f raydir(xx, yy, -1);
		raydir.normalize();

		Vec3f * pxInImage = img + i;
		*pxInImage = trace(Vec3f(0), raydir, spheres, 0);
	}
}


__attribute__((noreturn))
void rendering::fiberTraceRaysEntryPoint(uint64_t initArgs, uint64_t runArgs)
{
	FiberRayTraceRange * traceArea = (FiberRayTraceRange *)initArgs;
	// call traceRaysInRange(image, width, height, spheres, allocatedRays, nRaysToTrace);
	traceRaysInRange(traceArea->imagePtr, traceArea->imageWidth, traceArea->imageHeight, traceArea->spheres, traceArea->rangeStart, traceArea->rangeEnd);
	sceFiberReturnToThread(0, &runArgs);
}

void rendering::renderToFolderMultiThreadProfileable(std::string fileName, std::string folder, const std::vector<Sphere> & spheres, int frameNumber, std::chrono::steady_clock::time_point * startTimePoint, std::chrono::steady_clock::time_point * endTimePoint)
{
	*startTimePoint = std::chrono::steady_clock::now();
	// quick and dirty
	unsigned width = FRAME_WIDTH, height = FRAME_HEIGHT;
	// Recommended Testing Resolution
	//unsigned width = 640, height = 480;

	// Recommended Production Resolution
	//unsigned width = 1920, height = 1080;

	// here!!! use libFiber for tracing the rays


	// Vec3f *image = new Vec3f[width * height],

	unsigned int memNeededForImage = width * height * sizeof(Vec3f);

	LinearAllocator onionAllocator;

	int onionAllocatorInitVal = onionAllocator.initialize(
		onionMemSz,
		SCE_KERNEL_WB_ONION,
		SCE_KERNEL_PROT_CPU_RW | SCE_KERNEL_PROT_GPU_ALL);

	if (onionAllocatorInitVal != SCE_OK)
	{
		// something went wrong,
		// the initial example didn't do anything as a responce
	}

	// init it first

	void * buffer = onionAllocator.allocate(
		memNeededForImage, sce::Gnm::kAlignmentOfBufferInBytes);


	Vec3f *image = reinterpret_cast <Vec3f *>(buffer);

	Vec3f *pixel = image; // replance with the code from the single threaded version


	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30, aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);

	unsigned int nAvailableThreads = std::thread::hardware_concurrency();

	unsigned int nRaysToTrace = width * height;

	unsigned int nRaysPerThread = nRaysToTrace / nAvailableThreads;

	unsigned int nRaysPerFiber = nRaysToTrace / MAX_FIBERS;





	// test run the traceRaysInRange approach
	// traceRaysInRange(image, width, height, spheres, 0, width * height); // would do things this way if only wanted single thread. IT WORKED!!!

	assert(sceSysmoduleIsLoaded(SCE_SYSMODULE_FIBER) == SCE_OK); // pick up here!!!, use fibers



	FiberRayTraceRange raysToAssignToFibers[MAX_FIBERS];
	SceFiber fibers[MAX_FIBERS];
	char fiberContextBuffers[MAX_FIBERS][FIBER_CONTEXT_BUFFER_SIZE];
	// create the fibers then get them to run exercute the functions written for multithreaded functions

	unsigned int toRaysToAllocate = nRaysToTrace;
	unsigned int allocatedRays = 0;

	for (unsigned int i = 0; i < MAX_FIBERS; i++)
	{
		raysToAssignToFibers[i].imageWidth = width;
		raysToAssignToFibers[i].imageHeight = height;
		raysToAssignToFibers[i].rangeStart = allocatedRays;
		raysToAssignToFibers[i].rangeEnd = allocatedRays + nRaysPerFiber;
		raysToAssignToFibers[i].imagePtr = image;
		raysToAssignToFibers[i].spheres = spheres;

		allocatedRays += nRaysPerFiber; // some overlap of 1 pixel on each side of the range that each fiber will work on, (small issue)



		// now create & run the fiber
		int fiberCreateResults = sceFiberInitialize(&fibers[i], "RAY_FIBER", fiberTraceRaysEntryPoint, (uint64_t)&raysToAssignToFibers[i], (void *)fiberContextBuffers[i], FIBER_CONTEXT_BUFFER_SIZE, NULL);
		assert(fiberCreateResults == SCE_OK);
		// run it!!!
		uint64_t fiberReturnArgs = 0;
		fiberCreateResults = sceFiberRun(&fibers[i], 0, &fiberReturnArgs);
		assert(fiberCreateResults == SCE_OK);

	}

	// have the main thread deal with any left overs, if any
	if (allocatedRays < nRaysToTrace)
	{
		traceRaysInRange(image, width, height, spheres, allocatedRays, nRaysToTrace);
	}


	// rejoin & clean up the fibers
	for (int i = 0; i < MAX_FIBERS; i++)
	{
		bool fiberFinshed = false;

		int fiberFinshRes = sceFiberFinalize(&fibers[i]);
		if (fiberFinshRes == SCE_OK)
		{
			fiberFinshed = true;
		}
		else if (fiberFinshRes == SCE_FIBER_ERROR_STATE)
		{
			fiberFinshed = false; // it's still going
		}
		else
		{
			assert(false); // something is wrong!
		}

		if (fiberFinshed)
		{
			// do nothing, (everything is in the stack)

		}
		else
		{
			// the fiber is still working
			i--; // back to check the fiber again
		}
	}

	// traceThreads.clear();

	*endTimePoint = std::chrono::steady_clock::now();

	// Trace rays
	/* (Old Single Threaded code)
	for (unsigned y = 0; y < height; ++y)
	{
	for (unsigned x = 0; x < width; ++x, ++pixel)
	{
	float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
	float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
	Vec3f raydir(xx, yy, -1);
	raydir.normalize();
	*pixel = trace(Vec3f(0), raydir, spheres, 0);
	}
	}
	*/

	// Save result to a PPM image (keep these flags if you compile under Windows)
	std::stringstream ss;
	// ss << "./spheres" << FrameIndexStr(iteration) << ".ppm";
	// ss << folder << OS_FOLDER_SEPERATOR << fileName << frameNumber << ".ppm";
	ss << PS4_VISUAL_STUDIO_DIR << folder << OS_FOLDER_SEPERATOR << fileName << frameNumber << ".ppm";
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
	onionAllocator.terminate(); // frees the memory allocated to the image

	//delete[] image;
}


void rendering::finshRenderToFolderWithCleanUp(std::string folderStr, std::string framefileNameStartStr, std::string outputFileName, int nFrames, unsigned int outputFps, bool deletePPMFiles)
{
	// code this!!!
	std::stringstream ss;
	ss << "ffmpeg -r " << outputFps << " -y ";
	ss << "-i \"" << folderStr << OS_FOLDER_SEPERATOR << framefileNameStartStr << "%d.ppm\"";
	ss << " \"" << folderStr << OS_FOLDER_SEPERATOR << outputFileName << "\"";

	std::string ffmpegCmd = ss.str();

	std::cout << "FFMPEG command:\n" << ffmpegCmd << std::endl;

	// system(ffmpegCmd.c_str());

	// throw "NOT IMPLERMENTED";

	if (deletePPMFiles)
	{
		// will delete the ppm files
		std::string deleteCmdStr = ""; // del <folder path here>\*.ppm
		std::stringstream delSS;
#ifdef _WIN322
		delSS << "del " << folderStr << "\\" << "*.ppm";
		deleteCmdStr = delSS.str();

		std::cout << "Delete command exercuted: " << deleteCmdStr;
		system(deleteCmdStr.c_str());
#else
		// PS4 file delete code here

		fios2Wrapper wrapper;
		wrapper.deleteFrameFileInFolder(nFrames, folderStr, framefileNameStartStr);
#endif

	}

}