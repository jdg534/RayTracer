#ifndef _RENDER_MANAGER_H_
#define _RENDER_MANAGER_H_

#include "Rendering.h"
// #include "Frame.h"
#include "KeyFrame.h"

#include <thread>
#include <queue>
#include <chrono> // for determining the total rendering time
// this class is to use the functions in Rendering .h to handle the actual rendering, will be using threads to seperate drawing the frames accross 

#include <ostream> // for the output

#include <iostream> // for cout

#include "Math.h" // my math's library, for lerp

#include "RenderingStructs.h"

// now the fibers
#include <sce_fiber.h>
#include <libsysmodule.h>
#include "FibersAssist.h"




std::vector<FrameRenderDuration> g_FRDs; // just resize and override




class RenderManager
{
public:
	RenderManager(std::ostream & statsOutputStream):
		m_statsOutput(statsOutputStream)
	{
	}

	~RenderManager()
	{
	}


	void init()
	{
		m_nThreadsSupportedByPlatform = std::thread::hardware_concurrency();
	}

	void makeRoomForFrames(unsigned int nFrames)
	{
		// m_toRender.resize(nFrames);
		m_numFramesToRender = nFrames;
		m_framesToRender = new LerpedFrame[nFrames];
		// m_frameRenderDurations = new FrameRenderDuration[nFrames];
		g_FRDs.resize(nFrames);
	}

	void addLerpFrame(LerpedFrame lf)
	{
		m_framesToRender[lf.frameNumber] = lf;
	}

	void renderFramesSingleThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName)
	{
		// std::sort(m_toRender.begin(), m_toRender.end());
		// made changes & would be presented in order anway

		// unsigned int nFramesToRender = m_toRender.size();
		unsigned int nFramesToRender = m_numFramesToRender;
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();
		renderFrameRange(0, nFramesToRender, frameFileName, outputFolder);


		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();

		rendering::finshRenderToFolderWithCleanUp(outputFolder, frameFileName, outputVideoFile, fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();

		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void renderFramesMultiThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName)
	{
		//std::sort(m_toRender.begin(), m_toRender.end());

		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();

		unsigned int nFramesToRender = m_numFramesToRender;
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		m_statsOutput << "Num threads used: " << m_nThreadsSupportedByPlatform << std::endl;
		
		unsigned int nFramesToRenderPerThread = nFramesToRender / m_nThreadsSupportedByPlatform;

		unsigned int nFramesToRenderOnMainThread = 0;
		unsigned int frameIndexForMainThread = 0;
		


		// std::vector<std::thread *> renderingThreads;

		unsigned int nFramesPerFiber = nFramesToRender / MAX_FIBERS;

		FiberFrameRange frameInfoToPass[MAX_FIBERS];
		SceFiber fibers[MAX_FIBERS];
		char fiberContextBuffers[MAX_FIBERS][FIBER_CONTEXT_BUFFER_SIZE];

		for (int i = 0; i < MAX_FIBERS; i++)
		{
			int startFrameIndex = i * nFramesPerFiber;
			int endFrameIndex = startFrameIndex + nFramesPerFiber;
			m_statsOutput << "Fiber " << i << " was responsable for frames " << startFrameIndex << " to " << endFrameIndex - 1 << std::endl;
			if (i == MAX_FIBERS - 1)
			{
				nFramesToRenderOnMainThread = nFramesToRender - endFrameIndex;
				frameIndexForMainThread = endFrameIndex + 1;
			}

			frameInfoToPass[i].lerpedFramesArray = m_framesToRender;
			frameInfoToPass[i].outputFileStart = frameFileName;
			frameInfoToPass[i].outputFolder = outputFolder;
			frameInfoToPass[i].rangeStart = startFrameIndex;
			frameInfoToPass[i].rangeEnd = endFrameIndex;
			
			// now create & run the fiber
			int fiberCreateResults = sceFiberInitialize(&fibers[i], "FRAME_FIBER", fiberStartFuncForMultiFrames, (uint64_t)&frameInfoToPass[i], (void *)fiberContextBuffers[i], FIBER_CONTEXT_BUFFER_SIZE, NULL);
			assert(fiberCreateResults == SCE_OK);
			// run it!!!
			uint64_t fiberReturnArgs = 0;
			fiberCreateResults = sceFiberRun(&fibers[i], 0, &fiberReturnArgs);
			assert(fiberCreateResults == SCE_OK);

		}


		/*
		for (short i = 0; i < m_nThreadsSupportedByPlatform; i++) // unlikely to have more than 2 ^ 16 threads
		{
			unsigned int startFrameIndex = i * nFramesToRenderPerThread;
			unsigned int endFrameIndex = startFrameIndex + nFramesToRenderPerThread;

			m_statsOutput << "Thread " << i << " was responsable for frames " << startFrameIndex << " to " << endFrameIndex - 1 << std::endl;

			if (i == m_nThreadsSupportedByPlatform - 1)
			{
				nFramesToRenderOnMainThread = nFramesToRender - endFrameIndex; // not needed
				frameIndexForMainThread = endFrameIndex + 1;
			}
			// create a thread that runs
			// std::thread * t = new std::thread(&RenderManager::renderFrameRange, this, startFrameIndex, endFrameIndex);

			// std::thread * t = new std::thread(&RenderManager::renderFrameRange, startFrameIndex, endFrameIndex);

			// std::thread * t = new std::thread(renderFramesStatic, m_toRender, startFrameIndex, endFrameIndex);//, g_FRDs);

			// renderingThreads.push_back(t);
		}
		*/

		renderFrameRange(frameIndexForMainThread, nFramesToRender,frameFileName, outputFolder); // have the main thread deal with any left overs

		// finsh off the fibers
		for (int i = 0; i < MAX_FIBERS; i++)
		{
			bool fiberDone = false;
			int fiberFinshRes = sceFiberFinalize(&fibers[i]);
			if (fiberFinshRes == SCE_OK)
			{
				fiberDone = true;
			}
			else if (fiberFinshRes == SCE_FIBER_ERROR_STATE)
			{
				fiberDone = false; // it's still going
			}
			else
			{
				assert(false); // something is wrong!
			}

			if (!fiberDone)
			{
				i--; // go back and try again
			}
		}
		

		
		
		std::string outVidFile = outputVideoFile;



		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();

		// finshRenderToFolderWithCleanUp(std::string folderStr, std::string framefileNameStartStr, std::string outputFileName, int nFrames, unsigned int outputFps = 30, bool deletePPMFiles = true)

		rendering::finshRenderToFolderWithCleanUp(outputFolder, frameFileName, outVidFile, nFramesToRender,fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();


		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void renderFramesMultiThreadPerFrame(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName)
	{
		// std::sort(m_toRender.begin(), m_toRender.end());
		unsigned int nFramesToRender = m_numFramesToRender;
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();
		
		
		
		// renderFrameRange(0, nFramesToRender);

		for (int i = 0; i < nFramesToRender; i++)
		{
			std::vector<Sphere> spheres;

			LerpedFrame lf = m_framesToRender[i];


			for (int j = 0; j < lf.a->spheres.size(); j++)
			{
				Vec3f pos;
				float rad;
				Vec3f surfaceColour;
				float reflection;
				float transparency;
				Vec3f emmisive;
				pos.x = Math::interpolation::lerp(lf.a->spheres[j].center.x, lf.b->spheres[j].center.x, lf.lerpWeight);
				pos.y = Math::interpolation::lerp(lf.a->spheres[j].center.y, lf.b->spheres[j].center.y, lf.lerpWeight);
				pos.z = Math::interpolation::lerp(lf.a->spheres[j].center.z, lf.b->spheres[j].center.z, lf.lerpWeight);

				rad = Math::interpolation::lerp(lf.a->spheres[j].radius, lf.b->spheres[j].radius, lf.lerpWeight);

				surfaceColour.x = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.x, lf.b->spheres[j].surfaceColor.x, lf.lerpWeight);
				surfaceColour.y = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.y, lf.b->spheres[j].surfaceColor.y, lf.lerpWeight);
				surfaceColour.z = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.z, lf.b->spheres[j].surfaceColor.z, lf.lerpWeight);

				reflection = Math::interpolation::lerp(lf.a->spheres[j].reflection, lf.b->spheres[j].reflection, lf.lerpWeight);
				transparency = Math::interpolation::lerp(lf.a->spheres[j].transparency, lf.b->spheres[j].transparency, lf.lerpWeight);

				emmisive.x = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.x, lf.b->spheres[j].emissionColor.x, lf.lerpWeight);
				emmisive.y = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.y, lf.b->spheres[j].emissionColor.y, lf.lerpWeight);
				emmisive.z = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.z, lf.b->spheres[j].emissionColor.z, lf.lerpWeight);

				Sphere tempS(pos, rad, surfaceColour, reflection, transparency, emmisive);
				spheres.push_back(tempS);
			}

			assert(spheres.size() > 0);


			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			rendering::renderToFolderMultiThreadProfileable(frameFileName, outputFolder, spheres, m_framesToRender[i].frameNumber, &frd.start, &frd.end);
			
			// frd.end = std::chrono::steady_clock::now();
			frd.frameNumber = i;
			//m_frameRenderStats.push(frd);
			std::cout << "Rendered frame: " << m_framesToRender[i].frameNumber << std::endl;
			// g_FRDs.push_back(frd);
			g_FRDs[i] = frd;
		}



		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();



		rendering::finshRenderToFolderWithCleanUp(outputFolder, frameFileName, outputVideoFile, fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();

		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void clearForNextScene()
	{
		//m_toRender.clear();
		/*while (!m_frameRenderStats.empty())
		{
			m_frameRenderStats.pop();
		}*/
		g_FRDs.clear();
		delete[] m_framesToRender;
		// delete[] m_frameRenderDurations;
	}

private:
	// std::vector<Frame> m_toRender;
	// FrameRenderDuration * m_frameRenderDurations;
	LerpedFrame * m_framesToRender;
	unsigned int m_numFramesToRender;


	unsigned int m_nThreadsSupportedByPlatform;
	std::ostream & m_statsOutput;

	// std::priority_queue<FrameRenderDuration> m_frameRenderStats;

	

	void writeRenderStats(RenderDuration timeToRenderEverything, RenderDuration timeToCovertToVideo)
	{
		using namespace std::chrono;
		// rember m_frameRenderStats is a priory que, (the contents will be in the correct order)
		m_statsOutput << "_______________________________" << std::endl;
		// std::vector<FrameRenderDuration> toOut;

		/*
		while (!m_frameRenderStats.empty())
		{
			toOut.push_back(m_frameRenderStats.top());
			m_frameRenderStats.pop();
		}
		*/
		
		// std::sort(toOut.begin(), toOut.end());
		
		
		
		/*
		for (int i = 0; i < toOut.size(); i++)
		{

			// skip junk data
			steady_clock::duration frameRenterTime = toOut[i].end - toOut[i].start;
			milliseconds ms = duration_cast<milliseconds>(frameRenterTime);


			if (ms.count() > 0) // rendering the scene is highly unlikely to take less than 1ms
			{
				m_statsOutput << "Frame " << toOut[i].frameNumber << " took ";
				m_statsOutput << ms.count() << " milliseconds to render" << std::endl;
			}
			
		}
		*/

		std::sort(g_FRDs.begin(), g_FRDs.end());

		for (int i = 0; i < m_numFramesToRender; i++)
		{
			steady_clock::duration frameRenderTime = g_FRDs[i].end - g_FRDs[i].start;
			milliseconds ms = duration_cast<milliseconds>(frameRenderTime);
			if (ms.count() > 0)
			{
				m_statsOutput << "Frame " << g_FRDs[i].frameNumber << " took " << ms.count() << " milliseconds to render " << std::endl;
			}
		} 


		m_statsOutput << "---------------------------" << std::endl;
		steady_clock::duration toVidDur = timeToCovertToVideo.end - timeToCovertToVideo.start;
		milliseconds tvMS = duration_cast<milliseconds>(toVidDur);
		m_statsOutput << "Converting the frames to video took: " << tvMS.count() << " milliseconds" << std::endl;
		m_statsOutput << "---------------------------" << std::endl;
		steady_clock::duration sceneD = timeToRenderEverything.end - timeToRenderEverything.start;
		seconds s = duration_cast<seconds>(sceneD);
		m_statsOutput << "Rendering the entire scene took " << s.count() << " seconds" << std::endl;
	}
	
	void renderFrameRange(unsigned int start, unsigned int end, std::string frameFileName, std::string outputFolder)
	{
		for (unsigned int i = start; i < end; i++)
		{
			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			// rendering::renderToFolder(m_toRender[i].startOfFrameFileNameString, m_toRender[i].outPutFolder, m_toRender[i].frameData, m_toRender[i].frameNumber);
			// frd.end = std::chrono::steady_clock::now();

			std::vector<Sphere> s;

			LerpedFrame lf = m_framesToRender[i];
			for (auto j = 0; j < lf.a->spheres.size(); j++)
			{
				// the sphere needs:
				/*"POSITION" v3
				"RADIUS" f
				"SURFACE_COLOUR" v3
				"REFLECTION_VALUE" f
				"TRANSPARENCY_VALUE" f
				"EMISSIVE_COLOUR" v3*/
				Vec3f pos;
				float rad;
				Vec3f surfaceColour;
				float reflection;
				float transparency;
				Vec3f emmisive;
				pos.x = Math::interpolation::lerp(lf.a->spheres[j].center.x, lf.b->spheres[j].center.x, lf.lerpWeight);
				pos.y = Math::interpolation::lerp(lf.a->spheres[j].center.y, lf.b->spheres[j].center.y, lf.lerpWeight);
				pos.z = Math::interpolation::lerp(lf.a->spheres[j].center.z, lf.b->spheres[j].center.z, lf.lerpWeight);

				rad = Math::interpolation::lerp(lf.a->spheres[j].radius, lf.b->spheres[j].radius, lf.lerpWeight);

				surfaceColour.x = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.x, lf.b->spheres[j].surfaceColor.x, lf.lerpWeight);
				surfaceColour.y = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.y, lf.b->spheres[j].surfaceColor.y, lf.lerpWeight);
				surfaceColour.z = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.z, lf.b->spheres[j].surfaceColor.z, lf.lerpWeight);

				reflection = Math::interpolation::lerp(lf.a->spheres[j].reflection, lf.b->spheres[j].reflection, lf.lerpWeight);
				transparency = Math::interpolation::lerp(lf.a->spheres[j].transparency, lf.b->spheres[j].transparency, lf.lerpWeight);

				emmisive.x = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.x, lf.b->spheres[j].emissionColor.x, lf.lerpWeight);
				emmisive.y = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.y, lf.b->spheres[j].emissionColor.y, lf.lerpWeight);
				emmisive.z = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.z, lf.b->spheres[j].emissionColor.z, lf.lerpWeight);

				Sphere tempS(pos, rad, surfaceColour, reflection, transparency, emmisive);
				s.push_back(tempS);
			}

			assert(s.size() > 0); // pick up here, calculate the lerped spheres

			rendering::renderToFolderProfileable(frameFileName, outputFolder, s, m_framesToRender[i].frameNumber, &frd.start, &frd.end);

			frd.frameNumber = i;
			g_FRDs[i] = frd;
			// m_frameRenderStats.push(frd);
			std::cout << "Rendered frame: " << m_framesToRender[i].frameNumber << std::endl;
		}
	}

	__attribute__((noreturn))
	static void fiberStartFuncForMultiFrames(uint64_t onInitArgs, uint64_t onRunArgs)
	{
		FiberFrameRange * framesToRender = (FiberFrameRange *)onInitArgs;
		renderFramesStatic(framesToRender->lerpedFramesArray, framesToRender->rangeStart, framesToRender->rangeEnd, framesToRender->outputFolder, framesToRender->outputFileStart);
		sceFiberReturnToThread(0, &onRunArgs);
	}

	static void renderFramesStatic(LerpedFrame * lerpedFramesArray, unsigned int start, unsigned int end, std::string ouputFolder, std::string frameFileStart)
	{
		for (unsigned int i = start; i < end; i++)
		{
			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			// rendering::renderToFolder(frames[i].startOfFrameFileNameString, frames[i].outPutFolder, frames[i].frameData, frames[i].frameNumber);

			std::vector<Sphere> spheres;

			LerpedFrame lf = lerpedFramesArray[i];

			
			for (int j = 0; j < lf.a->spheres.size(); j++)
			{
				Vec3f pos;
				float rad;
				Vec3f surfaceColour;
				float reflection;
				float transparency;
				Vec3f emmisive;
				pos.x = Math::interpolation::lerp(lf.a->spheres[j].center.x, lf.b->spheres[j].center.x, lf.lerpWeight);
				pos.y = Math::interpolation::lerp(lf.a->spheres[j].center.y, lf.b->spheres[j].center.y, lf.lerpWeight);
				pos.z = Math::interpolation::lerp(lf.a->spheres[j].center.z, lf.b->spheres[j].center.z, lf.lerpWeight);

				rad = Math::interpolation::lerp(lf.a->spheres[j].radius, lf.b->spheres[j].radius, lf.lerpWeight);

				surfaceColour.x = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.x, lf.b->spheres[j].surfaceColor.x, lf.lerpWeight);
				surfaceColour.y = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.y, lf.b->spheres[j].surfaceColor.y, lf.lerpWeight);
				surfaceColour.z = Math::interpolation::lerp(lf.a->spheres[j].surfaceColor.z, lf.b->spheres[j].surfaceColor.z, lf.lerpWeight);

				reflection = Math::interpolation::lerp(lf.a->spheres[j].reflection, lf.b->spheres[j].reflection, lf.lerpWeight);
				transparency = Math::interpolation::lerp(lf.a->spheres[j].transparency, lf.b->spheres[j].transparency, lf.lerpWeight);

				emmisive.x = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.x, lf.b->spheres[j].emissionColor.x, lf.lerpWeight);
				emmisive.y = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.y, lf.b->spheres[j].emissionColor.y, lf.lerpWeight);
				emmisive.z = Math::interpolation::lerp(lf.a->spheres[j].emissionColor.z, lf.b->spheres[j].emissionColor.z, lf.lerpWeight);

				Sphere tempS(pos, rad, surfaceColour, reflection, transparency, emmisive);
				spheres.push_back(tempS);
			}

			assert(spheres.size() > 0);
			
			// rendering::renderToFolderProfileable(frames[i].startOfFrameFileNameString, frames[i].outPutFolder, frames[i].frameData, frames[i].frameNumber, &frd.start, &frd.end);
			rendering::renderToFolderProfileable(frameFileStart, ouputFolder, spheres, lf.frameNumber, &frd.start, &frd.end);
			// frd.end = std::chrono::steady_clock::now();
			// m_frameRenderStats.push(frd);
			frd.frameNumber = i;
			
			// g_FRDs.push_back(frd); // this could be an issue
			g_FRDs[i] = frd; // this could be an issue

			// m_frameRenderDurations[i] = frd;

			std::cout << "Rendered frame: " << lf.frameNumber << std::endl;
		}
	}

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
};



#endif