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

#include "RenderingStructs.h"



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


	void init();

	void addFrame(Frame frameToRender);

	void renderFramesSingleThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd);

	void renderFramesMultiThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd);

	void renderFramesMultiThreadPerFrame(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd);
	
	void clearForNextScene();

private:
	std::vector<Frame> m_toRender;
	unsigned int m_nThreadsSupportedByPlatform;
	std::ostream & m_statsOutput;

	std::priority_queue<FrameRenderDuration> m_frameRenderStats;

	

	void writeRenderStats(RenderDuration timeToRenderEverything, RenderDuration timeToCovertToVideo);
	
	void renderFrameRange(unsigned int start, unsigned int end);

	static void renderFramesStatic(std::vector<Frame> & frames, unsigned int start, unsigned int end);
};



#endif