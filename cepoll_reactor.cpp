/***********************************************************************************************
					created: 		2019-02-24

					author:			chensong

					purpose:		epoll_reactor

					later tasks:	null
	�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧������������ֻᱻ��������ں���������ĳ�������ҹ��������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶����ߡ�
�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
************************************************************************************************/

#include "cepoll_reactor.h"
#include "cobject_pool.h"
#include "cscoped_lock.h"
#include "csocket_ops.h"
//#include ""
#include "clog.h"

#if defined(__linux__)
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
		if (m_active)
		{
			delete[] m_active;
			m_active = NULL;
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
	 	/*if (m_curfd_count == 0)
		{
			return 0;
		}*/
		//int32 num_events = epoll_wait(m_epoll_fd_, m_active, m_maxfd_count, ms);
		//ERROR_EX_LOG(" num_events =%d", num_events);
		/*if (num_events > m_maxfd_count )
		{
			if (errno == EINTR)
			{
				return 0;
			}
		//	ERROR_EX_LOG("select failed, num_events=%d, errno=%d", num_events, errno);
			return 0;
		}*/
		 struct timespec time_out;
        time_out.tv_sec = 0;
        time_out.tv_nsec = ms * 10000;
		
		int32 num_events = epoll_wait(m_epoll_fd_, m_active, m_maxfd_count, ms);
		
		return num_events;
	}
	bool cepoll_reactor::is_read(uint32 index) const
	{
		return  (m_active[index].events & EPOLLIN);
	}
	bool cepoll_reactor::is_write(uint32 index) const
	{ //EVFILT_EXCEPT
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
	void	cepoll_reactor::register_write_descriptor(socket_type &descriptor, void* para)
	{
		if (descriptor == -1)
		{
			WARNING_EX_LOG(" register descriptor write failed");
			return ;
		}
		epoll_event ev = { 0, { 0 } };
		ev.events = EPOLLIN | EPOLLERR | EPOLLET | EPOLLOUT;
		ev.data.ptr = para;
		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_MOD, descriptor, &ev) ) 
		{
			  ERROR_EX_LOG("register_write_descriptor...epoll_ctl...ADD... descriptor = %d, error =%d", descriptor, errno);
            return;
		}
	}
	void    cepoll_reactor::deregiter_write_descriptor(socket_type& descriptor,  void* para)
	{
		if (descriptor == -1)
		{
			ERROR_EX_LOG("ERROR deregister_descriptor write  fd (socket_type=%d)", descriptor);
			return;
		}
		 epoll_event ev = { 0, { 0 } };
		ev.events = EPOLLIN | EPOLLERR | EPOLLET;
		ev.data.ptr = para;
		if (0 != ::epoll_ctl(m_epoll_fd_, EPOLL_CTL_MOD, descriptor, &ev) ) 
		{
			  ERROR_EX_LOG("deregiter_write_descriptor...epoll_ctl...DEL... descriptor = %d, error =%d", descriptor, errno);
            return;
		}
	}

	int32 cepoll_reactor::_register_descriptor(socket_type& descriptor, uint32 st, void * session)
	{
		if (descriptor == -1)
		{
			WARNING_EX_LOG(" register descriptor failed");
			return 0;
		}
		if (m_curfd_count > m_maxfd_count)
		{
			ERROR_EX_LOG("reach max fd count %lu", m_maxfd_count);
			return -1;
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

#endif //#if defined(__linux__)
