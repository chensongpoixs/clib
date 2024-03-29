﻿/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_session
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

#ifndef _C_NET_SESSION_H_
#define _C_NET_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include <mutex>
#include "cnet_mgr.h"
#include "cnet_msg_reserve.h"
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
		bool 	init(int64 send_buff_size, int64 recv_buff_size);
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

	public:
		/**
		* 发送数据包缓存到发送缓冲区中
		* @param   p	: 发送数据包
		* @param   size	: 发送数据包的大小
		**/
		bool		send_data(const void *p, int64 size);
		
		void		clearup();
		void		set_connect(socket_type sockfd);
		socket_type &get_sockfd() { return m_sockfd; };
		uint32 		get_session() const { return m_client_session; }
		void        back_off_msg_size(int64 size);
    private:
        /**
        *  @param  socket_status : 一个信息包的 发送状态  1. new client   2. 接收数据  3.  关闭连接信息包
        **/
        cnet_msg*  _next_new_net_msg(EMsgIDReserve socket_status );
        //void *		_next_new_msg_ptr();
        /**
         * 发送缓存移动正在发送缓存区中
         * @return
         */
        bool       _send_buf_move_sending_buf();
	//private:
	//	cnet_session(const cnet_session&);
	//	cnet_session& operator=(const cnet_session&);
	private:
		cnet_mgr&				m_net_ptr;
		uint32					m_client_session;		// 网络层的会话id
		uint32					m_status;
		socket_type				m_sockfd;
//		int32 		 			m_event;
		clock_type 				m_recv_mutex;
		char *					m_recv_buf_data_ptr;   // temp
        int64					m_recv_size;			//接收缓冲区大小
		char *					m_recv_buf_ptr;
		int64					m_recv_available_size;// 接收缓冲区可用大小
		//send msg
        int64					m_data_buf_size;     //待发送缓冲区大小
		char *					m_data_buf_ptr;
        int64					m_send_available_size; // 发送缓冲区的可用大小
        int64					m_send_size;           //发送缓冲区的大小
		char *					m_sending_buf_ptr;	
		clock_type				m_send_mutex;
		
	};
	
}  //namespace chen

#endif // !_C_NET_SESSION_H_