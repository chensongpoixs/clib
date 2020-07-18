/********************************************************************
created: 		2019-05-27

author:			chensong

purpose:		cnet_msg_statistic
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
*********************************************************************/

#include "cnet_msg_statistic.h"
#include "clog.h"
#include <sstream>
#include "ctime_api.h"
namespace chen {
	cnet_msg_statistic::cnet_msg_statistic()
		: m_log_ptr(NULL)
		, m_msgs(NULL)
		, m_count(0)
		, m_interval(0)
		, m_cur(0)
	{
	}
	cnet_msg_statistic::~cnet_msg_statistic()
	{
	}
	bool cnet_msg_statistic::init(unsigned int max_msg_id, unsigned int show_interval)
	{
		if (m_msgs)
		{
			return true;
		}

		m_msgs = new cstatistic[max_msg_id];
		if (!m_msgs)
		{
			ERROR_LOG( " cstatistic failed");
			return false;
		}

		m_count = max_msg_id;

		m_interval = show_interval;

		if (m_interval > 0 && !_init_log())
		{
			return false;
		}

		return true;
	}
	void cnet_msg_statistic::destroy()
	{
		write_out();
		if (m_log_ptr)
		{
			m_log_ptr->destroy();
			delete m_log_ptr;
			m_log_ptr = nullptr;
		}
		if (m_msgs)
		{
			delete[]  m_msgs;
			m_msgs = nullptr;
		}
	}

	void cnet_msg_statistic::update(unsigned int elapse)
	{
		if (m_interval)
		{
			m_cur += elapse;
			if (m_cur >= m_interval)
			{
				m_cur -= m_interval;
				write_out();
			}
		}
	}

	bool cnet_msg_statistic::register_msg(unsigned short msg_id, const char * msg_name)
	{
		if (!m_msgs)
		{
			ERROR_LOG( "not init");
			return false;
		}
		if (_invalid_msg(msg_id))
		{
			ERROR_LOG("invalid msg id = %d, msg_name= %s " , msg_id, msg_name);
			return false;
		}

		if (m_msgs[msg_id].msg_name != NULL)
		{
			ERROR_LOG("msg = %s, has been register ", msg_name);
			return false;
		}

		m_msgs[msg_id].msg_name = msg_name;
		return true;
	}

	void cnet_msg_statistic::count_msg(unsigned short msg_id, unsigned int msg_size)
	{
		if (!m_msgs)
		{
			return;
		}
		if (!_invalid_msg(msg_id))
		{
			++m_msgs[msg_id].msgCount;
			m_msgs[msg_id].msgSize += msg_size;
		}
	}

	const char* cnet_msg_statistic::get_msg_name(unsigned short msg_id) const
	{
		if (m_msgs && !_invalid_msg(msg_id))
		{
			return m_msgs[msg_id].msg_name;
		}
		return NULL;
	}

	void cnet_msg_statistic::write_out()
	{
		if (!m_msgs)
		{
			return;
		}

		std::stringstream ss;
		{
			//time
			char buf[ASCII_DATETIME_LEN];
		//	ctime_api::datetime_format(buf, '-', ' ', ':');
			ss << buf << '\n';
		}

		ss << "msg id, msg count, msg size\n";
		bool flag = false;
		unsigned long long msgCount = 0;
		unsigned long long MsgSize = 0;
		for (unsigned int i = 0; i < m_count; ++i)
		{
			cstatistic& data = m_msgs[i];
			if (data.msgCount == 0)
			{
				continue;
			}
			flag = true;
			msgCount = data.msgCount.exchange(0);
			MsgSize = data.msgSize.exchange(0);
			if (data.msg_name)
			{
				ss << data.msg_name;
			}
			else
			{
				ss << i;
			}
			ss << ',' << msgCount << ',' << MsgSize << '\n';

		}
		if (flag)
		{
			ss << '\n';
			if (!m_log_ptr && !_init_log())
			{
				return;
			}
			std::string str(ss.str());
			m_log_ptr->append_fix(ELogLevel_None, str.data(), static_cast<unsigned int>(str.length()));
		}
	}
	bool cnet_msg_statistic::_invalid_msg(unsigned short msg_id) const
	{
		return  (msg_id < 0 || msg_id >= m_count);
	}
	bool cnet_msg_statistic::_init_log()
	{
		m_log_ptr = new casync_log();
		if (!m_log_ptr)
		{
			return false;
		}
		/*std::string filename (clog::get_fullname());
		{
			std::string::size_type n = filename.find_last_of(".\\/");
			if (n != std::string::npos && filename[n] == '.')
			{
				filename.erase(filename.begin() + n, filename.end());
			}
		}
		filename.append(".csv");

		return m_log->init(filename, false, false);*/
		return true;
	}
} //namespace chen