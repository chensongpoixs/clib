﻿/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
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
#include "cnet_mgr.h"
#include "clog.h"
#include "cnet_msg_reserve.h"
#include "cnet_msg_header.h"
//#include "clib_util.h"
namespace chen {
	cnet_mgr::cnet_mgr()
		:m_connect_callback(NULL)
		, m_disconnect_callback(NULL)
		, m_msg_callback(NULL)
		, m_shuting(false)
		,m_msg_id_key(0)
		, m_msg_size_key(0)
		, m_max_received_msg_size(0)
		, m_wan(false)
		, m_reconnect_second(0)
		, m_active_num(0)
		, m_listening(true)
	{
#if defined(_MSC_VER)
		//begin 初始化网络环境
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{            //加载winsock2库
			printf("加载失败./n");
			return;
		}

#endif // !#if defined(_MSC_VER)
	}
	cnet_mgr::~cnet_mgr()
	{
#if defined(_MSC_VER)
		::WSACleanup();
#endif // !#if defined(_MSC_VER)

	}
	cnet_mgr * cnet_mgr::construct()
	{
		return new cnet_mgr();
	}
	void cnet_mgr::destroy(cnet_mgr * p)
	{
		//ERROR_EX_LOG("destroy net");
		delete p;
	}
	bool 		cnet_mgr::init(const std::string& name, uint32 client_session, uint32 max_session
		, uint32 send_buff_size, uint32 recv_buff_size)
	{
		m_name = name;

		/*if (client_session > max_session)
		{
			ERROR_EX_LOG(" client_session= %lu > max_session = %lu", client_session, max_session);
			return false;
		}*/

		// client init
		if (client_session > max_session)
		{
			m_listening = false;
			m_sessions.reserve(client_session);
			for (uint32 i = 0; i < client_session; ++i)
			{
				m_sessions.push_back(new cnet_session(*this, i));
			}
			for (cnet_session * session : m_sessions)
			{
				if (!session->init(send_buff_size, recv_buff_size))
				{
					return false;
				}

				//	m_available_sessions.push_back(session);  // client m_session_ptrs
					//std::move(m_available_sessions, m_sessions);
			}
			m_active_num = client_session;
		}

       // ERROR_EX_LOG("max_session new session fail  max_session = %lu", max_session);

		// server init
		if (client_session < max_session)
		{
			m_sessions.reserve(max_session);
			for (uint32 i = 0; i < max_session; ++i)
			{
				m_sessions.push_back(new cnet_session(*this, i));
			}
			if (m_sessions.size() != max_session)
			{
				ERROR_EX_LOG("max_session new session fail  max_session = %lu", max_session);
				return false;
			}
			for (cnet_session * session : m_sessions)
			{
				if (!session->init(send_buff_size, recv_buff_size))
				{
					return false;
				}

				m_available_sessions.push_back(session);
				//std::move(m_available_sessions, m_sessions);
			}
			m_active_num = max_session;
		}


		m_msgs = new cnet_msg_queue();
		if (!m_msgs)
		{
			ERROR_EX_LOG("alloc new msg queue");
			return false;
		}
			return true;
	}
		void 		cnet_mgr::destroy()
		{
			m_shuting = true;
			// clone new accept   client

            /// ?????????? ??????????
			SYSTEM_LOG(" net destroy");
			if (m_reactor)
			{
				m_reactor->destroy();
				delete m_reactor;
			}
            if (m_acceptor_ptr)
            {
                //m_acceptor_ptr->shutdown();
                m_acceptor_ptr->destroy();
                cacceptor::destroy(m_acceptor_ptr);
            }
            for (cnet_session *client : m_sessions)
            {
                client->destroy();
            }
			for (std::thread &thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			m_threads.clear();
			if (m_msgs)
			{
				delete m_msgs;
				m_msgs = NULL;
			}
		}
	
		bool 		cnet_mgr::startup(uint32 thread_num, const char *ip, uint16 port)
		{
			
			//m_acceptor_ptr = new cacceptor();
			m_acceptor_ptr = cacceptor::construct();
			if (!m_acceptor_ptr)
			{
				ERROR_EX_LOG(" acceptor alloc fail ");
				return false;
			}
			/*if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG(" acceptor init fail ");
				return false;
			}*/
			//反应堆描述监听
			/*if (!m_acceptor_ptr->create())
			{

				ERROR_EX_LOG(" create socket faill ");
				return false;
			}*/

			// active
			m_reactor = new creactor();
			if (!m_reactor)
			{
				ERROR_EX_LOG("create alloc fail ");
				return false;
			}
			if (!m_reactor->init(m_acceptor_ptr->get_sockfd(), m_active_num + 1/*反应堆的数量*/))
			{
				
				return false;
			}
			{
			
				//timer

				socket_type	m_sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
				if (m_sockfd == INVALID_SOCKET)
				{
					ERROR_EX_LOG("create socket ");
					return false;
				}
				//		// nlonl
				if (!csocket_ops::set_nonblocking(m_sockfd))
				{
					ERROR_EX_LOG("set create socket nonblocking ");
					return false;
				}
				m_reactor->register_read_descriptor(m_sockfd, NULL);
			}
			if (port)
			{
				_listen_start(ip, port);
			}
			
			for (uint32 i = 0; i < thread_num; ++i)
			{
				// thread
				m_threads.emplace_back(&cnet_mgr::_work_thread, this);
			}
			return true;
		}
		
		
		bool 		cnet_mgr::connect_to(uint32 index, const std::string& ip_address, uint16 port)
		{

			if (!_connect_start(index, ip_address.c_str(), port))
			{
				return false;
			}
		
			return true;
		}
		bool 		cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, int64 msg_size)
		{
			// session --> status
			clock_guard guard(m_send_data_mutex);
			cnet_session * session = m_sessions[sessionId];
			if (!session)
			{
				WARNING_EX_LOG(" find failed session id = %d", sessionId);
				return false;
			}
			if (!session->is_open())
			{
				WARNING_EX_LOG(" socket not open  = %d", sessionId);
				return false;
			}
			/*
			**信息包  size;
			**/
			cnet_msg_header msg_header;
			msg_header.m_msg_id = msg_id;
			msg_header.m_size = msg_size;
			if (!session->send_data(&msg_header, MSG_HEADER_SIZE))
			{
				return false;
			}
			/************************************************************************/
			/* 包                                                                     */
			/************************************************************************/
			if (!session->send_data(msg_ptr, msg_size))
			{
				return false;
			}
			m_reactor->register_write_descriptor(session->get_sockfd(), session);
			////发送数据   优化程序
			//if (!session->processoutputs())
			//{
			//	return false;
			//}
			return true;
		}

		bool cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, int64 msg_size, int32 extra)
		{
			// session --> status
            clock_guard guard(m_send_data_mutex);
			cnet_session * session = m_sessions[sessionId];
			if (!session)
			{
				WARNING_EX_LOG(" find failed session id = %d", sessionId);
				return false;
			}
			if (!session->is_open())
			{
				return false;
			}
			/*
			**信息包  size;
			**/
			cnet_msg_header msg_header;
			msg_header.m_msg_id = msg_id;
			msg_header.m_size = msg_size;
			msg_header.m_extra = extra;
			if (!session->send_data(&msg_header, MSG_HEADER_SIZE))
			{
				session->back_off_msg_size(MSG_HEADER_SIZE);
				return false;
			}
			/************************************************************************/
			/* 包                                                                     */
			/************************************************************************/
			if (!session->send_data(msg_ptr, msg_size))
			{
                session->back_off_msg_size(MSG_HEADER_SIZE);
				return false;
			}
			//注册写事件
			m_reactor->register_write_descriptor(session->get_sockfd(), session);
			/*if (!session->processoutputs())
			{
				return false;
			}*/
			return true;
		}

		void 		cnet_mgr::shutdown()
		{
			ERROR_EX_LOG("shutdown");
			m_shuting.store(true);
			//destroy();
			// accept 
			//if (m_acceptor_ptr)
			//{
			//	delete m_acceptor_ptr;
			//}
			//if (m_reactor)
			//{
			////	m_reactor->shutdown();
			//	delete m_reactor;
			//}
			
			/*for (std::thread &thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			m_threads.clear();*/
		}
		
		//void cnet_mgr::set_times_second(uint32 ms)
		//{
		//	//m_reconnection
		//}

		/*void cnet_mgr::set_reconnection(uint32 msleep)
		{
			m_reconnection = msleep;
		}*/

		void 		cnet_mgr::process_msg()
		{
			//cnet_msg* msg_ptr = NULL;
			if (m_msgs->empty())
			{
				return;
			}
			/*if (!m_msgs.is_full())
			{
				return;
			}
			SYSTEM_LOG( "msg size = %d", m_msgs.size());*/
			//SYSTEM_LOG("msgs size = %lu", m_msgs.size());
			cnet_msg* msg_ptr = m_msgs->dequeue();
			//task = m_task.front();
			
		/*	m_msgs.pop(msg_ptr);*/
			/*if (msg_ptr)
			{
				SYSTEM_LOG("msg_ptr === size =%d", msg_ptr->get_status());
			}*/
			while (msg_ptr)
			{
				//m_msgs.pop();
				//m_msgs.pop_front();
				if (msg_ptr->get_status() == EMIR_Connect)
				{
					//cmsg_connect* p = (cmsg_connect*)msg_ptr->get_buf();
					m_connect_callback(msg_ptr->get_session_id(), msg_ptr->get_size(), (const char *)msg_ptr->get_buf());
				}
				else if (msg_ptr->get_status() == EMIR_Disconnect)
				{
					m_disconnect_callback(msg_ptr->get_session_id());
				}
				else
				{
					m_msg_callback(*msg_ptr);
				}
				cnet_msg* new_msg_ptr = msg_ptr->get_next_msg();
				if (msg_ptr->get_buf())
				{
					msg_ptr->free_me();
					//delete[] msg_ptr->m_ptr;
				}
				delete msg_ptr;
				msg_ptr = NULL;
				 
				if (!new_msg_ptr )
				{
					break;
				}
				msg_ptr = new_msg_ptr;
			}
		}
		void 		cnet_mgr::close(uint32 sessionId)
		{ // TODO@chensong 2020-05-16 临时这样处理
			{
				//clock_guard guard(m_avail_session_mutex);
				cnet_session * session_ptr = m_sessions[sessionId];
				if (!session_ptr)
				{
					WARNING_LOG(" close session session %lu", sessionId);
					return;
				}

				_clearup_session(session_ptr);
			}
		}

		void cnet_mgr::show_info()
		{
			clock_guard guard(m_avail_session_mutex);
			SYSTEM_LOG("%s info m_listening_num = %lu, available_sessions %lu", m_name.c_str(), m_sessions.size(), m_available_sessions.size());
		}

		void cnet_mgr::msg_push(cnet_msg * msg_ptr)
		{
			m_msgs->enqueue(msg_ptr);
		}
		void 		cnet_mgr::_work_thread()
		{
			/*using namespace std::chrono;

			uint32_t elapse = 0;
			steady_clock::time_point cur_time;
			steady_clock::time_point active_time;
			steady_clock::time_point pre_time = steady_clock::now();
			steady_clock::duration dur;
			milliseconds ms;
			uint32_t	 net_elapse = 0;*/
			while (!m_shuting)
			{
				// write read queue
			/*	cur_time = steady_clock::now();
				dur = cur_time - pre_time;
				ms = duration_cast<milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				pre_time = cur_time;*/
				int32 num_event = m_reactor->select(200);
				/*active_time = steady_clock::now();
				dur = active_time - pre_time;
				ms = duration_cast<milliseconds>(dur);
				net_elapse = static_cast<uint32_t>(ms.count());*/
				//SYSTEM_LOG("num_event = %ld", num_event);
				for (int32 i = 0; i < num_event; ++i)
				{
					cnet_session * session = static_cast<cnet_session*>(m_reactor->get_para(i));
					
					if (!session)
					{
//					    ERROR_EX_LOG("new client appect  session == null");
#if ! defined(__APPLE__) // TODO@chensong 2020-05-15   mac
						if (m_reactor->is_exception(i))
						{
							ERROR_EX_LOG("listen socket has exception");
							continue;
						}
#endif // #if ! defined(__APPLE__)
						if (m_reactor->is_read(i))
						{
							// new client
							
							_new_connect();
							
						}
						{
							clock_guard guard(m_avail_session_mutex);
							if (m_available_sessions.size() == 0 )
							{
								m_reactor->deregister_descriptor(m_acceptor_ptr->get_sockfd());
							}
						}
						continue;
					 }

					 if (!session->is_open())
					{
						continue;
					}

					if (m_reactor->is_read(i))
					{
						if (!session->processinputs())
						{
							SYSTEM_LOG("read clearup");
							_clearup_session(session);
							continue;
						}
					}
#if ! defined(__APPLE__) // TODO@chensong 2020-05-15   mac
					if (m_reactor->is_exception(i))
					{
						SYSTEM_LOG("is_exception clearup");
						ERROR_EX_LOG("pSession %u has exception, disconnected", session->get_sockfd());
						_clearup_session(session);
						continue;
					}
#endif// #if ! defined(__APPLE__)
					if (m_reactor->is_write(i))
					{
						
						if (!session->processoutputs())
						{
							SYSTEM_LOG("is_write clearup");
							_clearup_session(session);
                            continue;
						}
						//注销写事件
						m_reactor->deregiter_write_descriptor(session->get_sockfd(), session);
						continue;
					}
				}
#if defined(_MSC_VER)
				//if (num_event <= 0)
				//{
				//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
				//	ERROR_EX_LOG("sleep 200");
				//	//csleep(200);
				//}
#endif // !#if defined(_MSC_VER)
			/*	cur_time = steady_clock::now();

				dur = cur_time - pre_time;
				ms = duration_cast<milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				ERROR_EX_LOG("[net work ]net_elapse = %u, elapse = %u, timer = %u", net_elapse,  elapse, cur_time.time_since_epoch().count());*/
			}


			ERROR_EX_LOG("work_thread exit");
		}
		void cnet_mgr::_clearup_session(cnet_session * psession)
		{
			m_reactor->deregister_descriptor(psession->get_sockfd());
			uint32 session = psession->get_session();
			psession->clearup();
			size_t size = m_available_sessions.size();
			_return_session(psession);
			cnet_msg * msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				ERROR_EX_LOG("new faild msg_ptr = NULL");
				return;
			}
			msg_ptr->set_session_id(session);
			msg_ptr->set_status(EMIR_Disconnect);
			msg_ptr->set_msg_id(0);
			//msg_ptr->m_client = session;
			//msg_ptr->m_status = EMIR_Disconnect;
			//msg_ptr->m_ptr = NULL;
			//msg_ptr->m_size = 0;
			m_msgs->enqueue(msg_ptr);
			// disconnect
			//m_msgs.push();
			if (m_listening && size == 0 && m_available_sessions.size() == 1)
			{
				m_reactor->register_read_descriptor(m_acceptor_ptr->get_sockfd(), NULL);
			}
		}
		void cnet_mgr::_new_connect()
		{
			sockaddr_in sock_addr;
			int err_code;
			//while (true)
			{
				socket_type	tmpsocket;

				memset(&sock_addr, 0, sizeof(sock_addr));
				sock_addr.sin_family = AF_INET;
				//-- accept socket
				if (!m_acceptor_ptr->accept(tmpsocket, sock_addr, err_code))
				{
					ERROR_EX_LOG("new accept client  failed");
					if (err_code == -1 /*WSAEWOULDBLOCK*/)
					{
						return;
					}
					else
					{
						return;
					}
				}
				/*uint32 addrlen = sizeof(sock_addr);
				socket = csocket_ops::accept(m_sockfd, (struct sockaddr *)(&(sock_addr)), (socklen_t*)&addrlen, err_code);
				if (socket == SOCKET_ERROR)
				{
					return false;
				}*/
				if (!csocket_ops::set_nonblocking(tmpsocket))
				{
					csocket_ops::close_socket(tmpsocket);
					return;
				}
				if (!m_acceptor_ptr->set_nosigpipe(tmpsocket))
				{
					csocket_ops::close_socket(tmpsocket);
					return;
				}
				cnet_session *session_ptr = _get_available_session();
				if (!session_ptr)
				{
					//没有已经是最大连接数了  就通知客户端
					ERROR_EX_LOG("session ptr == NULL");
					/*cnet_msg_header msg_header;
					msg_header.m_msg_id = 109;
					
					
					::send(tmpsocket, (const char *)&msg_header, MSG_HEADER_SIZE, 0);*/
					csocket_ops::close_socket(tmpsocket);
					return;
				}
				session_ptr->set_connect(tmpsocket);
				//m_reactor->register_read_descriptor(tmpsocket, session_ptr);
				m_reactor->register_read_descriptor(tmpsocket, session_ptr);

			//	uint32 port = ntohs(sock_addr.sin_port);
				//uint32 ip = (uint32)(sock_addr.sin_addr.s_addr);
				// connect
				cnet_msg * msg_ptr = new cnet_msg;
				if (!msg_ptr)
				{
					ERROR_EX_LOG("new faild msg_ptr = NULL");
					return;
				}
				msg_ptr->set_session_id(session_ptr->get_session());
				msg_ptr->set_status(EMIR_Connect);
				//msg_ptr->m_client = session_ptr->get_session();/*tmpsocket*/;
				//msg_ptr->m_status = EMIR_Connect;
				//msg_ptr->m_ptr = NULL;
				//msg_ptr->m_size = 0;
				m_msgs->enqueue(msg_ptr);
				//m_msgs.push(msg_ptr);
			}
		}
		void cnet_mgr::_listen_start(const char * ip, uint16 port)
		{
			if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG("server acceptor init fail ip =%s, port =%lu ", ip, port);
				return ;
			}
			if (!m_acceptor_ptr->create())
			{
			//	destroy();
				ERROR_EX_LOG(" create socket faill ");
				return;
			}
            // listen 不需要 屏蔽 pipsinge
//			if (!m_acceptor_ptr->set_nosigpipe())
//            {
//			    return;
//            }
			// bind
			if (!m_acceptor_ptr->bind())
			{
				//destroy();
				return ;
			}

			// listen
			if (!m_acceptor_ptr->listen())
			{
				//destroy();
				return;
			}
            // nlonl
          /*  if (!csocket_ops::set_nonblocking(m_acceptor_ptr->get_sockfd()))
            {
                ERROR_EX_LOG("set create socket nonblocking ");
                return;
            }*/
			if (!m_reactor)
			{
				// active
				m_reactor = new creactor();
				if (!m_reactor)
				{
					ERROR_EX_LOG("create alloc fail ");
					//destroy();
					return;
				}
			}
			//if (!m_reactor->init(m_acceptor_ptr->get_sockfd(), 2000 /*反应堆的数量*/))
			//{
			//	destroy();
			//	return ;
			//}
			m_reactor->register_read_descriptor(m_acceptor_ptr->get_sockfd(), NULL);
			NORMAL_LOG("listen ip = %s, port = %u", ip, port);
		}
		bool cnet_mgr::_connect_start(uint32 index, const char * ip, uint16 port)
		{
			if (!m_acceptor_ptr->init(ip, port))
			{
				ERROR_EX_LOG("clinet acceptor init failed ip =%s, port =%d ", ip, port);
				return false;
			}
			/// create socket   nonbalck
			if (!m_acceptor_ptr->create())
			{
				//	destroy();
				ERROR_EX_LOG(" create socket faill ");
				return false;
			}
			// open nagle 算法
			if (!m_acceptor_ptr->set_nonagle(false))
			{
				ERROR_EX_LOG("open tcp_nodelay nagle failed");
				return false;
			}
            //客户端使用心跳包 来看是否断开了
			if (!m_acceptor_ptr->set_nosigpipe(m_acceptor_ptr->get_sockfd()))
            {
			    return false;
            }
			if (!m_acceptor_ptr->async_connect(/*sockfd*/))
			{
				ERROR_EX_LOG(" async_connect failed ");
				return false;   //TODO@chensong 暂时还没有找到win上连接connect 判断是否连接上错误判断 
			}

			cnet_session *session_ptr = m_sessions[index];
			if (!session_ptr)
			{
				ERROR_EX_LOG("session ptr == NULL");
				return false;
			}
			session_ptr->set_connect(m_acceptor_ptr->get_sockfd());
//			m_reactor->register_readwrite_descriptor(m_acceptor_ptr->get_sockfd(), session_ptr);
			m_reactor->register_read_descriptor(m_acceptor_ptr->get_sockfd(), session_ptr);
		//	cmsg_data * msg_ptr = new cmsg_data;
			cnet_msg *msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				ERROR_EX_LOG("new faild msg_ptr = NULL");
				return false;
			}
			msg_ptr->set_session_id(session_ptr->get_session());
			msg_ptr->set_status(EMIR_Connect);
			//msg_ptr->m_client = session_ptr->get_session();/*tmpsocket*/;
			//msg_ptr->m_status = EMIR_Connect;
			//msg_ptr->m_ptr = NULL;
			//msg_ptr->m_size = 0;
			m_msgs->enqueue(msg_ptr);

			return true;
		}
		cnet_session * cnet_mgr::_get_available_session()
		{
			cnet_session* temp_ptr = NULL;
			{
				clock_guard guard(m_avail_session_mutex);
				if (m_available_sessions.empty())
				{
					return NULL;
				}
				temp_ptr = m_available_sessions.back();
				m_available_sessions.pop_back();
				//m_available_sessions.push_back(temp_ptr);
			}
			return temp_ptr;
		}
		void cnet_mgr::_return_session(cnet_session * session_ptr)
		{
			clock_guard guard(m_avail_session_mutex);
			//m_available_sessions.remove(m_avail_session_mutex);
			m_available_sessions.push_back(session_ptr);
		}
}// namespce chen


