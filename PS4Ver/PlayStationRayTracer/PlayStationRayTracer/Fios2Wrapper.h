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
		SceFiosParams params = SCE_FIOS_PARAMS_INITIALIZER;
		results = sceFiosIsInitialized(&params);
		return results;
	}

	bool initFios2()
	{
		// see <PS4 dev docs/Reserence/System/Fios2 Library Reference/Functions/(function name here)>

		int initResults;
		SceFiosParams initParams = SCE_FIOS_PARAMS_INITIALIZER;
		
		/*
		initParams.reserved1 = 0;
		// there is no reserved2 parameters
		initParams.reserved3 = 0;
		initParams.reserved4 = 0;
		initParams.reserved5 = 0;
		initParams.reserved6 = 0;
		initParams.reserved7 = 0;
		*/


		

		initResults = sceFiosInitialize(&initParams);

		assert(initResults == SCE_FIOS_OK);

		
		/*
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
		*/
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