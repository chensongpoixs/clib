/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	字符串处理工具类

*********************************************************************/
#include "cdigit2str.h"
#include <algorithm>
#include <cstdio>

#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	template<typename T>
	int digit2str_dec(char* buf, int buf_size, T value)
	{
		T i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}


	template<>
	int digit2str_dec<float>(char* buf, int buf_size, float value)
	{
		if (value < 0)
		{
			if (buf_size < 9)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		else
		{
			if (buf_size < 8)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		
		return sprintf(buf, "%.2g", value);
	}

	template<>
	int digit2str_dec<double>(char* buf, int buf_size, double value)
	{
		if (value < 0)
		{
			if (buf_size < 9)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		else
		{
			if (buf_size < 8)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		return sprintf(buf, "%.2g", value);
	}

	
} // namespace chen