/********************************************************************
	created:	2019-03-24

	author:		chensong
	
	purpose:	time tools 

*********************************************************************/

#include "ctime_api.h"
#include <cstdio>
#include <chrono>
#include <iostream>
#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen 
{
	namespace ctime_base_api
	{
		// 时区
		static time_t g_time_zone = 0;

		static long long g_time_adjust = 0;

		static long long g_time_offset = 0;

		struct ctime_init
		{
			ctime_init()
			{
				time_t now = time(0); // UTC
				struct tm *ptmgm = gmtime(&now); // further convert to GMT presuming now in local
				time_t gmnow = mktime(ptmgm);
				g_time_zone = now - gmnow;
				if (ptmgm->tm_isdst > 0) {
					g_time_zone = g_time_zone - 60 * 60;
				}
				g_time_offset = g_time_zone;
				std::cout << "timezone=" << g_time_zone / ETC_Hour << std::endl;
			}
		};

		//call before main
		static const ctime_init time_init;

		void set_time_zone(int value)
		{
			if (value < -12 || value > 12) return;
			g_time_zone = value * ETC_Hour;
			g_time_offset = g_time_adjust + g_time_zone;
		}

		void set_time_adjust(int value)
		{
			g_time_adjust = value;
			g_time_offset = g_time_adjust + g_time_zone;
		}		
		

		time_t get_gmt()
		{
			return (::time(NULL) + g_time_adjust);
		}

		void time_t_to_tm(time_t time, tm& out)
		{
			time += g_time_offset;
#if defined(_MSC_VER)
			::gmtime_s(&out, &time);
#elif defined(__GNUC__)
			::gmtime_r(&time, &out);
#else
#			pragma error "Unknown Platform Not Supported. Abort! Abort!"
#endif // 
		}

		tm time_t_to_tm(time_t time)
		{
			tm out;
			time_t_to_tm(time, out);
			return out;
		}

		void get_tm(tm& out)
		{	
			time_t_to_tm(get_gmt(), out);
		}

		tm get_tm()
		{
			tm out;
			get_tm(out);
			return out;
		}
	}
	
}