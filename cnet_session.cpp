/***********************************************************************************************
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
#include "cnet_session.h"
#include "clog.h"
#include "cscoped_ptr.h"
#include "csocket_ops.h"
#include "cnet_msg_reserve.h"
#include "cnet_msg_header.h"
#include "cstr2digit.h"
namespace chen  {
	
	
	
	cnet_session::cnet_session(cnet_mgr & _ptr, uint32 session_id)
		: m_net_ptr(_ptr) 
		, m_client_session(session_id)
		, m_status(ESS_NONE)
		, m_sockfd(0)
//		, m_event(0)
		, m_recv_size(0)
		, m_recv_buf_ptr(NULL)
		, m_data_buf_size(0)
		, m_data_buf_ptr(NULL)
		, m_send_size(0)
		, m_sending_buf_ptr(NULL)
		
	{
	}

	cnet_session::~cnet_session()
	{
	}

	bool 	cnet_session::init(int64 send_buff_size, int64 recv_buff_size)
	{
		m_recv_buf_ptr = new char [recv_buff_size];
		if (!m_recv_buf_ptr)
		{
			ERROR_EX_LOG ("alloc m_recv_buf_ptr memory error!");
			return false;
		}

		m_recv_buf_data_ptr = new char[recv_buff_size];
		if (!m_recv_buf_data_ptr)
		{
			ERROR_EX_LOG("alloc m_recv_buf_data_ptr memory error!");
			return false;
		}

		m_data_buf_ptr = new char[send_buff_size];
		if (!m_data_buf_ptr)
		{
			ERROR_EX_LOG("alloc send_buff_size memory error!");
			return false;
		}

		m_sending_buf_ptr = new char[send_buff_size];
		if (!m_sending_buf_ptr)
		{
			ERROR_EX_LOG("alloc sending_buff_size memory error!");
			return false;
		}

		m_status = ESS_INIT;
		m_send_size = send_buff_size;
		m_recv_size = recv_buff_size;
		m_send_available_size = send_buff_size;
		m_recv_available_size = recv_buff_size;
		return true;
	}
	bool cnet_session::processinputs()
	{
	    clock_guard guard(m_recv_mutex);
		//int64 temp_recv_size = ::recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), m_recv_available_size, 0);
		if (m_recv_available_size <= 0)
        {
            ERROR_EX_LOG(" not use mem size m_recv_available_size = %u", m_recv_available_size);
		    return true;
        }
        int64  used_mem_size = (m_recv_available_size > 1024 ? 1024 : m_recv_available_size);
		int32 temp_recv_size = csocket_ops::async_recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), used_mem_size, 0);


        if (temp_recv_size <= 0)
        {
            WARNING_LOG("recv size <= 0, used_mem_size = %u, temp_recv_size = %d", used_mem_size, temp_recv_size);
            return false;
        }
//        if (temp_recv_size == 0 /*|| temp_recv_size == -1*/)
//        {
//            WARNING_LOG("recv size = 0 session_id = %u, temp_recv_size = %d", m_client_session, temp_recv_size);
//            return false;
//        }
//        SYSTEM_LOG(" recv ok ");
		m_recv_available_size -= temp_recv_size;

		while (used_mem_size <= temp_recv_size && m_recv_available_size>0)
		{
            used_mem_size = m_recv_available_size;
			temp_recv_size = csocket_ops::async_recv(m_sockfd, m_recv_buf_ptr + (m_recv_size - m_recv_available_size), m_recv_available_size/*m_recv_available_size*/, 0);
			if (temp_recv_size > 0)
			{
				m_recv_available_size -= temp_recv_size;
			}
			else
			{
				break;
			}
		}

		if (m_recv_available_size <= 0)
        {
		    SYSTEM_LOG("not used available mem size !!!!");
        }
		/// 接收数据包的大小
		while (m_recv_size - m_recv_available_size >= MSG_HEADER_SIZE)
		{
			//分包 沾包问题的处理
			cnet_msg * recv_msg_ptr = _next_new_net_msg(EMIR_Close);
			if (recv_msg_ptr)
			{
				m_net_ptr.msg_push(recv_msg_ptr);
			}
			else
			{
				//数据包不完整
				break;
			}
		}
//		if (m_recv_size - m_recv_available_size > 0)
//        {
//		    SYSTEM_LOG("recv size buf too small !!! size = %u", m_recv_size - m_recv_available_size);
//        }
		// 这里需要经常是否读入完成 没有完成就继续读取数据 直到全部读取完成？？？？？？？？？？？？？？？？？ loop 反应堆
		//m_data_buf_size = 0;
		return true;
	}
	bool cnet_session::processoutputs()
	{
		clock_guard			guard(m_send_mutex);
	//	std::lock_guard<std::mutex> guard(m_send_mutex);
		if (m_data_buf_size <= 0)
		{
            WARNING_LOG("sesion_id = %u, not available send data!!!!", m_client_session);
			return false;
		}
        if (false == is_open())
        {
            WARNING_LOG(" file not open socket = %d", m_client_session);
            return false;
        }
        //如果正在发送缓存中还有数据就先发送正在缓冲区的的数据 ，发完了 在移动发送缓冲区到正在发送缓冲区中
		if (!_send_buf_move_sending_buf())
        {
            if (m_data_buf_ptr && m_data_buf_size > 0)
            {
                memcpy(m_sending_buf_ptr +(m_send_size - m_send_available_size) , m_data_buf_ptr, m_data_buf_size);

                m_send_available_size -= m_data_buf_size;
                m_data_buf_size = 0;
            }
        }
        if ((m_send_size - m_send_available_size) > 0)
        {
            int32 temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr, (m_send_size - m_send_available_size), 0);
            //已经发送的数据块的大小
            int32 send_size = temp_send_size;
            while ((m_send_available_size + temp_send_size) < m_send_size)
            {
                m_send_available_size += temp_send_size;
                send_size += temp_send_size;
                temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr + send_size, (m_send_size - m_send_available_size)/*m_send_size*/, 0);
            }
            m_send_available_size = m_send_size;
        }

		return true;
	}
    void        cnet_session::back_off_msg_size(int64 size)
    {
	    if (m_data_buf_size < size)
        {
	        WARNING_LOG("send data size back off m_data_buf_size = %u, size = %u", m_data_buf_size, size);
	        m_data_buf_size = 0;
        }
        else
        {
            m_data_buf_size -=size;
        }
    }
	cnet_msg * cnet_session::_next_new_net_msg(EMsgIDReserve socket_status)
	{
		if ((m_recv_size - m_recv_available_size) < MSG_HEADER_SIZE)
		{
			return NULL;
		}
		cnet_msg * msg_ptr = NULL;
		if (m_recv_size > MSG_HEADER_SIZE && m_recv_buf_ptr)
		{
			//数据包头信息
			cnet_msg_header m_net_header;
			memcpy(&m_net_header, m_recv_buf_ptr, MSG_HEADER_SIZE);

			//有可能是非法客户端数据
			if (m_net_header.m_size > m_recv_size)
			{
				m_recv_available_size = m_recv_size;
                WARNING_LOG("recv header size too big tail size = %d, msg_id = %d, recv_size = %u,  server_name = %s", m_net_header.m_size, m_net_header.m_msg_id, m_recv_size, m_net_ptr.get_name().c_str());
				m_net_ptr.close(m_client_session); // TODO@2020-05-16 chensong  临时这样关闭socket  调试代码要注释该行
				return NULL;
			}
			else if (m_net_header.m_size < 0)
			{ //非  
				m_recv_available_size = m_recv_size;
                WARNING_LOG("recv header size  packpage too small size %d, msg_id = %d, server_name = %s", m_net_header.m_size, m_net_header.m_msg_id, m_net_ptr.get_name().c_str());
				m_net_ptr.close(m_client_session);
				return NULL;
			}
			//数据包接收的不完整
			if (m_net_header.m_size > static_cast<int32>(m_recv_size - m_recv_available_size - MSG_HEADER_SIZE))
			{
				return NULL;
			}
			msg_ptr = new cnet_msg();
			if (!msg_ptr)
			{
				return NULL;
			}

			if (m_net_header.m_size)
			{
				if (!msg_ptr->alloc_me(m_net_header.m_size/*m_recv_size - m_recv_available_size*/))
				{
					delete msg_ptr;
					return NULL;
				}
			}
			msg_ptr->set_session_id(m_client_session);
			msg_ptr->set_status(socket_status);
			msg_ptr->set_msg_id(m_net_header.m_msg_id);

			if (m_net_header.m_size > 0)
			{
				memcpy(msg_ptr->get_buf(), m_recv_buf_ptr + MSG_HEADER_SIZE, m_net_header.m_size/*(m_recv_size - m_recv_available_size)*/);
			}
			//一个包体的大小 
			int64 temp_recv_available_size = m_recv_available_size +MSG_HEADER_SIZE + m_net_header.m_size;
			//是否已经到内存最后了
			if ((m_recv_size - temp_recv_available_size) > 0)
			{
				// 把还有没有读取包放到recv_buf中的前面   修改可使用的内存的大小
				memmove(m_recv_buf_ptr , m_recv_buf_ptr + MSG_HEADER_SIZE + m_net_header.m_size, (m_recv_size - temp_recv_available_size));

				m_recv_available_size = temp_recv_available_size;
			}
			else
			{
				//到了内存最后 了
				m_recv_available_size = m_recv_size;
			}
			return msg_ptr;

		}
		else
		{
			return NULL;
		}
		return msg_ptr;
	}
	
	bool cnet_session::send_data(const void * p, int64 size)
	{
		clock_guard			guard(m_send_mutex);
		if (m_send_size - m_data_buf_size < size)
        {
		    ERROR_EX_LOG("send data size too big ailable_size = %u, size =%u", (m_send_size - m_data_buf_size), size);
		    return false;
        }
		//lock
		if (!memcpy(m_data_buf_ptr + m_data_buf_size, p, size))
		{
			WARNING_LOG("send data memcpy fail");
			return false;
		}
		m_data_buf_size += size;
		return true;
	}
	void cnet_session::clearup()
	{
		m_status = ESS_INIT;
		csocket_ops::close_socket(m_sockfd);
		/*if ((m_sockfd & 0XFFFF0000) == 0XFFFF0000)
		{
			m_sockfd += 0X00020000;
		}
		else
		{
			m_sockfd += 0X00010000;
		}
		*/
	}
	void cnet_session::set_connect(socket_type sockfd)
	{
		m_status = ESS_OPEN;
		m_sockfd = sockfd;
	}
	
	void 	cnet_session::destroy()
	{
		// send -> data
//		SYSTEM_LOG(" session destroy  status = %d, sockfd =%d", m_status, m_sockfd);
		if (is_open())
		{
			if (m_send_size - m_send_available_size > 0)
			{
				// send -> 
				processoutputs();
			}
			//SYSTEM_LOG(" close client socket fd");
			csocket_ops::close_socket(m_sockfd);
		}
		if (m_recv_buf_data_ptr)
		{
			delete[] m_recv_buf_data_ptr;
		}
		if (m_sending_buf_ptr)
		{
			delete[] m_sending_buf_ptr;
		}
		if (m_data_buf_ptr)
		{
			delete[] m_data_buf_ptr;
		}
		if (m_recv_buf_ptr)
		{
			delete[] m_recv_buf_ptr;
		}
	}
    bool       cnet_session::_send_buf_move_sending_buf()
    {
        //判断发送缓冲区是否还有空间发送数据
        if (m_send_available_size< m_data_buf_size)
        {
            //发送缓存区的内存没有 内存可以使用的了

            if ((m_send_size - m_send_available_size) > 0)
            {
                int32 temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr, (m_send_size - m_send_available_size), 0);
                //已经发送的数据块的大小
                int32 send_size = temp_send_size;
                while ((m_send_available_size + temp_send_size) < m_send_size)
                {
                    m_send_available_size += temp_send_size;
                    send_size += temp_send_size;
                    temp_send_size = csocket_ops::async_send(m_sockfd, m_sending_buf_ptr + send_size, (m_send_size - m_send_available_size)/*m_send_size*/, 0);
                }
                m_send_available_size = m_send_size;
            }
            return false;
        }
        else
        {
            if (m_data_buf_ptr && m_data_buf_size > 0)
            {
                memcpy(m_sending_buf_ptr +(m_send_size - m_send_available_size) , m_data_buf_ptr, m_data_buf_size);

                m_send_available_size -= m_data_buf_size;
                m_data_buf_size = 0;
            }
            return true;
        }
        return true;
    }
	
}  // chen
