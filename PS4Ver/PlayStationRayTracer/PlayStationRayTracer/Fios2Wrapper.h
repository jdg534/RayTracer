#ifndef _FIOS2_WRAPPER_H_
#define _FIOS2_WRAPPER_H_


#include <string>





class fios2Wrapper
{
public:

	bool isFios2Initialised();

	bool initFios2();

	bool folderExists(std::string folderPath);

	bool createFolder(std::string folderPath);

	void deleteFrameFileInFolder(unsigned int nFramesToDelete, std::string folderString, std::string startOfFileString);

	void shutdown();

};


#endif