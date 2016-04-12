#ifndef _RENDER_MANAGER_H_
#define _RENDER_MANAGER_H_

#include "Rendering.h"
#include "Frame.h"

#include <thread>
#include <queue>
#include <chrono> // for determining the total rendering time
// this class is to use the functions in Rendering .h to handle the actual rendering, will be using threads to seperate drawing the frames accross 

#include <ostream> // for the output

#include <iostream> // for cout

struct RenderDuration
{
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
};

struct FrameRenderDuration : public RenderDuration
{
	unsigned int frameNumber;
	bool operator<(const FrameRenderDuration & other) const
	{
		return frameNumber < other.frameNumber;
	}
};

std::vector<FrameRenderDuration> g_FRDs;

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

	void addFrame(Frame frameToRender)
	{
		m_toRender.push_back(frameToRender);
	}

	void renderFramesSingleThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd)
	{
		std::sort(m_toRender.begin(), m_toRender.end());
		unsigned int nFramesToRender = m_toRender.size();
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();
		renderFrameRange(0, nFramesToRender);


		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();

		rendering::finshRenderToFolderWithCleanUp(m_toRender[0].outPutFolder, m_toRender[0].startOfFrameFileNameString, outputVideoFile, fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();

		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void renderFramesMultiThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd)
	{
		std::sort(m_toRender.begin(), m_toRender.end());

		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();

		unsigned int nFramesToRender = m_toRender.size();
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		m_statsOutput << "Num threads used: " << m_nThreadsSupportedByPlatform << std::endl;
		
		unsigned int nFramesToRenderPerThread = nFramesToRender / m_nThreadsSupportedByPlatform;

		unsigned int nFramesToRenderOnMainThread = 0;
		unsigned int frameIndexForMainThread = 0;
		
		std::vector<std::thread *> renderingThreads;


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

		renderFrameRange(frameIndexForMainThread, nFramesToRender);
		
		// re join the child threads
		for (short i = 0; i < renderingThreads.size(); i++)
		{
			if (renderingThreads[i]->joinable())
			{
				renderingThreads[i]->join();
				delete renderingThreads[i];
			}
			else
			{
				i--; // go back and check the same thread again
			}
		}

		renderingThreads.clear();

		for (int i = 0; i < g_FRDs.size(); i++)
		{
			m_frameRenderStats.push(g_FRDs[i]);
		}
		g_FRDs.clear();

		std::string outputFolder = m_toRender[0].outPutFolder;
		std::string outVidFile = outputVideoFile;



		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();

		rendering::finshRenderToFolderWithCleanUp(m_toRender[0].outPutFolder, m_toRender[0].startOfFrameFileNameString, outVidFile, fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();


		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void renderFramesMultiThreadPerFrame(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd)
	{
		std::sort(m_toRender.begin(), m_toRender.end());
		unsigned int nFramesToRender = m_toRender.size();
		m_statsOutput << "\n___START_RENDERING_INFORMATION___\n";
		RenderDuration timeToRenderEntireScene;
		timeToRenderEntireScene.start = std::chrono::steady_clock::now();
		
		
		
		// renderFrameRange(0, nFramesToRender);

		for (int i = 0; i < nFramesToRender; i++)
		{
			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			rendering::renderToFolderMultiThreadProfileable(m_toRender[i].startOfFrameFileNameString, m_toRender[i].outPutFolder, m_toRender[i].frameData, m_toRender[i].frameNumber, &frd.start, &frd.end);
			
			// frd.end = std::chrono::steady_clock::now();
			frd.frameNumber = i;
			m_frameRenderStats.push(frd);
			std::cout << "Rendered frame: " << m_toRender[i].frameNumber << std::endl;
		}



		RenderDuration timeToConvertToVideo;
		timeToConvertToVideo.start = std::chrono::steady_clock::now();



		rendering::finshRenderToFolderWithCleanUp(m_toRender[0].outPutFolder, m_toRender[0].startOfFrameFileNameString, outputVideoFile, fps, deleteFramesAtEnd);

		timeToConvertToVideo.end = std::chrono::steady_clock::now();

		timeToRenderEntireScene.end = std::chrono::steady_clock::now();
		writeRenderStats(timeToRenderEntireScene, timeToConvertToVideo);
	}

	void clearForNextScene()
	{
		m_toRender.clear();
		while (!m_frameRenderStats.empty())
		{
			m_frameRenderStats.pop();
		}
		g_FRDs.clear();
	}

private:
	std::vector<Frame> m_toRender;
	unsigned int m_nThreadsSupportedByPlatform;
	std::ostream & m_statsOutput;

	std::priority_queue<FrameRenderDuration> m_frameRenderStats;

	

	void writeRenderStats(RenderDuration timeToRenderEverything, RenderDuration timeToCovertToVideo)
	{
		using namespace std::chrono;
		// rember m_frameRenderStats is a priory que, (the contents will be in the correct order)
		m_statsOutput << "_______________________________" << std::endl;
		std::vector<FrameRenderDuration> toOut;

		while (!m_frameRenderStats.empty())
		{
			toOut.push_back(m_frameRenderStats.top());
			m_frameRenderStats.pop();
		}
		
		
		std::sort(toOut.begin(), toOut.end());
		
		
		

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

		m_statsOutput << "---------------------------" << std::endl;
		steady_clock::duration toVidDur = timeToCovertToVideo.end - timeToCovertToVideo.start;
		milliseconds tvMS = duration_cast<milliseconds>(toVidDur);
		m_statsOutput << "Converting the frames to video took: " << tvMS.count() << " milliseconds" << std::endl;
		m_statsOutput << "---------------------------" << std::endl;
		steady_clock::duration sceneD = timeToRenderEverything.end - timeToRenderEverything.start;
		seconds s = duration_cast<seconds>(sceneD);
		m_statsOutput << "Rendering the entire scene took " << s.count() << " seconds" << std::endl;
	}
	
	void renderFrameRange(unsigned int start, unsigned int end)
	{
		for (unsigned int i = start; i < end; i++)
		{
			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			// rendering::renderToFolder(m_toRender[i].startOfFrameFileNameString, m_toRender[i].outPutFolder, m_toRender[i].frameData, m_toRender[i].frameNumber);
			// frd.end = std::chrono::steady_clock::now();

			rendering::renderToFolderProfileable(m_toRender[i].startOfFrameFileNameString, m_toRender[i].outPutFolder, m_toRender[i].frameData, m_toRender[i].frameNumber, &frd.start, &frd.end);

			frd.frameNumber = i;
			m_frameRenderStats.push(frd);
			std::cout << "Rendered frame: " << m_toRender[i].frameNumber << std::endl;
		}
	}

	static void renderFramesStatic(std::vector<Frame> & frames, unsigned int start, unsigned int end)
	{
		for (unsigned int i = start; i < end; i++)
		{
			FrameRenderDuration frd;
			// frd.start = std::chrono::steady_clock::now();
			// rendering::renderToFolder(frames[i].startOfFrameFileNameString, frames[i].outPutFolder, frames[i].frameData, frames[i].frameNumber);
			rendering::renderToFolderProfileable(frames[i].startOfFrameFileNameString, frames[i].outPutFolder, frames[i].frameData, frames[i].frameNumber, &frd.start, &frd.end);
			// frd.end = std::chrono::steady_clock::now();
			// m_frameRenderStats.push(frd);
			frd.frameNumber = i;
			g_FRDs.push_back(frd); // this could be an issue

			std::cout << "Rendered frame: " << frames[i].frameNumber << std::endl;
		}
	}
};



#endif