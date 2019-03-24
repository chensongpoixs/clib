#ifndef _C_NET_TYPE_H
#define _C_NET_TYPE_H

namespace chen
{
	typedef signed char		int8;
	typedef unsigned char	uint8;

	typedef signed short	int16;
	typedef unsigned short	uint16;

	typedef signed int		int32;
	typedef unsigned int	uint32;

#if defined(_MSC_VER)

	typedef signed long long	int64;
	typedef unsigned long long	uint64;

#elif defined(__GNUC__)

	typedef signed long		int64;
	typedef unsigned long	uint64;
#else
#pragma error "unknow platform!!!"

#endif

}

#endif //_C_NET_TYPE_H
