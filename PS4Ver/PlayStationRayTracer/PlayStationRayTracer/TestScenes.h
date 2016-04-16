#ifndef _TEST_SCENES_H_
#define _TEST_SCENES_H_

#include "Rendering.h"

#include "RenderManager.h"

#include "KeyFrame.h"
#include "Frame.h"

#include "Math.h" // Josh's Maths library "Josh Math"

//#ifdef _DEBUG
#include <iostream>
#include <chrono>
//#endif

#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include "PS4FolderPath.h"
#include "Fios2Wrapper.h" // for simplerfied usage
#endif


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

	bool folderExists(std::string folderStr)
	{
		bool exists = true;
#ifdef _WIN32
		std::wstring folderWStr = L"";
		folderWStr.assign(folderStr.begin(), folderStr.end());
		DWORD attrib = ::GetFileAttributesA(folderStr.c_str());
		if (attrib == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}
		return (attrib & FILE_ATTRIBUTE_DIRECTORY);
#else 
		// PS4 stuff here
		fios2Wrapper wrapper;
		if (!wrapper.isFios2Initialised())
		{
			wrapper.initFios2();
		}
		
		bool folderExists = wrapper.folderExists(folderStr);


#endif

		return false;
	}

	void createFolder(std::string folderName)
	{
#ifdef _WIN32
		bool folderCreated = CreateDirectoryA(folderName.c_str(), NULL);
		assert(folderCreated);
#else
		// PS4 stuff here
		// ORBIS folder creation code here!!!
		// use FIOS2
		fios2Wrapper wrapper;
		if (!wrapper.isFios2Initialised())
		{
			wrapper.initFios2();
		}
		bool createResults = wrapper.createFolder(folderName);
#endif
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
	}
	
	void fillKeyFrameWithMostRecentSphereStates(KeyFrame & kf, std::vector<Sphere> spheres)
	{
		for (int i = 0; i < spheres.size(); i++)
		{
			kf.spheres.push_back(spheres[i]);
		}
	}
	
	void assignKeyFramesToLerpBetween(float timePoint, KeyFrame & kf1, KeyFrame & kf2, std::vector<KeyFrame> & keyFrames)
	{
		// keyFrames will have been sorted in order of time points, by the time the func is called

		int kf1Index = -1, kf2Index = -1; // treat negative values as unassigned

		for (int i = 0; i < keyFrames.size(); i++)
		{
			if (keyFrames[i].timePoint <= timePoint)
			{
				kf1Index = i;
				if (kf1Index < keyFrames.size() - 1)
				{
					kf2Index = i + 1;
				}
				else
				{
					kf2Index = i;
				}
			}
		}

		kf1 = keyFrames[kf1Index];
		kf2 = keyFrames[kf2Index];
	}

	float weightValueForTimePoints(float min, float max, float point)
	{
		// difference between min & max
		float diffMinMax = max - min;
		if (diffMinMax == 0.0f)
		{
			return 1.0f; 
			// prevents division by 0.
			// the value would be after would be 1.0 anyway
		}

		// difference between min & point
		float diffMinPoint = point - min;
		// return diffMinPoint/diffMinMax
		return diffMinPoint / diffMinMax;
	}

	void renderKeyFrameSceneFromFile(std::string sceneFile)
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

		std::vector<KeyFrame> keyFrames;

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
				Sphere tmpS(postion, radius, surfaceColour, reflectionValue, transparancy, emissiveColour);
				tmpS.m_id = sphereID;
				// add the std::vector
				spheres.push_back(tmpS);
			}
			else if (currentBit == "START_KEY_FRAME")
			{
				KeyFrame kf;

				if (keyFrames.size() == 0)
				{
					// it's the first key frame encountered

					// add a default key frame to the start, with default values
					KeyFrame firstKF;
					firstKF.timePoint = 0.0f;
					fillKeyFrameWithMostRecentSphereStates(firstKF, spheres);
					keyFrames.push_back(firstKF);
				}

				// fill kf with the values from the last key frame
				kf.timePoint = 0.0f;
				fillKeyFrameWithMostRecentSphereStates(kf, keyFrames[keyFrames.size() - 1].spheres);

				while (currentBit != "END_KEY_FRAME")
				{
					inFile >> currentBit;



					if (currentBit == "TIME_POINT")
					{
						inFile >> kf.timePoint;
					}
					else if (currentBit == "START_SPHERE_STATE")
					{
						Sphere * s = nullptr;
						while (currentBit != "END_SPHERE_STATE")
						{
							inFile >> currentBit;
							if (currentBit == "ID")
							{
								// the ID must always be set first in file
								
								// make s point to the sphere, with the matching ID

								std::string sphereID = "";
								inFile >> sphereID;

								for (int i = 0; i < kf.spheres.size(); i++)
								{
									if (kf.spheres[i].m_id == sphereID)
									{
										s = &kf.spheres[i];
									}
								}

								if (s == nullptr)
								{
									// couldn't find the correct sphere, send warning & return;
									std::cout << "Error: " << sphereID << " wasn't found" <<std::endl;
									return;
								}
							}
							else if (currentBit == "POSITION")
							{
								inFile >> s->center.x;
								inFile >> s->center.y;
								inFile >> s->center.z;
							}
							else if (currentBit == "RADIUS")
							{
								inFile >> s->radius;
							}
							else if (currentBit == "SURFACE_COLOUR")
							{
								inFile >> s->surfaceColor.x;
								inFile >> s->surfaceColor.y;
								inFile >> s->surfaceColor.z;
							}
							else if (currentBit == "REFLECTION_VALUE")
							{
								inFile >> s->reflection;
							}
							else if (currentBit == "TRANSPARENCY_VALUE")
							{
								inFile >> s->transparency;
							}
							else if (currentBit == "EMISSIVE_COLOUR")
							{
								inFile >> s->emissionColor.x;
								inFile >> s->emissionColor.y;
								inFile >> s->emissionColor.z;
							}
						}
					}
					else if (currentBit == "END_KEY_FRAME")
					{
						// add all of the other spheres, that arn't already included in the current key frame
						keyFrames.push_back(kf);
						std::sort(keyFrames.begin(), keyFrames.end());
					}
				}
			}
		}

		inFile.close();

		std::cout << "See: " << statsOutputFileName << " for information about rendering the scene" << std::endl;

		std::ofstream statsFile(outputFolder + "/" + statsOutputFileName);

		statsFile.clear();

		// now that loaded the scene information, it's drawing time
		/*
		std::cout << "Loaded Scene from: " << sceneFile << std::endl;
		std::cout << "Output folder: " << outputFolder << std::endl;
		std::cout << "Frames Per Second to use on Video: " << fps << std::endl;
		std::cout << "Start of output file name: " << ppmFileNameStart << std::endl;
		std::cout << "video Duration: " << videoDuration << std::endl;
		std::cout << "Output video file: " << outputVideoFile << std::endl;
		std::cout << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;
		*/
		statsFile << "Loaded Scene from: " << sceneFile << std::endl;
		statsFile << "Output folder: " << outputFolder << std::endl;
		statsFile << "Frames Per Second to use on Video: " << fps << std::endl;
		statsFile << "Start of output file name: " << ppmFileNameStart << std::endl;
		statsFile << "video Duration: " << videoDuration << std::endl;
		statsFile << "Output video file: " << outputVideoFile << std::endl;
		statsFile << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;

		for (int i = 0; i < keyFrames.size(); i++)
		{
			statsFile << "------------------------\n";
			statsFile << "key frame: " << i << std::endl;
			statsFile << "Time point: " << keyFrames[i].timePoint << std::endl;
			statsFile << "\t-------------" << std::endl;
			for (int j = 0; j < keyFrames[i].spheres.size(); j++)
			{
				statsFile << "\tSphere " << j << std::endl;
				statsFile << "\tPOSITION " << keyFrames[i].spheres[j].center.x << " " << keyFrames[i].spheres[j].center.y << " " << keyFrames[i].spheres[j].center.z << std::endl;
				statsFile << "\tRADIUS " << keyFrames[i].spheres[j].radius << std::endl;
				statsFile << "\tSURFACE_COLOUR " << keyFrames[i].spheres[j].surfaceColor.x << " " << keyFrames[i].spheres[j].surfaceColor.y << " " << keyFrames[i].spheres[j].surfaceColor.z << std::endl;
				statsFile << "\tREFLECTION_VALUE  " << keyFrames[i].spheres[j].reflection << std::endl;
				statsFile << "\tTRANSPARENCY_VALUE " << keyFrames[i].spheres[j].transparency << std::endl;
				statsFile << "\tEMISSIVE_COLOUR  " << keyFrames[i].spheres[j].emissionColor.x << " " << keyFrames[i].spheres[j].emissionColor.y << " " << keyFrames[i].spheres[j].emissionColor.z << std::endl;
				statsFile << "\tID " << keyFrames[i].spheres[j].m_id << std::endl;
				statsFile << "\t----------------------------------\n";
			}
		}

		// calculate number of frames to render
		unsigned int nFramesToRender = (unsigned int)(videoDuration * (float)fps);
		using namespace std::chrono;

		steady_clock::time_point timeAtStartOfRender = steady_clock::now();

		float inAnimationTimePerFrame = 1.0f / (float)fps;
		// represents the time in the animation that passes between frames
		// not to be confused with the time taken to render the frame

		for (int frameNumber = 0; frameNumber < nFramesToRender; frameNumber++)
		{
			steady_clock::time_point timeAtStartOfFrame = steady_clock::now();

			// need to know the time point in the animation, that is being delt with
			float frameTimePoint = inAnimationTimePerFrame * (float)frameNumber;

			// determine the two key frames that will be interpolating between
			KeyFrame kf1, kf2;

			

			assignKeyFramesToLerpBetween(frameTimePoint, kf1, kf2, keyFrames);

			using namespace std;

			// now determine the weighting value for the lerp
			float lerpVal = weightValueForTimePoints(kf1.timePoint, kf2.timePoint, frameTimePoint);

			// get lerped sphere states, spheres should be stored the same order in each key frame
			// see fillKeyFrameWithMostRecentSphereStates() for clarification

			statsFile << "Stats for Frame: " << frameNumber << std::endl;
			statsFile << "Time point: " << frameTimePoint << std::endl;
			statsFile << "First Key frame starting point: " << kf1.timePoint << std::endl;
			statsFile << "Second Key frame starting point: " << kf2.timePoint << std::endl;
			statsFile << "Lerp Value: " << lerpVal << std::endl;

			vector<Sphere> lerpS;

			for (int i = 0; i < kf1.spheres.size(); i++)
			{
				Vec3f posL;

				using namespace Math::interpolation;
				
				// position
				posL.x = lerp(kf1.spheres[i].center.x, kf2.spheres[i].center.x, lerpVal);
				posL.y = lerp(kf1.spheres[i].center.y, kf2.spheres[i].center.y, lerpVal);
				posL.z = lerp(kf1.spheres[i].center.z, kf2.spheres[i].center.z, lerpVal);
				
				// radius
				float radL = lerp(kf1.spheres[i].radius, kf2.spheres[i].radius, lerpVal);
				
				// colour
				Vec3f colourL;
				colourL.x = lerp(kf1.spheres[i].surfaceColor.x, kf2.spheres[i].surfaceColor.x, lerpVal);
				colourL.y = lerp(kf1.spheres[i].surfaceColor.y, kf2.spheres[i].surfaceColor.y, lerpVal);
				colourL.z = lerp(kf1.spheres[i].surfaceColor.z, kf2.spheres[i].surfaceColor.z, lerpVal);

				// reflection value
				float reflectionValueL;
				reflectionValueL = lerp(kf1.spheres[i].reflection, kf2.spheres[i].reflection, lerpVal);

				// transparancy
				float transparancyL;
				transparancyL = lerp(kf1.spheres[i].transparency, kf2.spheres[i].transparency, lerpVal);

				Vec3f emmisiveColourL;
				emmisiveColourL.x = lerp(kf1.spheres[i].emissionColor.x, kf2.spheres[i].emissionColor.x, lerpVal);
				emmisiveColourL.y = lerp(kf1.spheres[i].emissionColor.y, kf2.spheres[i].emissionColor.y, lerpVal);
				emmisiveColourL.z = lerp(kf1.spheres[i].emissionColor.z, kf2.spheres[i].emissionColor.z, lerpVal);

				
				


				Sphere sLerped = Sphere(posL, radL, colourL, reflectionValueL, transparancyL, emmisiveColourL);

				if (Sphere::diffMathOnly(kf1.spheres[i], kf2.spheres[i]))
				{
					// only executed if theres a difference between kf1's sphere & kf2's sphere

					// write stats for the sphere (kf1,kf2, lerped)
					statsFile << "sphere difference: " << endl;
					statsFile << "\t\t________________________________________\n";
					statsFile << "\t\tKey frame 1 | key frame 2 | lerped value\n";

					if (!(kf1.spheres[i].center == kf2.spheres[i].center))
					{
						statsFile << "\t\tPOSITION " << kf1.spheres[i].center << " | " << kf2.spheres[i].center << " | " << sLerped.center << endl;
					}
					
					if (kf1.spheres[i].radius != kf2.spheres[i].radius)
					{
						statsFile << "\t\tRADIUS " << kf1.spheres[i].radius << " | " << kf2.spheres[i].radius << " | " << sLerped.radius << endl;
					}
					
					if (!(kf1.spheres[i].surfaceColor == kf2.spheres[i].surfaceColor))
					{
						statsFile << "\t\tSURFACE_COLOUR " << kf1.spheres[i].surfaceColor << " | " << kf2.spheres[i].surfaceColor << " | " << sLerped.surfaceColor << endl;
					}
					
					if (kf1.spheres[i].reflection != kf2.spheres[i].reflection)
					{
						statsFile << "\t\tREFLECTION_VALUE " << kf1.spheres[i].reflection << " | " << kf2.spheres[i].reflection << " | " << sLerped.reflection << endl;
					}
					
					if (kf1.spheres[i].transparency != kf2.spheres[i].transparency)
					{
						statsFile << "\t\tTRANSPARENCY_VALUE " << kf1.spheres[i].transparency << " | " << kf2.spheres[i].transparency << " | " << sLerped.transparency << endl;
					}
					
					if (!(kf1.spheres[i].emissionColor == kf2.spheres[i].emissionColor))
					{
						statsFile << "\t\tEMISSIVE_COLOUR " << kf1.spheres[i].emissionColor << " | " << kf2.spheres[i].emissionColor << " | " << sLerped.emissionColor << endl;
					}
				}

				lerpS.push_back(sLerped);
				
			}


			

			// got the lerped spheres, now render the frame
			rendering::renderToFolder(ppmFileNameStart, outputFolder, lerpS, frameNumber);

			
			steady_clock::time_point timeAtEndOfFrame = steady_clock::now();

			unsigned framesLeftToRender = nFramesToRender - frameNumber;
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

	}

	void renderKeyFrameSceneFromFileMultiThread(std::string sceneFile)
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

		std::vector<KeyFrame> keyFrames;

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
				Sphere tmpS(postion, radius, surfaceColour, reflectionValue, transparancy, emissiveColour);
				tmpS.m_id = sphereID;
				// add the std::vector
				spheres.push_back(tmpS);
			}
			else if (currentBit == "START_KEY_FRAME")
			{
				KeyFrame kf;

				if (keyFrames.size() == 0)
				{
					// it's the first key frame encountered

					// add a default key frame to the start, with default values
					KeyFrame firstKF;
					firstKF.timePoint = 0.0f;
					fillKeyFrameWithMostRecentSphereStates(firstKF, spheres);
					keyFrames.push_back(firstKF);
				}

				// fill kf with the values from the last key frame
				kf.timePoint = 0.0f;
				fillKeyFrameWithMostRecentSphereStates(kf, keyFrames[keyFrames.size() - 1].spheres);

				while (currentBit != "END_KEY_FRAME")
				{
					inFile >> currentBit;



					if (currentBit == "TIME_POINT")
					{
						inFile >> kf.timePoint;
					}
					else if (currentBit == "START_SPHERE_STATE")
					{
						Sphere * s = nullptr;
						while (currentBit != "END_SPHERE_STATE")
						{
							inFile >> currentBit;
							if (currentBit == "ID")
							{
								// the ID must always be set first in file

								// make s point to the sphere, with the matching ID

								std::string sphereID = "";
								inFile >> sphereID;

								for (int i = 0; i < kf.spheres.size(); i++)
								{
									if (kf.spheres[i].m_id == sphereID)
									{
										s = &kf.spheres[i];
									}
								}

								if (s == nullptr)
								{
									// couldn't find the correct sphere, send warning & return;
									std::cout << "Error: " << sphereID << " wasn't found" << std::endl;
									return;
								}
							}
							else if (currentBit == "POSITION")
							{
								inFile >> s->center.x;
								inFile >> s->center.y;
								inFile >> s->center.z;
							}
							else if (currentBit == "RADIUS")
							{
								inFile >> s->radius;
							}
							else if (currentBit == "SURFACE_COLOUR")
							{
								inFile >> s->surfaceColor.x;
								inFile >> s->surfaceColor.y;
								inFile >> s->surfaceColor.z;
							}
							else if (currentBit == "REFLECTION_VALUE")
							{
								inFile >> s->reflection;
							}
							else if (currentBit == "TRANSPARENCY_VALUE")
							{
								inFile >> s->transparency;
							}
							else if (currentBit == "EMISSIVE_COLOUR")
							{
								inFile >> s->emissionColor.x;
								inFile >> s->emissionColor.y;
								inFile >> s->emissionColor.z;
							}
						}
					}
					else if (currentBit == "END_KEY_FRAME")
					{
						// add all of the other spheres, that arn't already included in the current key frame
						keyFrames.push_back(kf);
						std::sort(keyFrames.begin(), keyFrames.end());
					}
				}
			}
		}

		inFile.close();

		std::cout << "See: " << statsOutputFileName << " for information about rendering the scene" << std::endl;

		std::ofstream statsFile(outputFolder + "/" + statsOutputFileName);

		statsFile.clear();



		// now that loaded the scene information, it's drawing time
		/*
		std::cout << "Loaded Scene from: " << sceneFile << std::endl;
		std::cout << "Output folder: " << outputFolder << std::endl;
		std::cout << "Frames Per Second to use on Video: " << fps << std::endl;
		std::cout << "Start of output file name: " << ppmFileNameStart << std::endl;
		std::cout << "video Duration: " << videoDuration << std::endl;
		std::cout << "Output video file: " << outputVideoFile << std::endl;
		std::cout << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;
		*/
		statsFile << "Loaded Scene from: " << sceneFile << std::endl;
		statsFile << "Output folder: " << outputFolder << std::endl;
		statsFile << "Frames Per Second to use on Video: " << fps << std::endl;
		statsFile << "Start of output file name: " << ppmFileNameStart << std::endl;
		statsFile << "video Duration: " << videoDuration << std::endl;
		statsFile << "Output video file: " << outputVideoFile << std::endl;
		statsFile << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;

		for (int i = 0; i < keyFrames.size(); i++)
		{
			statsFile << "------------------------\n";
			statsFile << "key frame: " << i << std::endl;
			statsFile << "Time point: " << keyFrames[i].timePoint << std::endl;
			statsFile << "\t-------------" << std::endl;
			for (int j = 0; j < keyFrames[i].spheres.size(); j++)
			{
				statsFile << "\tSphere " << j << std::endl;
				statsFile << "\tPOSITION " << keyFrames[i].spheres[j].center.x << " " << keyFrames[i].spheres[j].center.y << " " << keyFrames[i].spheres[j].center.z << std::endl;
				statsFile << "\tRADIUS " << keyFrames[i].spheres[j].radius << std::endl;
				statsFile << "\tSURFACE_COLOUR " << keyFrames[i].spheres[j].surfaceColor.x << " " << keyFrames[i].spheres[j].surfaceColor.y << " " << keyFrames[i].spheres[j].surfaceColor.z << std::endl;
				statsFile << "\tREFLECTION_VALUE  " << keyFrames[i].spheres[j].reflection << std::endl;
				statsFile << "\tTRANSPARENCY_VALUE " << keyFrames[i].spheres[j].transparency << std::endl;
				statsFile << "\tEMISSIVE_COLOUR  " << keyFrames[i].spheres[j].emissionColor.x << " " << keyFrames[i].spheres[j].emissionColor.y << " " << keyFrames[i].spheres[j].emissionColor.z << std::endl;
				statsFile << "\tID " << keyFrames[i].spheres[j].m_id << std::endl;
				statsFile << "\t----------------------------------\n";
			}
		}

		// calculate number of frames to render
		unsigned int nFramesToRender = (unsigned int)(videoDuration * (float)fps);
		using namespace std::chrono;

		steady_clock::time_point timeAtStartOfRender = steady_clock::now();

		float inAnimationTimePerFrame = 1.0f / (float)fps;
		// represents the time in the animation that passes between frames
		// not to be confused with the time taken to render the frame

		RenderManager rm(statsFile);
		rm.init();

		for (int frameNumber = 0; frameNumber < nFramesToRender; frameNumber++)
		{
			// steady_clock::time_point timeAtStartOfFrame = steady_clock::now();

			// need to know the time point in the animation, that is being delt with
			float frameTimePoint = inAnimationTimePerFrame * (float)frameNumber;

			// determine the two key frames that will be interpolating between
			KeyFrame kf1, kf2;



			assignKeyFramesToLerpBetween(frameTimePoint, kf1, kf2, keyFrames);

			using namespace std;

			// now determine the weighting value for the lerp
			float lerpVal = weightValueForTimePoints(kf1.timePoint, kf2.timePoint, frameTimePoint);

			// get lerped sphere states, spheres should be stored the same order in each key frame
			// see fillKeyFrameWithMostRecentSphereStates() for clarification

			statsFile << "Stats for Frame: " << frameNumber << std::endl;
			statsFile << "Time point: " << frameTimePoint << std::endl;
			statsFile << "First Key frame starting point: " << kf1.timePoint << std::endl;
			statsFile << "Second Key frame starting point: " << kf2.timePoint << std::endl;
			statsFile << "Lerp Value: " << lerpVal << std::endl;

			

			vector<Sphere> lerpS;

			for (int i = 0; i < kf1.spheres.size(); i++)
			{
				Vec3f posL;

				using namespace Math::interpolation;

				// position
				posL.x = lerp(kf1.spheres[i].center.x, kf2.spheres[i].center.x, lerpVal);
				posL.y = lerp(kf1.spheres[i].center.y, kf2.spheres[i].center.y, lerpVal);
				posL.z = lerp(kf1.spheres[i].center.z, kf2.spheres[i].center.z, lerpVal);

				// radius
				float radL = lerp(kf1.spheres[i].radius, kf2.spheres[i].radius, lerpVal);

				// colour
				Vec3f colourL;
				colourL.x = lerp(kf1.spheres[i].surfaceColor.x, kf2.spheres[i].surfaceColor.x, lerpVal);
				colourL.y = lerp(kf1.spheres[i].surfaceColor.y, kf2.spheres[i].surfaceColor.y, lerpVal);
				colourL.z = lerp(kf1.spheres[i].surfaceColor.z, kf2.spheres[i].surfaceColor.z, lerpVal);

				// reflection value
				float reflectionValueL;
				reflectionValueL = lerp(kf1.spheres[i].reflection, kf2.spheres[i].reflection, lerpVal);

				// transparancy
				float transparancyL;
				transparancyL = lerp(kf1.spheres[i].transparency, kf2.spheres[i].transparency, lerpVal);

				Vec3f emmisiveColourL;
				emmisiveColourL.x = lerp(kf1.spheres[i].emissionColor.x, kf2.spheres[i].emissionColor.x, lerpVal);
				emmisiveColourL.y = lerp(kf1.spheres[i].emissionColor.y, kf2.spheres[i].emissionColor.y, lerpVal);
				emmisiveColourL.z = lerp(kf1.spheres[i].emissionColor.z, kf2.spheres[i].emissionColor.z, lerpVal);





				Sphere sLerped = Sphere(posL, radL, colourL, reflectionValueL, transparancyL, emmisiveColourL);

				// if (Sphere::diffMathOnly(kf1.spheres[i], kf2.spheres[i])) // wasn't nessary (over complercated the output file)
				if (false)
				{
					// only executed if theres a difference between kf1's sphere & kf2's sphere

					// write stats for the sphere (kf1,kf2, lerped)
					statsFile << "sphere difference: " << endl;
					statsFile << "\t\t________________________________________\n";
					statsFile << "\t\tKey frame 1 | key frame 2 | lerped value\n";

					if (!(kf1.spheres[i].center == kf2.spheres[i].center))
					{
						statsFile << "\t\tPOSITION " << kf1.spheres[i].center << " | " << kf2.spheres[i].center << " | " << sLerped.center << endl;
					}

					if (kf1.spheres[i].radius != kf2.spheres[i].radius)
					{
						statsFile << "\t\tRADIUS " << kf1.spheres[i].radius << " | " << kf2.spheres[i].radius << " | " << sLerped.radius << endl;
					}

					if (!(kf1.spheres[i].surfaceColor == kf2.spheres[i].surfaceColor))
					{
						statsFile << "\t\tSURFACE_COLOUR " << kf1.spheres[i].surfaceColor << " | " << kf2.spheres[i].surfaceColor << " | " << sLerped.surfaceColor << endl;
					}

					if (kf1.spheres[i].reflection != kf2.spheres[i].reflection)
					{
						statsFile << "\t\tREFLECTION_VALUE " << kf1.spheres[i].reflection << " | " << kf2.spheres[i].reflection << " | " << sLerped.reflection << endl;
					}

					if (kf1.spheres[i].transparency != kf2.spheres[i].transparency)
					{
						statsFile << "\t\tTRANSPARENCY_VALUE " << kf1.spheres[i].transparency << " | " << kf2.spheres[i].transparency << " | " << sLerped.transparency << endl;
					}

					if (!(kf1.spheres[i].emissionColor == kf2.spheres[i].emissionColor))
					{
						statsFile << "\t\tEMISSIVE_COLOUR " << kf1.spheres[i].emissionColor << " | " << kf2.spheres[i].emissionColor << " | " << sLerped.emissionColor << endl;
					}
				}

				lerpS.push_back(sLerped);

			}

			Frame frameData;
			frameData.frameNumber = frameNumber;
			frameData.frameData = lerpS;
			frameData.outPutFolder = outputFolder;
			frameData.startOfFrameFileNameString = ppmFileNameStart;

			rm.addFrame(frameData);

			// add to the renderManager
		}

		// rm now stores all of the data needed to render every frame
		// rm.renderFramesSingleThread(outputVideoFile, fps);
		rm.renderFramesMultiThread(outputVideoFile, fps, true);
		// rm.renderFramesMultiThreadPerFrame(outputVideoFile, fps);
		
	}

	void renderKeyFrameSceneFromFileMultiThreadPreFrame(std::string sceneFile)
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

		std::vector<KeyFrame> keyFrames;

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
				Sphere tmpS(postion, radius, surfaceColour, reflectionValue, transparancy, emissiveColour);
				tmpS.m_id = sphereID;
				// add the std::vector
				spheres.push_back(tmpS);
			}
			else if (currentBit == "START_KEY_FRAME")
			{
				KeyFrame kf;

				if (keyFrames.size() == 0)
				{
					// it's the first key frame encountered

					// add a default key frame to the start, with default values
					KeyFrame firstKF;
					firstKF.timePoint = 0.0f;
					fillKeyFrameWithMostRecentSphereStates(firstKF, spheres);
					keyFrames.push_back(firstKF);
				}

				// fill kf with the values from the last key frame
				kf.timePoint = 0.0f;
				fillKeyFrameWithMostRecentSphereStates(kf, keyFrames[keyFrames.size() - 1].spheres);

				while (currentBit != "END_KEY_FRAME")
				{
					inFile >> currentBit;



					if (currentBit == "TIME_POINT")
					{
						inFile >> kf.timePoint;
					}
					else if (currentBit == "START_SPHERE_STATE")
					{
						Sphere * s = nullptr;
						while (currentBit != "END_SPHERE_STATE")
						{
							inFile >> currentBit;
							if (currentBit == "ID")
							{
								// the ID must always be set first in file

								// make s point to the sphere, with the matching ID

								std::string sphereID = "";
								inFile >> sphereID;

								for (int i = 0; i < kf.spheres.size(); i++)
								{
									if (kf.spheres[i].m_id == sphereID)
									{
										s = &kf.spheres[i];
									}
								}

								if (s == nullptr)
								{
									// couldn't find the correct sphere, send warning & return;
									std::cout << "Error: " << sphereID << " wasn't found" << std::endl;
									return;
								}
							}
							else if (currentBit == "POSITION")
							{
								inFile >> s->center.x;
								inFile >> s->center.y;
								inFile >> s->center.z;
							}
							else if (currentBit == "RADIUS")
							{
								inFile >> s->radius;
							}
							else if (currentBit == "SURFACE_COLOUR")
							{
								inFile >> s->surfaceColor.x;
								inFile >> s->surfaceColor.y;
								inFile >> s->surfaceColor.z;
							}
							else if (currentBit == "REFLECTION_VALUE")
							{
								inFile >> s->reflection;
							}
							else if (currentBit == "TRANSPARENCY_VALUE")
							{
								inFile >> s->transparency;
							}
							else if (currentBit == "EMISSIVE_COLOUR")
							{
								inFile >> s->emissionColor.x;
								inFile >> s->emissionColor.y;
								inFile >> s->emissionColor.z;
							}
						}
					}
					else if (currentBit == "END_KEY_FRAME")
					{
						// add all of the other spheres, that arn't already included in the current key frame
						keyFrames.push_back(kf);
						std::sort(keyFrames.begin(), keyFrames.end());
					}
				}
			}
		}

		inFile.close();

		std::cout << "See: " << statsOutputFileName << " for information about rendering the scene" << std::endl;

		std::ofstream statsFile(outputFolder + "/" + statsOutputFileName);

		statsFile.clear();



		// now that loaded the scene information, it's drawing time
		/*
		std::cout << "Loaded Scene from: " << sceneFile << std::endl;
		std::cout << "Output folder: " << outputFolder << std::endl;
		std::cout << "Frames Per Second to use on Video: " << fps << std::endl;
		std::cout << "Start of output file name: " << ppmFileNameStart << std::endl;
		std::cout << "video Duration: " << videoDuration << std::endl;
		std::cout << "Output video file: " << outputVideoFile << std::endl;
		std::cout << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;
		*/
		statsFile << "Loaded Scene from: " << sceneFile << std::endl;
		statsFile << "Output folder: " << outputFolder << std::endl;
		statsFile << "Frames Per Second to use on Video: " << fps << std::endl;
		statsFile << "Start of output file name: " << ppmFileNameStart << std::endl;
		statsFile << "video Duration: " << videoDuration << std::endl;
		statsFile << "Output video file: " << outputVideoFile << std::endl;
		statsFile << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;

		for (int i = 0; i < keyFrames.size(); i++)
		{
			statsFile << "------------------------\n";
			statsFile << "key frame: " << i << std::endl;
			statsFile << "Time point: " << keyFrames[i].timePoint << std::endl;
			statsFile << "\t-------------" << std::endl;
			for (int j = 0; j < keyFrames[i].spheres.size(); j++)
			{
				statsFile << "\tSphere " << j << std::endl;
				statsFile << "\tPOSITION " << keyFrames[i].spheres[j].center.x << " " << keyFrames[i].spheres[j].center.y << " " << keyFrames[i].spheres[j].center.z << std::endl;
				statsFile << "\tRADIUS " << keyFrames[i].spheres[j].radius << std::endl;
				statsFile << "\tSURFACE_COLOUR " << keyFrames[i].spheres[j].surfaceColor.x << " " << keyFrames[i].spheres[j].surfaceColor.y << " " << keyFrames[i].spheres[j].surfaceColor.z << std::endl;
				statsFile << "\tREFLECTION_VALUE  " << keyFrames[i].spheres[j].reflection << std::endl;
				statsFile << "\tTRANSPARENCY_VALUE " << keyFrames[i].spheres[j].transparency << std::endl;
				statsFile << "\tEMISSIVE_COLOUR  " << keyFrames[i].spheres[j].emissionColor.x << " " << keyFrames[i].spheres[j].emissionColor.y << " " << keyFrames[i].spheres[j].emissionColor.z << std::endl;
				statsFile << "\tID " << keyFrames[i].spheres[j].m_id << std::endl;
				statsFile << "\t----------------------------------\n";
			}
		}

		// calculate number of frames to render
		unsigned int nFramesToRender = (unsigned int)(videoDuration * (float)fps);
		using namespace std::chrono;

		steady_clock::time_point timeAtStartOfRender = steady_clock::now();

		float inAnimationTimePerFrame = 1.0f / (float)fps;
		// represents the time in the animation that passes between frames
		// not to be confused with the time taken to render the frame

		RenderManager rm(statsFile);
		rm.init();

		for (int frameNumber = 0; frameNumber < nFramesToRender; frameNumber++)
		{
			// steady_clock::time_point timeAtStartOfFrame = steady_clock::now();

			// need to know the time point in the animation, that is being delt with
			float frameTimePoint = inAnimationTimePerFrame * (float)frameNumber;

			// determine the two key frames that will be interpolating between
			KeyFrame kf1, kf2;



			assignKeyFramesToLerpBetween(frameTimePoint, kf1, kf2, keyFrames);

			using namespace std;

			// now determine the weighting value for the lerp
			float lerpVal = weightValueForTimePoints(kf1.timePoint, kf2.timePoint, frameTimePoint);

			// get lerped sphere states, spheres should be stored the same order in each key frame
			// see fillKeyFrameWithMostRecentSphereStates() for clarification

			statsFile << "Stats for Frame: " << frameNumber << std::endl;
			statsFile << "Time point: " << frameTimePoint << std::endl;
			statsFile << "First Key frame starting point: " << kf1.timePoint << std::endl;
			statsFile << "Second Key frame starting point: " << kf2.timePoint << std::endl;
			statsFile << "Lerp Value: " << lerpVal << std::endl;



			vector<Sphere> lerpS;

			for (int i = 0; i < kf1.spheres.size(); i++)
			{
				Vec3f posL;

				using namespace Math::interpolation;

				// position
				posL.x = lerp(kf1.spheres[i].center.x, kf2.spheres[i].center.x, lerpVal);
				posL.y = lerp(kf1.spheres[i].center.y, kf2.spheres[i].center.y, lerpVal);
				posL.z = lerp(kf1.spheres[i].center.z, kf2.spheres[i].center.z, lerpVal);

				// radius
				float radL = lerp(kf1.spheres[i].radius, kf2.spheres[i].radius, lerpVal);

				// colour
				Vec3f colourL;
				colourL.x = lerp(kf1.spheres[i].surfaceColor.x, kf2.spheres[i].surfaceColor.x, lerpVal);
				colourL.y = lerp(kf1.spheres[i].surfaceColor.y, kf2.spheres[i].surfaceColor.y, lerpVal);
				colourL.z = lerp(kf1.spheres[i].surfaceColor.z, kf2.spheres[i].surfaceColor.z, lerpVal);

				// reflection value
				float reflectionValueL;
				reflectionValueL = lerp(kf1.spheres[i].reflection, kf2.spheres[i].reflection, lerpVal);

				// transparancy
				float transparancyL;
				transparancyL = lerp(kf1.spheres[i].transparency, kf2.spheres[i].transparency, lerpVal);

				Vec3f emmisiveColourL;
				emmisiveColourL.x = lerp(kf1.spheres[i].emissionColor.x, kf2.spheres[i].emissionColor.x, lerpVal);
				emmisiveColourL.y = lerp(kf1.spheres[i].emissionColor.y, kf2.spheres[i].emissionColor.y, lerpVal);
				emmisiveColourL.z = lerp(kf1.spheres[i].emissionColor.z, kf2.spheres[i].emissionColor.z, lerpVal);





				Sphere sLerped = Sphere(posL, radL, colourL, reflectionValueL, transparancyL, emmisiveColourL);

				// if (Sphere::diffMathOnly(kf1.spheres[i], kf2.spheres[i])) // wasn't nessary (over complercated the output file)
				if (false)
				{
					// only executed if theres a difference between kf1's sphere & kf2's sphere

					// write stats for the sphere (kf1,kf2, lerped)
					statsFile << "sphere difference: " << endl;
					statsFile << "\t\t________________________________________\n";
					statsFile << "\t\tKey frame 1 | key frame 2 | lerped value\n";

					if (!(kf1.spheres[i].center == kf2.spheres[i].center))
					{
						statsFile << "\t\tPOSITION " << kf1.spheres[i].center << " | " << kf2.spheres[i].center << " | " << sLerped.center << endl;
					}

					if (kf1.spheres[i].radius != kf2.spheres[i].radius)
					{
						statsFile << "\t\tRADIUS " << kf1.spheres[i].radius << " | " << kf2.spheres[i].radius << " | " << sLerped.radius << endl;
					}

					if (!(kf1.spheres[i].surfaceColor == kf2.spheres[i].surfaceColor))
					{
						statsFile << "\t\tSURFACE_COLOUR " << kf1.spheres[i].surfaceColor << " | " << kf2.spheres[i].surfaceColor << " | " << sLerped.surfaceColor << endl;
					}

					if (kf1.spheres[i].reflection != kf2.spheres[i].reflection)
					{
						statsFile << "\t\tREFLECTION_VALUE " << kf1.spheres[i].reflection << " | " << kf2.spheres[i].reflection << " | " << sLerped.reflection << endl;
					}

					if (kf1.spheres[i].transparency != kf2.spheres[i].transparency)
					{
						statsFile << "\t\tTRANSPARENCY_VALUE " << kf1.spheres[i].transparency << " | " << kf2.spheres[i].transparency << " | " << sLerped.transparency << endl;
					}

					if (!(kf1.spheres[i].emissionColor == kf2.spheres[i].emissionColor))
					{
						statsFile << "\t\tEMISSIVE_COLOUR " << kf1.spheres[i].emissionColor << " | " << kf2.spheres[i].emissionColor << " | " << sLerped.emissionColor << endl;
					}
				}

				lerpS.push_back(sLerped);

			}

			Frame frameData;
			frameData.frameNumber = frameNumber;
			frameData.frameData = lerpS;
			frameData.outPutFolder = outputFolder;
			frameData.startOfFrameFileNameString = ppmFileNameStart;

			rm.addFrame(frameData);

			// add to the renderManager
		}




		// rm now stores all of the data needed to render every frame
		// rm.renderFramesSingleThread(outputVideoFile, fps);
		// rm.renderFramesMultiThread(outputVideoFile, fps);
		rm.renderFramesMultiThreadPerFrame(outputVideoFile, fps, true);

	}

	void renderKeyFrameScene(std::string sceneFile)
	{
		std::string actualFilePath = PS4_VISUAL_STUDIO_DIR;
		actualFilePath += sceneFile;
		std::ifstream inFile(actualFilePath);
		if (!inFile.good())
		{
			std::cout << "Error " << sceneFile << " not found" << std::endl;
			return;
		}

		// parse the file for the scene information
		// duration in seconds, FPS, ppm file name(just first bit), output video file name, outputFolder
		std::string currentBit = "";

		std::vector<Sphere> spheres;

		std::vector<KeyFrame> keyFrames;

		std::string outputFolder = "DEFAULT_OUTPUT_FOLDER";
		int fps = 24;
		float videoDuration = 10.0f;
		std::string ppmFileNameStart = "DEFAULT_PPM_";
		std::string statsOutputFileName = "DEFAULT_STATS_OUTPUT.txt";
		std::string outputVideoFile = "DEFAUL_OUT.avi";

		std::string threadUsage = "SINGLE"; // could also be MULTI_PER_FRAME or MULTI_PER_VIDEO
		bool deletePPMs = true;


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
			else if (currentBit == "threadUsage:")
			{
				inFile >> threadUsage;
			}
			else if (currentBit == "cleanUpFrameFiles:")
			{
				std::string cleanUpStr = "YES";
				inFile >> cleanUpStr;
				if (cleanUpStr == "YES")
				{
					deletePPMs = true;
				}
				else
				{
					deletePPMs = false;
				}
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
				Sphere tmpS(postion, radius, surfaceColour, reflectionValue, transparancy, emissiveColour);
				tmpS.m_id = sphereID;
				// add the std::vector
				spheres.push_back(tmpS);
			}
			else if (currentBit == "START_KEY_FRAME")
			{
				KeyFrame kf;

				if (keyFrames.size() == 0)
				{
					// it's the first key frame encountered

					// add a default key frame to the start, with default values
					KeyFrame firstKF;
					firstKF.timePoint = 0.0f;
					fillKeyFrameWithMostRecentSphereStates(firstKF, spheres);
					keyFrames.push_back(firstKF);
				}

				// fill kf with the values from the last key frame
				kf.timePoint = 0.0f;
				fillKeyFrameWithMostRecentSphereStates(kf, keyFrames[keyFrames.size() - 1].spheres);

				while (currentBit != "END_KEY_FRAME")
				{
					inFile >> currentBit;



					if (currentBit == "TIME_POINT")
					{
						inFile >> kf.timePoint;
					}
					else if (currentBit == "START_SPHERE_STATE")
					{
						Sphere * s = nullptr;
						while (currentBit != "END_SPHERE_STATE")
						{
							inFile >> currentBit;
							if (currentBit == "ID")
							{
								// the ID must always be set first in file

								// make s point to the sphere, with the matching ID

								std::string sphereID = "";
								inFile >> sphereID;

								for (int i = 0; i < kf.spheres.size(); i++)
								{
									if (kf.spheres[i].m_id == sphereID)
									{
										s = &kf.spheres[i];
									}
								}

								if (s == nullptr)
								{
									// couldn't find the correct sphere, send warning & return;
									std::cout << "Error: " << sphereID << " wasn't found" << std::endl;
									return;
								}
							}
							else if (currentBit == "POSITION")
							{
								inFile >> s->center.x;
								inFile >> s->center.y;
								inFile >> s->center.z;
							}
							else if (currentBit == "RADIUS")
							{
								inFile >> s->radius;
							}
							else if (currentBit == "SURFACE_COLOUR")
							{
								inFile >> s->surfaceColor.x;
								inFile >> s->surfaceColor.y;
								inFile >> s->surfaceColor.z;
							}
							else if (currentBit == "REFLECTION_VALUE")
							{
								inFile >> s->reflection;
							}
							else if (currentBit == "TRANSPARENCY_VALUE")
							{
								inFile >> s->transparency;
							}
							else if (currentBit == "EMISSIVE_COLOUR")
							{
								inFile >> s->emissionColor.x;
								inFile >> s->emissionColor.y;
								inFile >> s->emissionColor.z;
							}
						}
					}
					else if (currentBit == "END_KEY_FRAME")
					{
						// add all of the other spheres, that arn't already included in the current key frame
						keyFrames.push_back(kf);
						std::sort(keyFrames.begin(), keyFrames.end());
					}
				}
			}
		}

		inFile.close();

		std::cout << "See: " << statsOutputFileName << " for information about rendering the scene" << std::endl;

		// find folder code here
		if (!folderExists(outputFolder))
		{
			createFolder(outputFolder);
		}


		std::ofstream statsFile(outputFolder + "/" + statsOutputFileName);

		statsFile.clear();



		// now that loaded the scene information, it's drawing time
		/*
		std::cout << "Loaded Scene from: " << sceneFile << std::endl;
		std::cout << "Output folder: " << outputFolder << std::endl;
		std::cout << "Frames Per Second to use on Video: " << fps << std::endl;
		std::cout << "Start of output file name: " << ppmFileNameStart << std::endl;
		std::cout << "video Duration: " << videoDuration << std::endl;
		std::cout << "Output video file: " << outputVideoFile << std::endl;
		std::cout << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;
		*/
		statsFile << "Loaded Scene from: " << sceneFile << std::endl;
		statsFile << "Output folder: " << outputFolder << std::endl;
		statsFile << "Frames Per Second to use on Video: " << fps << std::endl;
		statsFile << "Start of output file name: " << ppmFileNameStart << std::endl;
		statsFile << "video Duration: " << videoDuration << std::endl;
		statsFile << "Output video file: " << outputVideoFile << std::endl;
		statsFile << "Total key frames (number in the file + 1): " << keyFrames.size() << std::endl;

		for (int i = 0; i < keyFrames.size(); i++)
		{
			statsFile << "------------------------\n";
			statsFile << "key frame: " << i << std::endl;
			statsFile << "Time point: " << keyFrames[i].timePoint << std::endl;
			statsFile << "\t-------------" << std::endl;
			for (int j = 0; j < keyFrames[i].spheres.size(); j++)
			{
				statsFile << "\tSphere " << j << std::endl;
				statsFile << "\tPOSITION " << keyFrames[i].spheres[j].center.x << " " << keyFrames[i].spheres[j].center.y << " " << keyFrames[i].spheres[j].center.z << std::endl;
				statsFile << "\tRADIUS " << keyFrames[i].spheres[j].radius << std::endl;
				statsFile << "\tSURFACE_COLOUR " << keyFrames[i].spheres[j].surfaceColor.x << " " << keyFrames[i].spheres[j].surfaceColor.y << " " << keyFrames[i].spheres[j].surfaceColor.z << std::endl;
				statsFile << "\tREFLECTION_VALUE  " << keyFrames[i].spheres[j].reflection << std::endl;
				statsFile << "\tTRANSPARENCY_VALUE " << keyFrames[i].spheres[j].transparency << std::endl;
				statsFile << "\tEMISSIVE_COLOUR  " << keyFrames[i].spheres[j].emissionColor.x << " " << keyFrames[i].spheres[j].emissionColor.y << " " << keyFrames[i].spheres[j].emissionColor.z << std::endl;
				statsFile << "\tID " << keyFrames[i].spheres[j].m_id << std::endl;
				statsFile << "\t----------------------------------\n";
			}
		}

		// calculate number of frames to render
		unsigned int nFramesToRender = (unsigned int)(videoDuration * (float)fps);
		using namespace std::chrono;

		steady_clock::time_point timeAtStartOfRender = steady_clock::now();

		float inAnimationTimePerFrame = 1.0f / (float)fps;
		// represents the time in the animation that passes between frames
		// not to be confused with the time taken to render the frame

		RenderManager rm(statsFile);
		rm.init();

		for (int frameNumber = 0; frameNumber < nFramesToRender; frameNumber++)
		{
			// steady_clock::time_point timeAtStartOfFrame = steady_clock::now();

			// need to know the time point in the animation, that is being delt with
			float frameTimePoint = inAnimationTimePerFrame * (float)frameNumber;

			// determine the two key frames that will be interpolating between
			KeyFrame kf1, kf2;



			assignKeyFramesToLerpBetween(frameTimePoint, kf1, kf2, keyFrames);

			using namespace std;

			// now determine the weighting value for the lerp
			float lerpVal = weightValueForTimePoints(kf1.timePoint, kf2.timePoint, frameTimePoint);

			// get lerped sphere states, spheres should be stored the same order in each key frame
			// see fillKeyFrameWithMostRecentSphereStates() for clarification

			statsFile << "Stats for Frame: " << frameNumber << std::endl;
			statsFile << "Time point: " << frameTimePoint << std::endl;
			statsFile << "First Key frame starting point: " << kf1.timePoint << std::endl;
			statsFile << "Second Key frame starting point: " << kf2.timePoint << std::endl;
			statsFile << "Lerp Value: " << lerpVal << std::endl;



			vector<Sphere> lerpS;

			for (int i = 0; i < kf1.spheres.size(); i++)
			{
				Vec3f posL;

				using namespace Math::interpolation;

				// position
				posL.x = lerp(kf1.spheres[i].center.x, kf2.spheres[i].center.x, lerpVal);
				posL.y = lerp(kf1.spheres[i].center.y, kf2.spheres[i].center.y, lerpVal);
				posL.z = lerp(kf1.spheres[i].center.z, kf2.spheres[i].center.z, lerpVal);

				// radius
				float radL = lerp(kf1.spheres[i].radius, kf2.spheres[i].radius, lerpVal);

				// colour
				Vec3f colourL;
				colourL.x = lerp(kf1.spheres[i].surfaceColor.x, kf2.spheres[i].surfaceColor.x, lerpVal);
				colourL.y = lerp(kf1.spheres[i].surfaceColor.y, kf2.spheres[i].surfaceColor.y, lerpVal);
				colourL.z = lerp(kf1.spheres[i].surfaceColor.z, kf2.spheres[i].surfaceColor.z, lerpVal);

				// reflection value
				float reflectionValueL;
				reflectionValueL = lerp(kf1.spheres[i].reflection, kf2.spheres[i].reflection, lerpVal);

				// transparancy
				float transparancyL;
				transparancyL = lerp(kf1.spheres[i].transparency, kf2.spheres[i].transparency, lerpVal);

				Vec3f emmisiveColourL;
				emmisiveColourL.x = lerp(kf1.spheres[i].emissionColor.x, kf2.spheres[i].emissionColor.x, lerpVal);
				emmisiveColourL.y = lerp(kf1.spheres[i].emissionColor.y, kf2.spheres[i].emissionColor.y, lerpVal);
				emmisiveColourL.z = lerp(kf1.spheres[i].emissionColor.z, kf2.spheres[i].emissionColor.z, lerpVal);





				Sphere sLerped = Sphere(posL, radL, colourL, reflectionValueL, transparancyL, emmisiveColourL);

				// if (Sphere::diffMathOnly(kf1.spheres[i], kf2.spheres[i])) // wasn't nessary (over complercated the output file)
				if (false)
				{
					// only executed if theres a difference between kf1's sphere & kf2's sphere

					// write stats for the sphere (kf1,kf2, lerped)
					statsFile << "sphere difference: " << endl;
					statsFile << "\t\t________________________________________\n";
					statsFile << "\t\tKey frame 1 | key frame 2 | lerped value\n";

					if (!(kf1.spheres[i].center == kf2.spheres[i].center))
					{
						statsFile << "\t\tPOSITION " << kf1.spheres[i].center << " | " << kf2.spheres[i].center << " | " << sLerped.center << endl;
					}

					if (kf1.spheres[i].radius != kf2.spheres[i].radius)
					{
						statsFile << "\t\tRADIUS " << kf1.spheres[i].radius << " | " << kf2.spheres[i].radius << " | " << sLerped.radius << endl;
					}

					if (!(kf1.spheres[i].surfaceColor == kf2.spheres[i].surfaceColor))
					{
						statsFile << "\t\tSURFACE_COLOUR " << kf1.spheres[i].surfaceColor << " | " << kf2.spheres[i].surfaceColor << " | " << sLerped.surfaceColor << endl;
					}

					if (kf1.spheres[i].reflection != kf2.spheres[i].reflection)
					{
						statsFile << "\t\tREFLECTION_VALUE " << kf1.spheres[i].reflection << " | " << kf2.spheres[i].reflection << " | " << sLerped.reflection << endl;
					}

					if (kf1.spheres[i].transparency != kf2.spheres[i].transparency)
					{
						statsFile << "\t\tTRANSPARENCY_VALUE " << kf1.spheres[i].transparency << " | " << kf2.spheres[i].transparency << " | " << sLerped.transparency << endl;
					}

					if (!(kf1.spheres[i].emissionColor == kf2.spheres[i].emissionColor))
					{
						statsFile << "\t\tEMISSIVE_COLOUR " << kf1.spheres[i].emissionColor << " | " << kf2.spheres[i].emissionColor << " | " << sLerped.emissionColor << endl;
					}
				}

				lerpS.push_back(sLerped);

			}

			Frame frameData;
			frameData.frameNumber = frameNumber;
			frameData.frameData = lerpS;
			frameData.outPutFolder = outputFolder;
			frameData.startOfFrameFileNameString = ppmFileNameStart;

			rm.addFrame(frameData);

			// add to the renderManager
		}




		// rm now stores all of the data needed to render every frame
		// rm.renderFramesSingleThread(outputVideoFile, fps);
		// rm.renderFramesMultiThread(outputVideoFile, fps);
		// rm.renderFramesMultiThreadPerFrame(outputVideoFile, fps);
		if (threadUsage == "SINGLE")
		{
			rm.renderFramesSingleThread(outputVideoFile, fps, deletePPMs);
		}
		else if (threadUsage == "MULTI_PER_FRAME")
		{
			rm.renderFramesMultiThreadPerFrame(outputVideoFile, fps, deletePPMs);
		}
		else if (threadUsage == "MULTI_PER_VIDEO")
		{
			rm.renderFramesMultiThread(outputVideoFile, fps, deletePPMs);
		}

		rm.clearForNextScene();
	}

};



// side note compiler optimisation, only works on Relese Builds

#endif