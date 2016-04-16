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


// Josh files
#include "Vec3.h"
#include "MathDefines.h"

#include "Rendering.h"

#include "ExampleScenes.h"
#include "TestScenes.h"



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

/*
bool initFIOS2()
{
	SceFiosParams fiosInitParameters;
	fiosInitParameters.
}
*/

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

	
	writeTestOutputFile();
	std::vector<std::string> scenes = parseIndexFile("index.txt");

	// init fios2, via the wrapper class
	fios2Wrapper wrapper;

	if (!wrapper.isFios2Initialised())
	{
		wrapper.initFios2();
	}


	//TestScenes::renderKeyFrameScene("keyFrameScene.txt");
	for (auto i = 0; i < scenes.size(); i++)
	{
		TestScenes::renderKeyFrameScene(scenes[i]);
	}
	

	// ffmpeg to be called in the finaliseRender() in the rendering.h // once its coded
	// system("ffmpeg -y -i spheres%d.ppm outVideo.avi"); // -y for overide output video if there's a conflict
	// system("outVideo.avi"); // plays the video using the default player


	return 0;
}

