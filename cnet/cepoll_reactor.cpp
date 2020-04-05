/***********************************************************************************************
					created: 		2019-02-24

					author:			chensong

					purpose:		epoll_reactor

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

#include "cepoll_reactor.h"
#include "cobject_pool.h"
#include "cscoped_lock.h"
#include "csocket_ops.h"
//#include ""
#include "clog.h"

#if defined(__GNUC__)
namespace chen
{
	bool cepoll_reactor::init(socket_type listenfds, uint32 active_num)
	{
		if (active_num == 0 || active_num > 0xFFFF)
		{
			ERROR_EX_LOG(" max fd you not active_num =%lu", active_num);
			return false;
		}

		if (m_active == NULL)
		{
			m_active = new epoll_event[active_num];
			if (!m_active)
			{
				ERROR_EX_LOG(" alloc epoll active eror active_num =%lu", active_num);
				return false;
			}
		}

		if (m_epoll_fd_ == -1)
		{
			m_epoll_fd_ = ::epoll_create(active_num);
			if (m_epoll_fd_ == -1)
			{
				ERROR_EX_LOG(" epoll_create alloc ");
				return false;
			}
		}

		m_curfd_count = 0;
		m_maxfd_count = active_num;
	//	m_maxfd = listenfds;
		// add epoll 
		{
//csocket_ops::set_nonblocking(listenfds);
//		register_read_descriptor(listenfds, NULL);
			
//			register_readwrite_descriptor(m_epoll_fd_, NULL);

			//epoll_event ev = { 0, { 0 } };
			//ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			
	//		::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, listenfds, &ev);
			//csocket_ops::set_nonblocking(listenfds);

			//epoll_event ev = { 0, { 0 } };
			//ev.events = EPOLLIN | EPOLLERR | EPOLLET;
			////ev.data.ptr = listenfds;
			//::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, listenfds, &ev);
		}

		return true;
	}

	void cepoll_reactor::destroy()
	{
		if (m_epoll_fd_ != -1)
		{
			csocket_ops::close_socket(m_epoll_fd_);
		}
		/*
		if (m_timer_fd_ != -1)
		{
			::close(m_timer_fd_);
		}
		*/
	}

	int32 cepoll_reactor::select(uint32 ms)
	{
		if (m_curfd_count == 0)
		{
			return 0;
		}
		int32 num_events = epoll_wait(m_epoll_fd_, m_active, m_maxfd_count, ms);
		//ERROR_EX_LOG(" num_events =%d", num_events);
		if (num_events > m_maxfd_count )
		{
			if (errno == EINTR)
			{
				return 0;
			}
		//	ERROR_EX_LOG("select failed, num_events=%d, errno=%d", num_events, errno);
			return 0;
		}
		//epoll_event events[128];
		//m_active.clear();
		//int64 num_events = epoll_wait(epoll_fd_, events, 128, ms);
		//for (int64 i = 0; i < num_events; ++i)
		{
			//m_active.emplace_back(&events[i]);  // core addr 
		}
		return num_events;
	}
	bool cepoll_reactor::is_read(uint32 index) const
	{
		return  (m_active[index].events & EPOLLIN);
	}
	bool cepoll_reactor::is_write(uint32 index) const
	{
		return  (m_active[index].events & EPOLLOUT);
	}
	bool cepoll_reactor::is_exception(uint32 index) const
	{
		return (m_active[index].events &(EPOLLERR | EPOLLHUP));
	}
	/*	void cepoll_reactor::showdown()
		{

		}
		*/
	//int32  cepoll_reactor::register_descriptor(socket_type descriptor, cnet_session *& session)
	//{



	//	epoll_event ev = { 0, { 0 } };
	//	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
	//	session->set_event(ev.events);
	//	ev.data.ptr = session;
	//	int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
	//	if (result != 0)
	//	{
	//		return errno;
	//	}
	//	return 0;
	//}
	void cepoll_reactor::deregister_descriptor(socket_type& descriptor)
	{
		if (descriptor == -1)
		{
			ERROR_EX_LOG("ERROR deregister_descriptor fd (socket_type=%d)", descriptor);
			return;
		}

		struct epoll_event ev;
		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev))
		{
			ERROR_EX_LOG("deregister_descriptor...epoll_ctl...DEL...");
			return;
		}
		--m_curfd_count;
	}

	int32 cepoll_reactor::_register_descriptor(socket_type& descriptor, uint32 st, void * session)
	{
		if (descriptor == -1)
		{
			WARNING_EX_LOG(" register descriptor failed");
			return 0;
		}
		struct epoll_event ev;

		ev.data.ptr = session;
		ev.events = EPOLLET;
		if ((st & EPOLLIN) != 0)
		{
			ev.events |= EPOLLIN;
		}
		if ((st & EPOLLOUT) != 0)
		{
			ev.events |= EPOLLOUT;
		}

		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev))
		{
			ERROR_EX_LOG("epoll_ctl failed error =%d", errno);
			return -1;
		}
		++m_curfd_count;
		return 0;
	}

	/*socket_type cepoll_reactor::do_epoll_create()
	{

		socket_type fd = ::epoll_create(epoll_size);
		if (fd != -1)
		{
			::fcntl(fd, F_SETFD, FD_CLOEXEC);
		}
		if (fd == -1)
		{
			ERRROR_EX_LOG(" epoll_create alloc" );
			return -1;
		}
		return fd;
	}*/



} //namespace chen

#endif //#if defined(__GNUC__)
