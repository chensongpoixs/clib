/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		net_mgr
************************************************************************************************/
#ifndef _C_NET_MGR_H_
#define _C_NET_MGR_H_
#include "cnoncopyable.h"
#include "creactor.h"
#include "cnet_types.h"
#include "cacceptor.h"
#include <iostream>
#include <vector>
#include "cnet_session.h"
#include <atomic>
#include "cqueue.h"
#include <queue>
#include <thread>
#include "cnet_msg.h"
namespace chen {
	class cnet_session;
	//typedef struct cmsg_data
	//{
	//		uint32 	m_client;
	//		uint32  m_status;
	//		uint32 	m_msg_id;
	//		char  	*m_ptr;
	//		uint64 	m_size;
	//		cmsg_data():m_client(0) , m_status(0), m_msg_id(0), m_ptr(NULL),m_size(0){}
	//}cmsg_data;


	class cnet_mgr :public cnoncopyable
	{
	public:
		//连接回调
		typedef std::function<void(uint32 session_id, uint32 para, const char* buf)> cconnect_cb;
		//断开连接回调
		typedef std::function<void(uint32 session_id)> 								cdisconnect_cb;
		//新消息回调
		typedef std::function<void(uint32 session_id, uint16 msg_id, const void* p, uint32 size)> cmsg_cb;
	private:
		typedef std::vector<cnet_session*> 											csessions;
		typedef std::vector<std::thread> 											cthreads;
		typedef std::atomic_bool 													catomic_bool;
		typedef std::mutex															clock_type;
		typedef std::lock_guard<clock_type>											clock_guard;
	public:
		explicit	cnet_mgr();
		virtual		~cnet_mgr();
	public:
		static cnet_mgr *	construct();
		static void			destroy(cnet_mgr * p);
	public:
		/**
		* @param name: 服务的名称
		* @param client_session:   作为客户端是转入 大于0 
		* @param max_session : 作为服务端 是输入大于0
		* @param send_buff_size: 发送缓冲区的大小
		* @param recv_buff_size: 接收缓冲区的大小
		*
		**/
		bool 		init(const std::string& name, uint32 client_session, uint32 max_session
			, uint32 send_buff_size, uint32 recv_buff_size);
		/************************************************************************/
		/* 关服操作                                                                   */
		/************************************************************************/
		void 		shutdown();
		void 		destroy();
	public:
		/**
		*  @param  thread_num io线程数
		*  @param  ip  
		*  @param  port   
		**/
		bool 		startup(uint32 thread_num, const char *ip, uint16 port);
	public:
		//消息id秘钥
		void set_msg_id_key(uint16 value) { m_msg_id_key = value; }

		//消息size秘钥
		void set_msg_size_key(uint32 value) { m_msg_size_key = value; }

		//最大接收消息大小
		void set_max_msg_size(uint32 value) { m_max_received_msg_size = value; }

		//自动重连时间
		void set_reconnet_second(uint32 seconds) { m_reconnect_second = seconds; }

		//是否外网
		void set_wan() { m_wan = true; }
	public:
		// client
		bool 		connect_to(uint32 index, const std::string& ip_address, uint16 port);
	public:
		//连接回调
		void set_connect_callback(cconnect_cb callback) { m_connect_callback = callback; }

		//断开连接回调
		void set_disconnect_callback(cdisconnect_cb callback) { m_disconnect_callback = callback; }

		//消息回调
		void set_msg_callback(cmsg_cb callback) { m_msg_callback = callback; }
	public:
		// 设置连接超时时间[心跳包] 单位 秒
		/*void set_times_second(uint32 msleep);*/
		//// 断线重连时间 单位 秒
		//void set_reconnection(uint32 msleep);
	public:
		void 		process_msg();
		/**
		*  @param  sessionId : 回话id
		*  @param  msg_id    : 消息id
		*  @param  msg_ptr   : 消息的数据
		*  @param  msg_size  : 消息的大小
		**/
		bool 		send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size);

		// 指定服务器发送
		bool 		send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size, int32 extra);
		/**
		*  @param sessionId  : 关闭指定的回话id
		**/
		void 		close(uint32 sessionId);
		// 连接客户端的数量和数据 信息  
		void		show_info();
	public:
		/*
		*   消息包
		*/
		void		msg_push(cnet_msg * msg_ptr);
	
	private:
		void 		_work_thread();
		/**
		* @param  psession : 清除指定得到session
		**/
		void		_clearup_session(cnet_session * psession);

		// 得到一个新的客户端连接
		void		_new_connect();
	private:
		// server api
		// 服务器异步监听是否有新的客户端连接
		void		_listen_start(const char *ip, uint16 port);

	private:
		// client api
		// 同步连接服务器
		bool		_connect_start(uint32 index, const char *ip, uint16 port);
	private:
		// 取得一个可用的会话
		cnet_session* _get_available_session();

		// 归还一个关闭的会话
		void _return_session(cnet_session* session_ptr);
	private: 	
		std::string						m_name;
		cacceptor*						m_acceptor_ptr;		// 监听socket
		creactor*						m_reactor;          // 反应堆
		// callback			
		cconnect_cb						m_connect_callback;
		cdisconnect_cb					m_disconnect_callback;
		cmsg_cb							m_msg_callback;
		csessions						m_sessions;               //所有会话
		clock_type						m_avail_session_mutex;
		csessions						m_available_sessions;     //开使用的会话
		catomic_bool					m_shuting;
		cthreads 						m_threads;    
		std::queue<cnet_msg*>			m_msgs;   
		//config
		uint16							m_msg_id_key;
		uint32							m_msg_size_key;
		uint32							m_max_received_msg_size;
		bool							m_wan;  //是否是网关

		//client
		uint32							m_reconnect_second;
		uint32							m_active_num; //反应堆的数量
	};
} // chen 

#endif // _C_NET_MGR_H_
