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
// Windows only
#include <algorithm>
#include <sstream>
#include <string.h>


// Josh files
#include "Vec3.h"
#include "MathDefines.h"

#include "Rendering.h"

#include "ExampleScenes.h"
#include "TestScenes.h"



//[comment]
// In the main function, we will create the scene which is composed of 5 spheres
// and 1 light (which is also a sphere). Then, once the scene description is complete
// we render that scene, by calling the render() function.
//[/comment]
int main(int argc, char **argv)
{
	// This sample only allows one choice per program execution. Feel free to improve upon this
	srand(13);

	// system("del \"*.ppm\""); // clear out any old images, bad idea will want to keep the files 

	//BasicRender();
	//SimpleShrinking();
	// ExampleScenes::SmoothScaling();

	// TestScenes::renderSceneForTime(10.0f, 24);
	
	// duration in seconds, frames per second, folder string, output video file string, start of frame file name
	
	// TestScenes::renderSceneForTimeSpecificOutput(120.0f, 30, "TestFrame", "TestVid.avi", "TestRenderFolder"); 

	TestScenes::renderSceneForTimeSpecificOutput(5.0f, 24, "TestFrame", "TestVid.avi", "TestRenderFolder");
	

	// ffmpeg to be called in the finaliseRender() in the rendering.h // once its coded
	// system("ffmpeg -y -i spheres%d.ppm outVideo.avi"); // -y for overide output video if there's a conflict
	// system("outVideo.avi"); // plays the video using the default player


	return 0;
}

