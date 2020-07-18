/***********************************************************************************************
created: 		2019-12-27

author:			chensong

purpose:		cmsg_buffer
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
#include "cmsg_buffer.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
namespace chen {
	cmsg_send_buffer::cmsg_send_buffer(uint32 max_size)
		: m_max_size(max_size)
		, m_size(0)
		, m_send_size(0)
	{
	}
	cmsg_send_buffer::~cmsg_send_buffer()
	{
	}
	cmsg_send_buffer * cmsg_send_buffer::alloc_me(uint32 size)
	{
		void * p = ::malloc(sizeof(cmsg_send_buffer)- 1 + size);
		if (!p)
		{
			return NULL;
		}
		cmsg_send_buffer * q = new(p) cmsg_send_buffer(size);
		
		return q;
	}
	void cmsg_send_buffer::free_me(cmsg_send_buffer * ptr)
	{
		::free(ptr);
	}
	uint32 cmsg_send_buffer::append(const void * p, uint32 len)
	{
		if (len > avail())
		{
			len = avail();
		}
		memcpy(m_buffer + m_size, p, len);
		m_size += len;
		return len;
	}
	void cmsg_send_buffer::reset()
	{
		m_size = 0;
		m_send_size = 0;
	}
	/*void cmsg_send_buffer::resert()
	{
	}*/
	cmsg_receive_buffer::cmsg_receive_buffer(uint32 size)
		: m_size(size)
		, m_data_size(0)
		/*, m_send_size(0)*/
	{
	}
	cmsg_receive_buffer::~cmsg_receive_buffer()
	{
	}
	cmsg_receive_buffer * cmsg_receive_buffer::alloc_me(uint32 size)
	{
		void * p = ::malloc(sizeof(cmsg_receive_buffer) - 1 + size);
		if (!p)
		{
			return NULL;
		}
		cmsg_receive_buffer * q = new(p) cmsg_receive_buffer(size);
		return q;
	}
	void cmsg_receive_buffer::free_me(cmsg_receive_buffer * ptr)
	{
		::free(ptr);
	}
	uint32 cmsg_receive_buffer::append(void * p, uint32 len)
	{
		if (len > avail())
		{
			len = avail();
		}
		memcpy(m_buffer + m_data_size, p, len);
		m_data_size += len;
		return len;
	}
	void cmsg_receive_buffer::resert()
	{
		m_data_size = 0;
		/*m_send_size = 0;*/
	}
} // !namespace chen

