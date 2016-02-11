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
			std::cout << "Error " << sceneFile << " not found" << std::endl;
			return;
		}

		// parse the file for the scene information
		// duration in seconds, FPS, ppm file name(just first bit), output video file name, outputFolder
		std::string currentBit = "";

		std::vector<Sphere> spheres;

		std::string outputFolder = "DEFAULT_OUTPUT_FOLDER";
		int fps = 24;
		float videoDuration = 10.0f;
		std::string ppmFileNameStart = "DEFAULT_PPM_";
		std::string statsOutputFileName = "DEFAULT_STATS_OUTPUT.txt";
		std::string outputVideoFile = "DEFAUL_OUT.avi";

		while (currentBit != "END_SCENE_FILE")
		{
			inFile >> currentBit;
			if (currentBit == "outputFolder:")
			{
				inFile >> outputFolder;
			}
			// continue codeing this based staticScene.txt
			else if (currentBit == "framesPerSecond:")
			{
				inFile >> fps;
			}
			else if (currentBit == "sceneDuration:")
			{
				inFile >> videoDuration;
			}
			else if (currentBit == "frameFileNameStart:")
			{
				inFile >> ppmFileNameStart;
			}
			else if (currentBit == "statsOutFile:")
			{
				inFile >> statsOutputFileName;
			}
			else if (currentBit == "outputFile:")
			{
				inFile >> outputVideoFile;
			}
			// next deal with Sphere definitions
			else if (currentBit == "NEW_SPHERE")
			{
				Vec3f postion;
				postion.x = 0.0f;
				postion.y = 0.0f;
				postion.z = 0.0f;
				float radius = 1.0f;
				Vec3f surfaceColour;
				surfaceColour.x = 1.0f;
				surfaceColour.y = 1.0f;
				surfaceColour.z = 1.0f;
				float reflectionValue = 0.0f;
				float transparancy = 0.0f;
				Vec3f emissiveColour;
				emissiveColour.x = 0.0f;
				emissiveColour.y = 0.0f;
				emissiveColour.z = 0.0f;
				std::string sphereID = "DEFAULT_SPHERE_ID";
				// next have while loop to override the above values, see: staticScene.txt for reference
				while (currentBit != "END_SPHERE")
				{
					inFile >> currentBit;
					if (currentBit == "POSITION")
					{
						inFile >> postion.x;
						inFile >> postion.y;
						inFile >> postion.z;
					}
					else if (currentBit == "RADIUS")
					{
						inFile >> radius;
					}
					else if (currentBit == "SURFACE_COLOUR")
					{
						inFile >> surfaceColour.x;
						inFile >> surfaceColour.y;
						inFile >> surfaceColour.z;
					}
					else if (currentBit == "REFLECTION_VALUE")
					{
						inFile >> reflectionValue;
					}
					else if (currentBit == "TRANSPARENCY_VALUE ")
					{
						inFile >> transparancy;
					}
					else if (currentBit == "EMISSIVE_COLOUR")
					{
						inFile >> emissiveColour.x;
						inFile >> emissiveColour.y;
						inFile >> emissiveColour.z;
					}
					else if (currentBit == "ID")
					{
						inFile >> sphereID;
					}
				}
				// now add the sphere
				Sphere tmpS(postion, radius,surfaceColour, reflectionValue, transparancy, emissiveColour);
				tmpS.m_id = sphereID;
				// add the std::vector
				spheres.push_back(tmpS);
			}
		}

		// now that loaded the scene information, it's drawing time
		std::cout << "Loaded Scene from: " << sceneFile << std::endl;
		std::cout << "Output folder: " << outputFolder << std::endl;
		std::cout << "Frames Per Second to use on Video: " << fps << std::endl;
		std::cout << "Start of output file name: " << ppmFileNameStart << std::endl;
		std::cout << "video Duration: " << videoDuration << std::endl;
		std::cout << "Output video file: " << outputVideoFile << std::endl;

		// calculate number of frames to render
		unsigned int nFramesToRender = (unsigned int)(videoDuration * (float)fps);
		using namespace std::chrono;

		steady_clock::time_point timeAtStartOfRender = steady_clock::now();

		for (unsigned int frame = 0; frame < nFramesToRender; frame++)
		{
			steady_clock::time_point timeAtStartOfFrame = steady_clock::now();

			rendering::renderToFolder(ppmFileNameStart, outputFolder, spheres, frame);

			steady_clock::time_point timeAtEndOfFrame = steady_clock::now();

			unsigned framesLeftToRender = nFramesToRender - frame;
			// calculate time taken the draw the frame
			steady_clock::duration timeTakenToRenderFrame = timeAtEndOfFrame - timeAtStartOfFrame;
			milliseconds tttrfMS = duration_cast<milliseconds>(timeTakenToRenderFrame);
			predictNumSecondsToFinshRendering(tttrfMS.count(), framesLeftToRender);
		}
		steady_clock::time_point finaliseRenderStart = steady_clock::now();
		rendering::finshRenderToFolderAndFileName(outputFolder, ppmFileNameStart, outputVideoFile, fps);
		steady_clock::time_point finaliseRenderEnd = steady_clock::now();
		steady_clock::duration finialiseRenderDuration = finaliseRenderEnd - finaliseRenderStart;
		milliseconds frdMS = duration_cast<milliseconds>(finialiseRenderDuration);

		std::cout << "Time to convert frames to video: " << frdMS.count() << "ms" << std::endl;

		steady_clock::time_point timeAtEndOfRender = steady_clock::now();



		steady_clock::duration fullRenderDuration = timeAtEndOfRender - timeAtStartOfRender;
		seconds frdSec = duration_cast<seconds>(fullRenderDuration);
		std::cout << "Time to render the entire scene " << frdSec.count() << " seconds" << std::endl;
		char e;
		std::cin >> e;
	}
	
	



};

// side note compiler optimisation, only works on Relese Builds

#endif