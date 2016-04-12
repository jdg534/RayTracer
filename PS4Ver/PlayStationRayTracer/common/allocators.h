/* SCE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 03.008.041
* Copyright (C) 2015 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _SCE_GNM_TOOLKIT_ALLOCATORS_H_
#define _SCE_GNM_TOOLKIT_ALLOCATORS_H_

#include <gnm.h>
#include "allocator.h"

namespace sce
{
	namespace Gnmx
	{
		namespace Toolkit
		{
			class Allocators
			{
				static IAllocator getNullIAllocator() { IAllocator result = {0, 0, 0}; return result; }
			public:
				Allocators(IAllocator onion = getNullIAllocator(), IAllocator garlic = getNullIAllocator(), Gnm::OwnerHandle owner = Gnm::kInvalidOwnerHandle)
				: m_onion(onion)
				, m_garlic(garlic)
				, m_owner(owner)
				{
				}
				IAllocator m_onion;
				IAllocator m_garlic;
				Gnm::OwnerHandle m_owner;
			};
		}
	}
}

#endif /* _SCE_GNM_TOOLKIT_ALLOCATOR_H_ */
