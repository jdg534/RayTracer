// [header]
// A very basic raytracer example.
// [/header]
// [compile]
// c++ -o raytracer -O3 -Wall raytracer.cpp
// [/compile]
// [ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// [/ignore]
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

#include <fstream>

// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>

// PS4 only
#include <fios2.h>
#include <fios2\fios2_types.h>
#include <fios2\fios2_api.h>

#include "PS4FolderPath.h"
#include "Fios2Wrapper.h"

#include <sce_fiber.h>

#include <libsysmodule.h>

// Josh files
#include "Vec3.h"
#include "MathDefines.h"

#include "Rendering.h"

// #include "ExampleScenes.h"
#include "Scenes.h"


// simplerfied fiber usage
#include "FibersAssist.h"


void initFibers()
{
	// this function is for playing around with fibers
	// SceFiber testFiber;


	int fiberInitResults = SCE_OK;
	
	fiberInitResults = sceSysmoduleLoadModule(SCE_SYSMODULE_FIBER);


	// = sceFiberStartContextSizeCheck(0);

	assert(fiberInitResults == SCE_OK);

	return; 
}

std::vector<std::string> parseIndexFile(std::string indexFile)
{
	std::string actualFilePath = PS4_VISUAL_STUDIO_DIR;
	actualFilePath += indexFile;
	std::ifstream in(actualFilePath);
	std::vector<std::string> files;
	if (in.good())
	{
		std::string f = "";
		while (!in.eof())
		{
			in >> f;
			files.push_back(f);
		}
	}
	in.close();
	return files;
}

int main(int argc, char **argv)
{
	
	srand(13);

	// system("del \"*.ppm\""); // is the windows way, need to figure out the ORBIS way

	initFibers();
	


	
	// writeTestOutputFile();
	std::vector<std::string> scenes = parseIndexFile("index.txt");

	// init fios2, via the wrapper class
	fios2Wrapper wrapper;

	if (!wrapper.isFios2Initialised())
	{
		wrapper.initFios2();
	}

	// test test deleteing with FIOS2
	// wrapper.deleteFrameFileInFolder(288,"1_SINGLE_THREAD", "KFO_FRAME_");


	//TestScenes::renderKeyFrameScene("keyFrameScene.txt");
	for (auto i = 0; i < scenes.size(); i++)
	{
		Scenes::renderKeyFrameScene(scenes[i]);
	}
	

	// ffmpeg to be called in the finaliseRender() in the rendering.h // once its coded
	// system("ffmpeg -y -i spheres%d.ppm outVideo.avi"); // -y for overide output video if there's a conflict
	// system("outVideo.avi"); // plays the video using the default player

	// unload the fibers

	sceSysmoduleUnloadModule(SCE_SYSMODULE_FIBER);


	return 0;
}

