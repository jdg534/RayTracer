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


// Josh files
#include "Vec3.h"
#include "MathDefines.h"

#include "Rendering.h"


#include "Scenes.h"


std::vector<std::string> parseIndexFile(std::string indexFile)
{
	std::ifstream in(indexFile);
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

	std::vector<std::string> scenes = parseIndexFile("index.txt");

	//TestScenes::renderKeyFrameScene("keyFrameScene.txt");
	for (auto i = 0; i < scenes.size(); i++)
	{
		Scenes::renderKeyFrameScene(scenes[i]);
	}
	
	return 0;
}

