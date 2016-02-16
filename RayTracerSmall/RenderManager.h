#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include "Rendering.h"
#include "Frame.h"

#include <thread>
#include <queue>
#include <chrono> // for determining the total rendering time
// this class is to use the functions in Rendering .h to handle the actual rendering, will be using threads to seperate drawing the frames accross 

#include <ostream> // for the output

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
		m_threads.resize(m_nThreadsSupportedByPlatform);
	}

	void addFrame(Frame frameToRender)
	{

	}

	void renderFrames() // call this once added all of the frames
	{

	}

private:
	std::priority_queue<Frame> m_toRender;
	unsigned int m_nThreadsSupportedByPlatform;
	std::vector<std::thread> m_threads;
	std::ostream & m_statsOutput;
};



#endif