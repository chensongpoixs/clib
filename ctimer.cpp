/***********************************************************************************************
created: 		2019-12-12

author:			chensong

purpose:		ctimer
我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
//
//#include "ctimer.h"
//#include <boost/asio.hpp>
//namespace chen {
//	static int64 tick_factor() // multiplier to convert ticks
//											  //  to nanoseconds; -1 if unknown
//	{
//		static int64 tick_factor = 0;
//		//if (!tick_factor)
//		//{
//		//	if ((tick_factor = ::sysconf(_SC_CLK_TCK)) <= 0)
//		//		tick_factor = -1;
//		//	else
//		//	{
//		//		assert(tick_factor <= 1000000000LL); // logic doesn't handle large ticks
//		//		tick_factor = 1000000000LL / tick_factor;  // compute factor
//		//		if (!tick_factor)
//		//			tick_factor = -1;
//		//	}
//		//}
//		return tick_factor;
//	}
//	static void get_cpu_times(ccpu_times& current)
//	{//std::atto
//		std::chrono::duration<int64, std::nano> x(std::chrono::high_resolution_clock::now().time_since_epoch());
//		current.wall = x.count();
//
//		#if defined(_MSC_VER)
//		FILETIME creation, exit;
//		if (::GetProcessTimes(::GetCurrentProcess(), &creation, &exit,
//			(LPFILETIME)&current.system, (LPFILETIME)&current.user))
//		{
//			current.user *= 100;  // Windows uses 100 nanosecond ticks
//			current.system *= 100;
//		}
//		else
//		{
//			current.system = current.user = 0;
//		}
//		#elif defined(__GNUC__) ||  defined(__APPLE__)
//		tms tm;
//		clock_t c = ::times(&tm);
//		if (c == static_cast<clock_t>(-1)) // error
//		{
//			current.system = current.user = 0;
//		}
//		else
//		{
//			current.system = tm.tms_stime + tm.tms_cstime;
//			current.user = tm.tms_utime + tm.tms_cutime;
//			boost::int_least64_t factor;
//			if ((factor = tick_factor()) != -1)
//			{
//				current.user *= factor;
//				current.system *= factor;
//			}
//			else
//			{
//				current.user = current.system = boost::timer::nanosecond_type(-1);
//			}
//		}
//		#else
//		#pragma error "unknow platform!!!"
//		#endif
//
//	}
//
//
//	void ctimer::start()
//	{
//		m_stopped = false;
//		get_cpu_times(m_times);
//	}
//	void ctimer::stop()
//	{
//		if (m_stopped)
//			return;
//		m_stopped = true;
//
//		ccpu_times current;
//		get_cpu_times(current);
//		m_times.wall = (current.wall - m_times.wall);
//		m_times.user = (current.user - m_times.user);
//		m_times.system = (current.system - m_times.system);
//	}
//	void ctimer::resume()
//	{
//		if (m_stopped)
//		{
//			ccpu_times current(m_times);
//			start();
//			m_times.wall -= current.wall;
//			m_times.user -= current.user;
//			m_times.system -= current.system;
//		}
//	}
//	ccpu_times ctimer::elapsed() const
//	{
//		if (m_stopped)
//			return m_times;
//		ccpu_times current;
//		get_cpu_times(current);
//		current.wall -= m_times.wall;
//		current.user -= m_times.user;
//		current.system -= m_times.system;
//		return current;
//	}
//
//	void ctimer::show_data(ccpu_times& times) const
//	{
//		const double sec = 1000000000.0L;
//		long long total = times.system + times.user;
//		double wall_sec = static_cast<double>(times.wall) / sec;
//		double total_sec = static_cast<double>(total) / sec;
//		double  p_sec = 0;
//		if (wall_sec > 0.001L && total_sec > 0.001L)
//		{
//			p_sec =(total_sec / wall_sec) * 100.0;
//		}
//		/**
//		* ��ѧ������
//		* stata �� ������ 1.333e+09 ����ʲô��˼
//		* ��������̫���ˣ��ÿ�ѧ��������ʾ��
//		* ����  �����ݴη��������1.333e-09����˵��������0�� ��1.33*10�ĸ�9�η�
//		* ��1330000000��Ҳ���� 1.33 * (10^9)
//		*/
//		printf("[w = %.2g][u = %.2g][s = %.2g][t = %.2g][p = %.2g]\n", wall_sec, static_cast<double>(times.user) / sec, static_cast<double>(times.system) / sec, total_sec, p_sec);
//	}
//
//
//
//	ctimer::~ctimer()
//	{
//		if (!m_stopped)
//		{
//			stop();  // the sooner we stop(), the better
//			show_data(elapsed());
//		}
//	}
//} // namespace chen