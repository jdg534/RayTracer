#ifndef _FIOS2_WRAPPER_H_
#define _FIOS2_WRAPPER_H_

#include <fios2.h>
#include <fios2\fios2_types.h>

#include "PS4FolderPath.h"
#include <string>




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

		int initResults;
		SceFiosParams * initParams = new SceFiosParams;// SCE_FIOS_PARAMS_INITIALIZER;
		*initParams = SCE_FIOS_PARAMS_INITIALIZER;
		/*
		initParams.reserved1 = 0;
		// there is no reserved2 parameters
		initParams.reserved3 = 0;
		initParams.reserved4 = 0;
		initParams.reserved5 = 0;
		initParams.reserved6 = 0;
		initParams.reserved7 = 0;
		*/

		initParams->initialized = 1;
		initParams->paramsSize = sizeof(SceFiosParams);
		initParams->pathMax = 0;
		initParams->reserved1 = 0;
		initParams->ioThreadCount = SCE_FIOS_IO_THREAD_COUNT_MIN;
		initParams->threadsPerScheduler = SCE_FIOS_SCHEDULER_THREAD_COUNT_DEFAULT;
		initParams->extraFlags = initParams->extraFlag1 = 0;
		initParams->maxChunk = SCE_FIOS_CHUNK_DEFAULT;
		initParams->maxDecompressorThreadCount = SCE_FIOS_DECOMPRESSOR_THREAD_COUNT_DEFAULT;
		initParams->schedulerAlgorithm = SCE_FIOS_IO_SCHED_DEADLINE;
		initParams->reserved3 = 0;
		initParams->reserved4 = 0;
		initParams->reserved5 = 0;
		initParams->reserved6 = 0;

		
		initParams->opStorage;
		initParams->fhStorage;
		initParams->dhStorage;
		initParams->chunkStorage;
		initParams->pVprintf = NULL;
		initParams->pMemcpy = NULL;
		initParams->reserved7 = 0;
		initParams->threadPriority; // array of three ints
		initParams->threadAffinity; // three ints array
		initParams->threadStackSize; // three int array

		initResults = sceFiosInitialize(initParams);
		// initResults = sceFiosInitialize(SCE_FIOS_PARAMS_INITIALIZER);

		// assert(initResults == SCE_FIOS_OK);
		
		
		delete initParams; // stored by fios2?, (not needed?)
		
		
		if (initResults == SCE_FIOS_OK)
		{
			return true;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_PTR)
		{
			return false;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_ALIGNMENT)
		{
			return false;
		}
		else if (initResults == SCE_FIOS_ERROR_BAD_SIZE)
		{
			return false;
		}
		else
		{
			// the error code doesn't match the the posible return codes in the PS4 dev docs
			return false;
		}
		
		if (initResults == SCE_FIOS_OK)
		{
			return true;
		}
		else
		{
			// the assert should prevent this code being reached
			return false;
		}
	}

	bool folderExists(std::string folderPath)
	{
		// see sceFiosDirectoryExists

		// asumes that fios has been initialised

		std::string actualPath = PS4_VISUAL_STUDIO_DIR;
		actualPath += folderPath;
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

		bool exists; 
		operationResults = sceFiosDirectoryExists(&operationAttribs, actualPath.c_str(), &exists);

		return exists;
	}

	bool createFolder(std::string folderPath)
	{
		// see sceFiosDirectoryCreate

		// asumes that fios has been initialised
		std::string actualPath = PS4_VISUAL_STUDIO_DIR;
		actualPath += folderPath;

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
		operationResults = sceFiosDirectoryCreate(&operationAttribs, actualPath.c_str());
	}
private:

};


#endif