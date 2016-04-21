#ifndef _RENDERING_STRUCTS_H_
#define _RENDERING_STRUCTS_H_

#include <chrono>

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

#endif