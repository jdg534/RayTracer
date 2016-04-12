#ifndef _KEY_FRAME_H_
#define _KEY_FRAME_H_

#include "Sphere.h"
#include <vector>

struct KeyFrame
{
	float timePoint;
	std::vector<Sphere> spheres;

	bool operator<(const KeyFrame & a)
	{
		return timePoint < a.timePoint;
	}
};

#endif