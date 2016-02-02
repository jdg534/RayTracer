#ifndef _TEST_SCENES_H_
#define _TEST_SCENES_H_

#include "Rendering.h"

//#ifdef _DEBUG
#include <iostream>
#include <chrono>
//#endif

#include <fstream>

namespace TestScenes
{
	void predictNumSecondsToFinshRendering(unsigned int msToRenderCurrentFrame, unsigned int numFramesLeftToRender)
	{
		unsigned long long estMsToGo = msToRenderCurrentFrame * numFramesLeftToRender;
		unsigned long inSeconds = estMsToGo / 1000;
		unsigned int inMinutes = inSeconds / 60;
		unsigned short inHours = inMinutes / 60; // this can happen

		unsigned short minRemainder = inMinutes - (inHours * 60);
		unsigned short secRemainder = inSeconds;
		// subtract in hours value
		secRemainder -= (inHours * 3600);
		// then minute remainder
		secRemainder -= (minRemainder * 60);

		std::cout << "Time to render the rest of the scene: ";

		if (inHours > 0)
		{
			std::cout << inHours << "hr(s) " << minRemainder << "min(s) " << secRemainder << "sec(s)";
		}
		else 
		{
			if (inMinutes > 0)
			{
				std::cout << inMinutes << "min(s) " << secRemainder << "sec(s)";
			}
			else
			{
				if (inSeconds > 0)
				{
					std::cout << inSeconds << " seconds";
				}
				else
				{
					std::cout << estMsToGo << "ms";
				}
			}
		}

		std::cout << std::endl;
	}

	void renderSceneForTime(float secondsToRenderFor, int framesPerSecond)
	{
		using namespace std::chrono;

		float framesToRender = secondsToRenderFor * (float)framesPerSecond;

		int ftrIntForm = (int)framesToRender;

		// define the Spheres (taken from BasicRender())
		std::vector<Sphere> spheres;
		// Vector structure for Sphere (position, radius, surface color, reflectivity, transparency, emission color)

		spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
		spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // The radius paramter is the value we will change
		spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
		spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));


		for (int i = 0; i < ftrIntForm; i++)
		{
			
			std::cout << "Rendering frame " << i << " of " << ftrIntForm << std::endl;
			system_clock::time_point renderStart = system_clock::now();
			rendering::render(spheres, i);
			system_clock::time_point renderEnd = system_clock::now();

			milliseconds msToRender = duration_cast<milliseconds>(renderEnd - renderStart);
			std::cout << "Rendering to file took: " << msToRender.count() << "ms" << std::endl;
		}
		rendering::finshRender(framesPerSecond);
	}

	void renderSceneForTimeSpecificOutput(float secondsToRenderFor, int framesPerSecond, 
		std::string ppmFile, std::string outputVideoFileName,
		std::string fileDirectory)
	{
		using namespace std::chrono;

		float framesToRender = secondsToRenderFor * (float)framesPerSecond;

		int ftrIntForm = (int)framesToRender;

		// define the Spheres (taken from BasicRender())
		std::vector<Sphere> spheres;
		// Vector structure for Sphere (position, radius, surface color, reflectivity, transparency, emission color)

		spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
		spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); // The radius paramter is the value we will change
		spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
		spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));


		for (int i = 0; i < ftrIntForm; i++)
		{

			std::cout << "Rendering frame " << i << " of " << ftrIntForm << std::endl;
			system_clock::time_point renderStart = system_clock::now();
			// rendering::render(spheres, i);
			rendering::renderToFolder(ppmFile, fileDirectory, spheres, i);

			system_clock::time_point renderEnd = system_clock::now();

			milliseconds msToRender = duration_cast<milliseconds>(renderEnd - renderStart);
			std::cout << "Rendering to file took: " << msToRender.count() << "ms" << std::endl;
			predictNumSecondsToFinshRendering(msToRender.count(), ftrIntForm - i);
		}
		// rendering::finshRender(framesPerSecond);
		rendering::finshRenderToFolderAndFileName(fileDirectory, ppmFile, outputVideoFileName, framesPerSecond);
	}

	void renderStaticSceneFileFromFile(std::string sceneFile)
	{
		std::ifstream inFile(sceneFile);
		if (!inFile.good())
		{
			return;
		}


	}
};

#endif