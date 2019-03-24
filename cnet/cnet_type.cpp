#include "cnet_type.h"
#include <cassert>

namespace chen
{
	inline void check_net_type_size()
	{
		static_assert(sizeof(int8) == 1, "sizeof(int8) != 1");
		static_assert(sizeof(uint8) == 1, "sizeof(uint8) != 1");
		static_assert(sizeof(int16) == 2, "sizeof(int16) != 2");
		static_assert(sizeof(uint16) == 2, "sizeof(uint16) != 2");
		static_assert(sizeof(int32) == 4, "sizeof(int32) != 4");
		static_assert(sizeof(uint32) == 4, "sizeof(uint32) != 4");
		static_assert(sizeof(int64) == 8, "sizeof(int64) != 8");
		static_assert(sizeof(uint64) == 8, "sizeof(uint64) != 8");
	}	
}
