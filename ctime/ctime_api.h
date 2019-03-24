/********************************************************************
	created:	2019-03-24

	author:		chensong
	
	purpose:	time tools 

*********************************************************************/

#ifndef _C_TIME_API_H
#define _C_TIME_API_H
#include "ctime_const.h"
#include <ctime>
namespace chen 
{
	
	namespace ctime_base_api
	{
		void set_time_zone(int value);
		void set_time_adjust(int value);

		time_t get_gmt();

		void time_t_to_tm(time_t time, tm& out);
		tm time_t_to_tm(time_t time);

		void get_tm(tm& out);
		tm get_tm();

		time_t tm_to_time_t(const tm& _tm);
	}  // namespace ctime_base_api
}  // namespace chen 

#endif //_C_TIME_API_H