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

#ifndef _C_HTTP_CLIENT_H_
#define _C_HTTP_CLIENT_H_
#include "cnoncopyable.h"
#include "cnet_define.h"
#include "cnet_types.h"
#include <mutex>
#include <functional>
#include <list>
//#include "chttp_session.h"

namespace chen {

    class chttp_session;
	// http消息数据
	struct chttp_message
	{
		uint32							client_id;			// 用户id
		boost::asio::streambuf			request;			// 请求缓存数据
		boost::asio::streambuf			response;			// 回复缓存数据
		uint32							status_code;		// 状态码
		std::string						error_msg;			// 错误消息
		uint16							msg_id;				// 回调消息id

		chttp_message() : client_id(0), status_code(0), msg_id(0) {}
	};


	class chttp_client : private cnoncopyable
	{
	private:
		friend class chttp_session;
		typedef		std::vector<std::thread>								cthreads;
	public:
		typedef std::function<void(uint32 client_id, uint16 msg_id, const void* buf, uint32 size)> nmsg_handler;

	public:
		/*explicit*/ chttp_client();
		~chttp_client();
	public:
		bool init(nmsg_handler msg_handler, uint32 max_session, uint32 send_buf_size, uint32 recv_buf_size);
		void destroy();
		bool startup();

		/**
		* 关闭客户端，该函数所在执行线程不被强制关闭，建议在主线程中关闭
		*/
		void shutdown();

		/**
		* 异步发送http请求
		* @param host			host地址
		* @param method		方法
		* @param param			参数
		* @param client_id		请求标识id
		* @param msg_id		返回消息id
		* @param time_out_sec	超时时间，单位s
		* @return				true请求成功, false表示请求投递失败
		*/
		bool http_request(const std::string& host, const char* request_ptr, uint32 request_size, uint32 client_id, uint16 msg_id, uint32 time_out_sec = 10);

		/**
		* 消息回复回调函数, 注意线程安全
		* 函数不要处理过多的逻辑，转发消息或将消息投递到统一的逻辑消息队列
		* @param msg			消息数据
		*/
		void on_response(uint32 client_id, uint16 msg_id, const void* buf, uint32 size);

		uint32 get_send_buf_size() const { return m_send_buf_size; }
		uint32 get_recv_buf_size() const { return m_recv_buf_size; }
	public:
		void process_msg();
	private:
		// iocp工作线程负责处理完成包
		void _wrok_pthread();

	public:
		// 取得一个可用的会话
		chttp_session* _get_available_session()
		{
			guard_type guard(m_mutex);

			if (!m_is_working || m_available_sessions.empty())
			{
				return NULL;
			}

			chttp_session* temp_ptr = m_available_sessions.front();
			m_available_sessions.pop_front();
			return temp_ptr;
		}

		// 归还一个关闭的会话
		void _return_session(chttp_session* session_ptr)
		{
			guard_type guard(m_mutex);

			if (!session_ptr)
			{
				return;
			}

			m_available_sessions.push_back(session_ptr);
		}

	private:
		typedef std::mutex							lock_type;
		typedef std::lock_guard<lock_type>			guard_type;
		typedef std::vector<chttp_session*>			csessions;

	private:
		boost::asio::io_service			m_io_service;			// io服务
		boost::asio::io_service::work	m_work;					// 保证io服务work
		bool							m_is_working;			// 服务是否开启

		lock_type						m_mutex;				// 保护连接池线程安全及状态改变
		csessions						m_sessions;
		std::list<chttp_session*>		m_available_sessions;

		cthreads							m_threads;
		volatile bool					m_stop;

		uint32							m_max_session;			// 最大连接数

		nmsg_handler					m_msg_handler;

		uint32							m_send_buf_size;
		uint32							m_recv_buf_size;
	};
}//namespace chen

#endif // !#define _C_HTTP_CLIENT_H_