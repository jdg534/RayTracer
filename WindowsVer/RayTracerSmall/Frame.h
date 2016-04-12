#ifndef _FRAME_H_
#define _FRAME_H_


// the frame struct is to store everything needed to render a frame, (RenderManager class to store a que of frame, that will be rendered by seperate threads)

#include <string>
#include <vector>

#include "Sphere.h"

struct Frame
{
	unsigned int frameNumber;
	std::string outPutFolder;
	std::string startOfFrameFileNameString;
	std::vector<Sphere> frameData;

	bool operator<(const Frame & other) const
	{
		return frameNumber < other.frameNumber;
	}
};

#endif