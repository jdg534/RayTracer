#ifndef _FIOS2_WRAPPER_H_
#define _FIOS2_WRAPPER_H_

#include <fios2.h>
#include <fios2\fios2_types.h>

#include "PS4FolderPath.h"
#include <string>
#include <sstream>


// define a few global values (based off the PS4 sdk sample code)
#define MAX_PATH_LENGTH 256 // max, of 256 characters for a file path

#define MAX_DIRECTORY_HANDLES 1
#define MAX_FILE_HANDLES 16

int64_t g_operationStorage[SCE_FIOS_DIVIDE_ROUNDING_UP(SCE_FIOS_OP_STORAGE_SIZE(64, MAX_PATH_LENGTH), sizeof(int64_t))];
int64_t g_ChunkStorage[SCE_FIOS_DIVIDE_ROUNDING_UP(SCE_FIOS_CHUNK_STORAGE_SIZE(1024), sizeof(int64_t))]; // 1024 chunks
int64_t g_FileHandleStorage[SCE_FIOS_DIVIDE_ROUNDING_UP(SCE_FIOS_FH_STORAGE_SIZE(MAX_FILE_HANDLES, MAX_PATH_LENGTH), sizeof(int64_t))]; // max of 16 file handles
int64_t g_DirectoryHandleStorage[SCE_FIOS_DIVIDE_ROUNDING_UP(SCE_FIOS_DH_STORAGE_SIZE(MAX_DIRECTORY_HANDLES, MAX_PATH_LENGTH), sizeof(int64_t))]; // 1 directory handle

class fios2Wrapper
{
public:

	bool isFios2Initialised()
	{
		bool results;
		SceFiosParams params;
		
		results = sceFiosIsInitialized(&params);
		return results;
	}

	bool initFios2()
	{
		// see <PS4 dev docs/Reserence/System/Fios2 Library Reference/Functions/(function name here)>

		// the init code is based off the FIOS2 sample code!

		int initResults;
		SceFiosParams initParameters = SCE_FIOS_PARAMS_INITIALIZER; // following code based off the SDK simples
		initParameters.opStorage.pPtr = g_operationStorage;
		initParameters.opStorage.length = sizeof(g_operationStorage);
		initParameters.chunkStorage.pPtr = g_ChunkStorage;
		initParameters.chunkStorage.length = sizeof(g_ChunkStorage);
		initParameters.fhStorage.pPtr = g_FileHandleStorage;
		initParameters.fhStorage.length = sizeof(g_FileHandleStorage);
		initParameters.dhStorage.pPtr = g_DirectoryHandleStorage;
		initParameters.dhStorage.length = sizeof(g_DirectoryHandleStorage);
		
		initParameters.pathMax = MAX_PATH_LENGTH;
		
		initParameters.pVprintf = vprintf; // vprintf is in stdio.h
		initParameters.pMemcpy = memcpy; // memcpy is in string.h

		initResults = sceFiosInitialize(&initParameters);
		// initResults = sceFiosInitialize(SCE_FIOS_PARAMS_INITIALIZER);

		
		assert(initResults == SCE_OK);
		
		if (initResults == SCE_FIOS_OK)
		{
			printf("FIOS2 initialised successfully");
			return true;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_PTR)
		{
			printf("FIOS2 error, bad pointer");
			return false;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_ALIGNMENT)
		{
			printf("FIOS2 error, bad alignment");
			return false;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_SIZE)
		{
			printf("FIOS2 error, bad size");
			return false;
		}
		else
		{
			// the error code doesn't match the the posible return codes in the PS4 dev docs
			printf("FIOS2 error, unrecognised error code");
			return false;
		}
	}

	bool folderExists(std::string folderPath)
	{
		// see sceFiosDirectoryExists

		// asumes that fios has been initialised

		std::string actualPath = PS4_VISUAL_STUDIO_DIR;
		actualPath += folderPath;
		actualPath += "/";
		
		SceFiosOpAttr operationAttribs;
		operationAttribs.deadline = SCE_FIOS_TIME_EARLIEST;
		operationAttribs.pCallback = NULL;
		operationAttribs.pCallbackContext = NULL;
		operationAttribs.priority = 0; // negative for low priorty, position for high priorty
		operationAttribs.opflags = SCE_FIOS_OPFLAG_DONTFILLCACHE; // leaves RAM caches alone
		operationAttribs.userPtr = NULL;
		operationAttribs.userTag =  0;
		operationAttribs.pReserved = 0;

		SceFiosOp operationResults;

		bool exists = false; 
		// can just pass NULL for operationAttribs if needed
		// operationResults = sceFiosDirectoryExists(&operationAttribs, actualPath.c_str(), &exists);
		// operationResults = sceFiosDirectoryExistsSync(NULL, actualPath.c_str(), &exists);

		exists = sceFiosDirectoryExistsSync(NULL, actualPath.c_str());

		// assert(operationResults >= 0);

		return exists;
	}

	bool createFolder(std::string folderPath)
	{
		// see sceFiosDirectoryCreate

		// asumes that fios has been initialised
		std::string actualPath = PS4_VISUAL_STUDIO_DIR;
		actualPath += folderPath;

		actualPath += "/";

		SceFiosOpAttr operationAttribs;
		operationAttribs.deadline = SCE_FIOS_TIME_EARLIEST;
		operationAttribs.pCallback = NULL;
		operationAttribs.pCallbackContext = NULL;
		operationAttribs.priority = 0; // negative for low priorty, position for high priorty
		operationAttribs.opflags = SCE_FIOS_OPFLAG_DONTFILLCACHE; // leaves RAM caches alone
		operationAttribs.userPtr = NULL;
		operationAttribs.userTag = 0;
		operationAttribs.pReserved = 0;
		SceFiosOp operationResults;
		// operationResults = sceFiosDirectoryCreate(&operationAttribs, actualPath.c_str());
		// operationResults = sceFiosDirectoryCreate(NULL, actualPath.c_str());
		operationResults = sceFiosDirectoryCreateSync(NULL, actualPath.c_str());
		// assert(operationResults >= 0);
		if (operationResults >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void deleteFrameFileInFolder(unsigned int nFramesToDelete, std::string folderString, std::string startOfFileString)
	{
		// std::stringstream ss;
		std::string fileStrStart = PS4_VISUAL_STUDIO_DIR;
		fileStrStart += folderString + "/" + startOfFileString;

		std::string endOfFileString = ".ppm";

		for (int i = 0; i < nFramesToDelete; i++)
		{
			std::stringstream ss;
			ss << fileStrStart << i << endOfFileString;
			std::string fileStr = ss.str();
			int deleteResults = sceFiosDeleteSync(NULL, fileStr.c_str());
			if (deleteResults != SCE_FIOS_OK)
			{
				// can ignor file not found, figure the rest out
				if (deleteResults == SCE_FIOS_ERROR_BAD_PATH)
				{
					// couldn't find the file to delete (ignor it!!!)
				}
				else if (deleteResults == SCE_FIOS_ERROR_ACCESS)
				{
					
				}
				else
				{
					char breakPoint = '?';
				}
			}
		}
	}

	void shutdown()
	{
		sceFiosTerminate();
	}
private:

};


#endif