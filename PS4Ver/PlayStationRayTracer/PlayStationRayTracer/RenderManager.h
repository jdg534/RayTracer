#ifndef _RENDER_MANAGER_H_
#define _RENDER_MANAGER_H_

#include "RenderingStructs.h"
#include "KeyFrame.h"

#include <string>
#include <ostream>
#include <_types.h>



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

	void makeRoomForFrames(unsigned int nFrames);

	void addLerpFrame(LerpedFrame lf);

	void renderFramesSingleThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName);

	void renderFramesMultiThread(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName);

	void renderFramesMultiThreadPerFrame(std::string outputVideoFile, unsigned int fps, bool deleteFramesAtEnd, std::string outputFolder, std::string frameFileName);

	void clearForNextScene();

private:
	// std::vector<Frame> m_toRender;
	// FrameRenderDuration * m_frameRenderDurations;
	LerpedFrame * m_framesToRender;
	unsigned int m_numFramesToRender;


	unsigned int m_nThreadsSupportedByPlatform;
	std::ostream & m_statsOutput;

	// std::priority_queue<FrameRenderDuration> m_frameRenderStats;

	

	void writeRenderStats(RenderDuration timeToRenderEverything, RenderDuration timeToCovertToVideo);
	
	void renderFrameRange(unsigned int start, unsigned int end, std::string frameFileName, std::string outputFolder);

	__attribute__((noreturn))
	static void fiberStartFuncForMultiFrames(uint64_t onInitArgs, uint64_t onRunArgs);

	static void renderFramesStatic(LerpedFrame * lerpedFramesArray, unsigned int start, unsigned int end, std::string ouputFolder, std::string frameFileStart);

	void predictNumSecondsToFinshRendering(unsigned int msToRenderCurrentFrame, unsigned int numFramesLeftToRender);
};



#endif