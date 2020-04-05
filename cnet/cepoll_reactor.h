/***********************************************************************************************
	created: 		2019-02-24
	
	author:			chensong
					
	purpose:		epoll_reactor

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
	
	later tasks:	null
	
	   The struct epoll_event is defined as :

           typedef union epoll_data {
               void    *ptr;
               int      fd;
               uint32_t u32;
               uint64_t u64;
           } epoll_data_t;

           struct epoll_event {
               uint32_t     events;    // Epoll events 
               epoll_data_t data;      // User data variable 
           };
************************************************************************************************/
#ifndef _C_EPOLL_REACTOR_H_
#define _C_EPOLL_REACTOR_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include <vector>



#if defined(__GNUC__)
#include <sys/epoll.h>
# include <sys/timerfd.h>
namespace chen 
{
	
	
	class cepoll_reactor :public cnoncopyable 
	{
	public:
	    explicit	cepoll_reactor(): m_epoll_fd_(-1), m_curfd_count(0), m_maxfd_count(0), m_active(NULL), m_stoped(false){}
		virtual ~cepoll_reactor() {}
		
		bool init(socket_type listenfds, uint32 active_num);
		void destroy();
		
	public:
		
		int32 select(uint32 ms);
	public:

		void * get_para(int64 index) {return m_active[index].data.ptr;}

	public:
		bool	is_read(uint32 index) const;
		bool	is_write(uint32 index)const;
		bool	is_exception(uint32 index) const;
	public:

		/*	bool 	descriptor_read_state(int64 index) {return m_active[index].event | EPOLLIN;}

			int32 register_descriptor(socket_type descriptor, cnet_session *& session);
			void deregister_descriptor(socket_type descriptor);
			socket_type		get_event_descriptor(int32 index) {return m_active[index].data->fd;}*/
			// register
		void	register_read_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, EPOLLIN, para); }
		void	register_write_descriptor(socket_type &descriptor, void* para) { _register_descriptor(descriptor, EPOLLOUT, para); }
		void	register_readwrite_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, EPOLLIN | EPOLLOUT, para); }

		void deregister_descriptor(socket_type &descriptor);
	private:
		int32 _register_descriptor(socket_type &descriptor, uint32 st, void * session);
	private:
		// The epoll file descriptor.
		socket_type					m_epoll_fd_;
//		socket_type 				m_maxfd;
		uint32						m_curfd_count;
		uint32						m_maxfd_count;
		epoll_event* 				m_active;
		bool 						m_stoped;
		
	};
} // namespace chen
#endif //!#if defined(__GNUC__)

#endif // !#define _C_EPOLL_REACTOR_H_
