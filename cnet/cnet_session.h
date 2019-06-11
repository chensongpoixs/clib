/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
************************************************************************************************/

#ifndef _C_NET_SESSION_H_
#define _C_NET_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include <mutex>
#include "cnet_mgr.h"
namespace chen  {
	
	class cnet_mgr;
	class cnet_msg;
	class cnet_session :private cnoncopyable 
	{
	private:
		typedef std::mutex															clock_type;
		typedef std::lock_guard<clock_type>											clock_guard;
	private:
		enum ESESSIONSTATUS
		{
			ESS_NONE = 0,
			ESS_INIT,
			ESS_OPEN,
			ESS_CLOSE,
		};
	public:
		
		explicit cnet_session(cnet_mgr&  _ptr, uint32 session_id);
		virtual ~cnet_session();
	public:
		/**
		* @param   send_buff_size : 发送缓冲区的大小
		* @param   recv_buff_size : 接收缓冲区的大小
		**/
		bool 	init(uint32 send_buff_size, uint32 recv_buff_size);
		void 	destroy();
	public:
		bool	is_init() const { return m_status == ESS_INIT; }
		bool	is_open() const { return m_status == ESS_OPEN; }
		bool	is_close() const { return m_status == ESS_CLOSE; }

	public:
		// 读取数据
		bool	processinputs();
		//发送缓冲区中数据
		bool	processoutputs();
	private:
		/**
		*  @param  socket_status : 一个信息包的 发送状态  1. new client   2. 接收数据  3.  关闭连接信息包
		**/
		cnet_msg*  _next_new_net_msg(uint32 socket_status );
		//void *		_next_new_msg_ptr();
	public:
		/**
		* 发送数据包缓存到发送缓冲区中
		* @param   p	: 发送数据包
		* @param   size	: 发送数据包的大小
		**/
		bool		send_data(const void *p, uint32 size);
		
		void		clearup();
		void		set_connect(socket_type sockfd);
		socket_type &get_sockfd() { return m_sockfd; };
		uint32 	get_session() const { return m_client_session; }
	//private:
	//	cnet_session(const cnet_session&);
	//	cnet_session& operator=(const cnet_session&);
	private:
		cnet_mgr&				m_net_ptr;
		uint32					m_client_session;		// 网络层的会话id
		uint32					m_status;
		socket_type				m_sockfd;
		int32 		 			m_event;
		clock_type 				m_mutex;
		char *					m_recv_buf_data_ptr;   // temp
		uint32					m_recv_size;			//接收缓冲区大小
		char *					m_recv_buf_ptr;
		uint32					m_recv_available_size;// 接收缓冲区可用大小
		//send msg
		uint32					m_data_buf_size;     //待发送缓冲区大小
		char *					m_data_buf_ptr;	
		uint32					m_send_available_size; // 发送缓冲区的可用大小
		uint32					m_send_size;           //发送缓冲区的大小
		char *					m_sending_buf_ptr;	
		clock_type				m_send_mutex;
		
	};
	
}  //namespace chen

#endif // !_C_NET_SESSION_H_