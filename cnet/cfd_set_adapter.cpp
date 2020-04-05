/***********************************************************************************************
created: 		2019-06-16

author:			chensong

purpose:		fd_set_adapter
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
#include "cfd_set_adapter.h"
#include "csocket_ops.h"
#include "clog.h"
#if defined(_MSC_VER)
namespace chen {
	cfd_set_adapter::cfd_set_adapter()
		: m_fd_set(NULL)
		, m_capacity(default_fd_set_size)
		, m_max_descriptor_fd(0)
	{
	}
	cfd_set_adapter::~cfd_set_adapter()
	{
	}
	bool cfd_set_adapter::init()
	{
		m_fd_set = static_cast<_fd_set*>(::operator new(
			sizeof(_fd_set) - sizeof(socket_type)
			+ sizeof(socket_type) * (m_capacity)));
		m_fd_set->m_count = 0;
		
		return true;
	}
	void cfd_set_adapter::destroy()
	{
		::operator delete(m_fd_set);
		m_max_descriptor_fd = 0;
		m_capacity = 0;
	}
	bool cfd_set_adapter::set(socket_type descriptor)
	{
		//ERROR_LOG("count = %lu", m_fd_set.m_count);
		for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			if (m_fd_set->m_fd_array[i] == descriptor)
			{
				return true;
			}
		}

		reserve(m_fd_set->m_count + 1);
		m_fd_set->m_fd_array[m_fd_set->m_count++] = descriptor;
	/*	for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			if (m_fd_set.m_fd_array[i] == descriptor)
			{
				return true;
			}
		}
		if (m_fd_set.m_count < default_fd_set_size)
		{
			m_fd_set.m_fd_array[m_fd_set.m_count++] = descriptor;
			return true;
		}*/
		return true;
	}
	bool cfd_set_adapter::is_set(socket_type descriptor) const
	{
		//int __stdcall _WSAFDIsSet(SOCKET fd, fd_set *a2)
		//{
		//	int result; // eax@1 
		//	u_int v3; // ecx@1 
		//	char *v4; // edx@2 

		//	v3 = a2->fd_count;
		//	result = 0;
		//	if (a2->fd_count)
		//	{
		//		v4 = (char *)&a2->fd_array[v3];
		//		do
		//		{
		//			v4 -= 4;
		//			if (*(_DWORD *)v4 == fd)
		//				result = 1;
		//			--v3;
		//		} while (v3);
		//	}
		//	return result;
		//}
		return !!__WSAFDIsSet(descriptor,
			const_cast<fd_set*>(reinterpret_cast<const fd_set*>(m_fd_set)));
	}
	
	cfd_set_adapter::operator fd_set*()
	{
		return reinterpret_cast<fd_set*>(m_fd_set);
	}

	void cfd_set_adapter::reset()
	{
		m_fd_set->m_count = 0;
		m_max_descriptor_fd = 0;
	}

	void cfd_set_adapter::reserve(uint32 size)
	{
		
		if (size <= m_capacity)
		{
			return;
		}
		// 动态扩容
		uint32 new_capacity = m_capacity + m_capacity / 2;
		if (new_capacity < size)
		{
			new_capacity = size;
		}

		_fd_set* new_fd_set = static_cast<_fd_set*>(::operator new(
			sizeof(_fd_set) - sizeof(socket_type)
			+ sizeof(socket_type) * (new_capacity)));

		new_fd_set->m_count = m_fd_set->m_count;
		for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			new_fd_set->m_fd_array[i] = m_fd_set->m_fd_array[i];
		}

		::operator delete(m_fd_set);
		m_fd_set = new_fd_set;
		m_capacity = new_capacity;
	}
} // namespace chen
#endif // #if defined(_MSC_VER)