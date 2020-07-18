/***********************************************************************************************
				created: 		2019-05-15

				author:			chensong

				purpose:		http_client
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

#include "chttp_client.h"
#include "clog.h"
#include "chttp_session.h"

namespace chen {
	chttp_client::chttp_client() : m_work(m_io_service)
		, m_is_working(false)
		, m_stop(true)
		, m_max_session(0)
		, m_msg_handler(NULL)
		, m_send_buf_size(0)
		, m_recv_buf_size(0)
		/*, m_pool_ptr(NULL)
		, m_msgs(NULL)*/
	{
	}

	chttp_client::~chttp_client()
	{

	}

	bool chttp_client::init(nmsg_handler msg_handler, uint32 max_session, uint32 send_buf_size, uint32 recv_buf_size)
	{
		m_msg_handler = msg_handler;

		m_max_session = max_session;

		m_send_buf_size = send_buf_size;

		m_recv_buf_size = recv_buf_size;

		m_sessions.reserve(max_session);

		for (uint32 i = 0; i < m_max_session; ++i)
		{
			chttp_session* p = new chttp_session(m_io_service);

			if (!p)
			{
				ERROR_EX_LOG("alloc whttp_session memory error!");
				return false;
			}

			if (!p->init(this))
			{
				return false;
			}

			m_sessions.push_back(p);

			_return_session(p);
		}

		if (m_sessions.size() != m_max_session)
		{
			ERROR_EX_LOG("m_sessions.size() != m_max_session!");
			return false;
		}
		return true;
	}

	void chttp_client::destroy()
	{
		for (csessions::iterator it = m_sessions.begin(); it != m_sessions.end(); ++it)
		{
			delete (*it);
		}
		m_sessions.clear();
		m_available_sessions.clear();
	}

	bool chttp_client::startup()
	{
		// 开启状态
		m_is_working = true;

		// 创建完成包处理线程
		m_stop = false;
		
		m_threads.emplace_back(&chttp_client::_wrok_pthread, this);
		return false;
	}

	/*void chttp_client::shutdown()
	{

	}*/

	bool chttp_client::http_request(const std::string & host, const char * request_ptr, uint32 request_size, uint32 client_id, uint16 msg_id, uint32 time_out_sec)
	{
		if (!m_is_working)
		{
			return false;
		}

		chttp_session* session_ptr = _get_available_session();

		if (!session_ptr)
		{
			return false;
		}

		bool ret = session_ptr->http_request(host, request_ptr, request_size, client_id, msg_id, time_out_sec);

		if (!ret)
		{
			if (session_ptr->get_socket().is_open())
			{
				boost::system::error_code ec;
				session_ptr->get_socket().close(ec);
			}

			_return_session(session_ptr);
			return false;
		}

		return true;
	}

	void chttp_client::on_response(uint32 client_id, uint16 msg_id, const void* buf, uint32 size)
	{
		if (m_msg_handler)
		{
			m_msg_handler(client_id, msg_id, buf, size);
		}
	}

	void chttp_client::process_msg()
	{
	}

	void chttp_client::_wrok_pthread()
	{
		while (!m_stop)
		{
			try
			{
				m_io_service.run();
				NORMAL_LOG("nc_client_asio work thread exit");
				break;
			}
			catch (boost::system::system_error& e)
			{
				ERROR_EX_LOG("work thread exception, code = %d", e.code().value());
			}
		}
	}

	void chttp_client::shutdown()
	{

		// 设置关闭状态
		m_is_working = false;

		m_stop = true;

		// 关闭工作线程
		m_io_service.stop();

		//m_thread.join();
		for (std::thread &thread : m_threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
		m_threads.clear();

		for (uint32 i = 0; i < m_max_session; ++i)
		{
			m_sessions[i]->close();
		}
	}

}//namespace chen
