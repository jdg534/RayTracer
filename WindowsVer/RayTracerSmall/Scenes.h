#ifndef _TEST_SCENES_H_
#define _TEST_SCENES_H_

//#include "Rendering.h"

//#include "RenderManager.h"

#include "KeyFrame.h"
// #include "Frame.h"

// #include <fstream>
/*
#ifdef _WIN32
#include <Windows.h>
#endif
*/

namespace Scenes
{
	bool folderExists(std::string folderStr);

	void createFolder(std::string folderName);

	void fillKeyFrameWithMostRecentSphereStates(KeyFrame & kf, std::vector<Sphere> spheres);
	
	void assignKeyFramesToLerpBetween(float timePoint, KeyFrame & kf1, KeyFrame & kf2, std::vector<KeyFrame> & keyFrames);

	float weightValueForTimePoints(float min, float max, float point);

	void renderKeyFrameScene(std::string sceneFile);

};



// side note compiler optimisation, only works on Relese Builds

#endif