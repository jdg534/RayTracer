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

#include "ExampleScenes.h"
#include "TestScenes.h"


// simplerfied fiber usage
#include "FibersAssist.h"



#define TEST_FIBER_CONTEXT_SIZE 2 * 1024

char testFiberContextBuffer[FIBER_CONTEXT_BUFFER_SIZE] __attribute__((aligned(SCE_FIBER_CONTEXT_ALIGNMENT)));

struct TestConuntRange
{
	int start, end;
};


__attribute__((noreturn))
void fiberCount(uint64_t startArg, uint64_t endArg)
{
	for (int i = 0; i < 100; i++)
	{
		std::cout << i << std::endl;
	}

	sceFiberReturnToThread(0, &endArg);
}


__attribute__((noreturn))
void fiberCountStart(uint64_t startArg, uint64_t endArg)
{
	int * start = (int *)startArg;

	for (int i = *start; i < 100; i++)
	{
		std::cout << i << std::endl;
	}

	sceFiberReturnToThread(0, &endArg);
}

__attribute__((noreturn))
void fiberCountRange(uint64_t initArgs, uint64_t onRunArgs)
{
	TestConuntRange * range = (TestConuntRange *)initArgs;
	for (int i = range->start; i < range->end; i++)
	{
		std::cout << i << std::endl;
	}

	sceFiberReturnToThread(0, &onRunArgs);
}

void initFibers()
{
	// this function is for playing around with fibers
	SceFiber testFiber;


	int fiberInitResults = SCE_OK;
	
	fiberInitResults = sceSysmoduleLoadModule(SCE_SYSMODULE_FIBER);


	// = sceFiberStartContextSizeCheck(0);

	assert(fiberInitResults == SCE_OK);


	return; // rest of this function works as intended
	
	
	// not yet, need to test passing parameters to a fiber

	// run a test fiber count 0 to 99
	// SceFiber testFiber;

	int startingIndex = 27;

	int cbsz = FIBER_CONTEXT_BUFFER_SIZE;

	int cbSizeMod16 = FIBER_CONTEXT_BUFFER_SIZE % 16;

	assert(cbSizeMod16 == 0);

	// fiberInitResults = sceFiberInitialize(&testFiber, "testFiber", fiberCount, (uint64_t)&startingIndex, (void *)testFiberContextBuffer, FIBER_CONTEXT_BUFFER_SIZE, NULL);
	// fiberInitResults = sceFiberInitialize(&testFiber, "testFiber", fiberCountStart, (uint64_t)&startingIndex, (void *)testFiberContextBuffer, FIBER_CONTEXT_BUFFER_SIZE, NULL);

	TestConuntRange tcr;
	tcr.start = -6254;
	tcr.end = 9001;

	fiberInitResults = sceFiberInitialize(&testFiber, "testFiber", fiberCountRange, (uint64_t)&tcr, (void *)testFiberContextBuffer, FIBER_CONTEXT_BUFFER_SIZE, NULL);

	char e;

	if (fiberInitResults == SCE_FIBER_ERROR_NULL)
	{
		e = '1';
	}
	else if (fiberInitResults == SCE_FIBER_ERROR_ALIGNMENT)
	{
		e = '2';
	}
	else if (fiberInitResults == SCE_FIBER_ERROR_RANGE)
	{
		e = '3';
	}
	else if (fiberInitResults == SCE_FIBER_ERROR_INVALID)
	{
		e = '4';
	}

	assert(fiberInitResults == SCE_OK);

	uint64_t fiberReturnArgs = 0;

	// now run the fiber
	fiberInitResults = sceFiberRun(&testFiber, 0, &fiberReturnArgs);
	assert(fiberInitResults == SCE_OK);

	bool fiberCleanedUp = false;
	while (!fiberCleanedUp)
	{
		fiberInitResults = sceFiberFinalize(&testFiber);
		if (fiberInitResults == SCE_OK)
		{
			fiberCleanedUp = true;
		}
		else if (fiberInitResults == SCE_FIBER_ERROR_STATE)
		{
			// it's still running
		}
		else
		{
			// something went wrong
		}
	}
	// fiberInitResults = sceFiberInitialize(&testFiber, "TEST_FIBER", fiberCount, 0,  )
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

void writeTestOutputFile()
{
	std::string filePath = PS4_VISUAL_STUDIO_DIR;
	filePath += "outTest.txt";
	std::ofstream out(filePath);
	out.clear();
	out << "this is a test to see the output folder for the PS4 Dev kit.";
	out.close();

}

int main(int argc, char **argv)
{
	
	srand(13);

	// system("del \"*.ppm\""); // is the windows way, need to figure out the ORBIS way

	initFibers();
	


	
	writeTestOutputFile();
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
		TestScenes::renderKeyFrameScene(scenes[i]);
	}
	

	// ffmpeg to be called in the finaliseRender() in the rendering.h // once its coded
	// system("ffmpeg -y -i spheres%d.ppm outVideo.avi"); // -y for overide output video if there's a conflict
	// system("outVideo.avi"); // plays the video using the default player

	// unload the fibers

	sceSysmoduleUnloadModule(SCE_SYSMODULE_FIBER);


	return 0;
}

