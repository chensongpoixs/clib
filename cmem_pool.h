/***********************************************************************************************
				created: 		2019-05-12

				author:			chensong

				purpose:		cmem_pool
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

#ifndef _C_MEM_POOL_H_
#define _C_MEM_POOL_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include <mutex>
namespace chen {
	
//	class cmem_block;
//	class cmem_pool
//	{
//	public:
//		cmem_pool();
//		~cmem_pool();
//	public:
//		bool init(int32 block_size = 1024, int32 node_size = 1024 );
//		void * alloc(int32 size);
//		void free(void const * ptr);
//		void destroy();
//	public:
//		void show_data();
//	private:
////		int64					m_use_size;
//		cmem_block			*	m_penter_ptr;
//		cmem_block			*	m_next_ptr;
//	};
//
//
//	inline int32 size2index(uint32 nSize, uint32 & realSize)
//	{
//		if (nSize <= 32) { realSize = 32;			return 0; }
//		if (nSize <= 64) { realSize = 64;			return 1; }
//		if (nSize <= 128) { realSize = 128;			return 2; }
//		if (nSize <= 256) { realSize = 256;			return 3; }
//		if (nSize <= 512) { realSize = 512;			return 4; }
//		if (nSize <= 1024) { realSize = 1024;			return 5; }
//		if (nSize <= 2 * 1024) { realSize = 2 * 1024;		return 6; }
//		if (nSize <= 4 * 1024) { realSize = 4 * 1024;		return 7; }
//		if (nSize <= 8 * 1024) { realSize = 8 * 1024;		return 8; }
//		if (nSize <= 16 * 1024) { realSize = 16 * 1024;		return 9; }
//		if (nSize <= 32 * 1024) { realSize = 32 * 1024;		return 10; }
//		if (nSize <= 64 * 1024) { realSize = 64 * 1024;		return 11; }
//		if (nSize <= 128 * 1024) { realSize = 128 * 1024;	return 12; }
//		if (nSize <= 256 * 1024) { realSize = 256 * 1024;	return 13; }
//		if (nSize <= 512 * 1024) { realSize = 512 * 1024;	return 14; }
//		if (nSize <= 1024 * 1024) { realSize = 1024 * 1024;	return 15; }
//		realSize = nSize;
//		return -1;
//	}
}//namespace chen

#endif // !#define _C_MEM_POOL_H_