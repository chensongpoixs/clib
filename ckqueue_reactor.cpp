/***********************************************************************************************
created: 		2019-10-27

author:			chensong

purpose:		ckqueue_reactor

later tasks:	null
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
#include "ckqueue_reactor.h"

#include "clog.h"
#if defined(__APPLE__)

#include "csocket_ops.h"
namespace chen {
	ckqueue_reactor::ckqueue_reactor()
	    : m_kqueue_fd(-1)
	    , m_curfd_count(0)
	    , m_maxfd_count(0)
	    , m_active(NULL)
	    , m_stoped(false)
	{
	}


	ckqueue_reactor::~ckqueue_reactor()
	{
	}
	bool ckqueue_reactor::init(socket_type listenfds, uint32 active_num)
	{

		if (active_num == 0 || active_num > 0xFFFF)
		{
			ERROR_EX_LOG(" max fd you not active_num =%lu", active_num);
			return false;
		}

		if (m_active == NULL)
		{
			m_active = new struct kevent[active_num];
			if (!m_active)
			{
				ERROR_EX_LOG(" alloc mac struct kevent active eror active_num =%lu", active_num);
				return false;
			}
		}

		if (m_kqueue_fd == -1)
		{
			m_kqueue_fd = ::kqueue();
			if (m_kqueue_fd == -1)
			{
				ERROR_EX_LOG(" kqueue init fd failed ");
				return false;
			}
		}

		m_curfd_count = 0;
		m_maxfd_count = active_num;
		return true;
	}
	void ckqueue_reactor::destroy()
	{
		if (m_kqueue_fd != -1)
		{
			csocket_ops::close_socket(m_kqueue_fd);
		}
		if (m_active)
		{
			delete[] m_active;
			m_active = NULL;
		}
	}
	void * ckqueue_reactor::get_para(uint32 index)
	{
		return m_active[index].udata;
	}
	bool ckqueue_reactor::is_read(uint32 index) const
	{
		return (m_active[index].fflags & E_READFDS);
	}
	bool ckqueue_reactor::is_write(uint32 index) const
	{
		return (m_active[index].fflags & E_WRITFDS);
	}
	bool ckqueue_reactor::is_exception(uint32 index) const
	{
		return (m_active[index].fflags & E_EXCEFDS);
	}
	void ckqueue_reactor::deregister_descriptor(socket_type & descriptor)
	{
        if (descriptor == -1)
        {
            ERROR_EX_LOG("ERROR deregister_descriptor fd (socket_type=%d)", descriptor);
            return;
        }

        NORMAL_LOG("deregister read write session_id = %u", descriptor);

        struct kevent ev[2];
        EV_SET(&ev[0], descriptor, EVFILT_READ, EV_DELETE, 0, 0, (void*)(intptr_t)descriptor);
        EV_SET(&ev[1], descriptor, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(intptr_t)descriptor);
        ::kevent(m_kqueue_fd, ev, 2, NULL, 0, NULL);
        --m_curfd_count;
	}
    void	ckqueue_reactor::register_write_descriptor(socket_type& descriptor, void* para)
    {
        if (descriptor == -1)
        {
            ERROR_EX_LOG("ERROR register_write_descriptor fd (socket_type=%d)", descriptor);
            return;
        }

       // NORMAL_LOG("register write session_id = %u", descriptor);
        struct kevent ev;
       // ev.udata = para;
        EV_SET(&ev, descriptor, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, para);
        ::kevent(m_kqueue_fd, &ev, 1, NULL, 0, NULL);
        if (::kevent(m_kqueue_fd, &ev, 1, NULL, 0, NULL) == -1) {
            ERROR_EX_LOG("register_write_descriptor...kevent...ADD... descriptor = %d, error =%d", descriptor, errno);
            return;
        }
    }
    void    ckqueue_reactor::deregiter_write_descriptor(socket_type& descriptor, void* para)
    {
        if (descriptor == -1)
        {
            ERROR_EX_LOG("ERROR deregiter_write_descriptor fd (socket_type=%d)", descriptor);
            return;
        }
       // NORMAL_LOG("deregiter write session_id = %u", descriptor);
        struct kevent ev;
        EV_SET(&ev, descriptor, EVFILT_WRITE, EV_DELETE, 0, 0, para);
        if ( ::kevent(m_kqueue_fd, &ev, 1, NULL, 0, NULL) == -1) {
            ERROR_EX_LOG("deregiter_write_descriptor...kevent...DEL... descriptor = %d, error =%d", descriptor, errno);
            return;
        }
    }

	int32 ckqueue_reactor::select(uint32 ms)
	{
       /* if (m_curfd_count == 0)
        {
            return 0;
        }*/
        struct timespec time_out;
        time_out.tv_sec = 0;
        time_out.tv_nsec = ms * 10000;
        struct kevent events[128];
        // 交给硬件来触发 read write
        int32 retval = ::kevent(m_kqueue_fd, NULL, 0, events, 128, &time_out);
        uint32 event = 0;
        int32 num_event = 0;
        if (retval > 0)
        {
            for (int32  i = 0;  i< retval; ++i)
            {
                event = 0;

                // TODO@chensong 2020-07-14 可能没有注册write file  descriptor
//                if(events[i].filter == EVFILT_WRITE   )
//                {}
                /*if (m_active_paras[i].events == E_NONE)
                {
                    continue;
                }*/
//                NORMAL_LOG("i = %u, filter = %d", i, events[i].filter);
                if (events[i].filter == EVFILT_READ/*m_active_paras[i].events & E_READFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &readfds)*/ /*read_fd_ptr->fd_array[i] == m_paras[i].socket*/)
                {
                   // NORMAL_LOG("count = %d, read m_curfd_count = %ld", i, m_curfd_count);
                    event |= E_READFDS;
                }
                if (events[i].filter == EVFILT_WRITE/*m_active_paras[i].events & E_WRITFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &writefds)*/ /*write_fd_ptr->fd_array[i] == m_paras[i].socket*/)
                {
                   // NORMAL_LOG("count = %d, write m_curfd_count = %ld", i, m_curfd_count);
                    event |= E_WRITFDS;
                }
                // 参考 redis中网络io 删除异常文件描述符
//                if (events[i].filter & EVFILT_EXCEPT/*m_active_paras[i].events & E_EXCEFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &excefds)*/ /*exec_fd_ptr->fd_array[i] == m_paras[i].socket*/)
//                {
//                    ERROR_EX_LOG("count = %d, exce m_curfd_count = %ld", i, m_curfd_count);
//                    event |= EVFILT_EXCEPT;
//                }
                /*m_active_paras[i].socket = i;
                m_active_paras[i].events = events;*/
                if (0 != event )
                {
//                    NORMAL_LOG("i - =%d", i);
                    m_active[i].fflags = event;
                    m_active[i].udata = events[i].udata;
                    ++num_event;
                }
            }
        }
        return num_event;
	}
	int32 ckqueue_reactor::_register_descriptor(socket_type & descriptor, int32 st, void * session)
	{
        if (descriptor == -1)
        {
            WARNING_EX_LOG(" register descriptor failed");
            return 0;
        }
        struct kevent ev[3];

        if (st & E_READFDS)
        {
            NORMAL_LOG("register read session_id = %u", descriptor);
            EV_SET(&ev[0], descriptor, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, session);
            //把会话交给kevent管理
           // ev[0].udata = session;
            if (::kevent(m_kqueue_fd, &ev[0], 1, NULL, 0, NULL) == -1)
            {
                ERROR_EX_LOG("kevent add EVFILT_READ failed error =%d", errno);
                return -1;
            }
        }
        if (st & E_WRITFDS)
        {
//            NORMAL_LOG("register write session_id = %u", descriptor);
            EV_SET(&ev[1], descriptor, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, session);
            //把会话交给kevent管理
            ev[1].udata = session;
            if ( ::kevent(m_kqueue_fd, &ev[1], 1, NULL, 0, NULL) == -1)
            {
                ERROR_EX_LOG("kevent add EVFILT_WRITE failed error =%d", errno);
                return -1;
            }
        }
        ++m_curfd_count;
        return 0;
	}
} // !namespace chen
#endif //!#if defined(__APPLE__)