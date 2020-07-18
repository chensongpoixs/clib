/***********************************************************************************************
	created: 		2019-02-26
	
	author:			chensong
					
	purpose:		queue
	
	Copyright boost
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

#ifndef _C_QUEUE_H_
#define _C_QUEUE_H_
#include "cnoncopyable.h"

namespace chen {

//template <typename operation>
//class cqueue;
//
//class cqueue_access
//{
//public:
//  template <typename Operation>
//  static Operation* next(Operation* o)
//  {
//    return static_cast<Operation*>(o->next_);
//  }
//
//  template <typename Operation1, typename Operation2>
//  static void next(Operation1*& o1, Operation2* o2)
//  {
//    o1->next_ = o2;
//  }
//
//  template <typename Operation>
//  static void destroy(Operation* o)
//  {
//    o->destroy();
//  }
//
//  template <typename Operation>
//  static Operation*& front(cqueue<Operation>& q)
//  {
//    return q.front_;
//  }
//
//  template <typename Operation>
//  static Operation*& back(cqueue<Operation>& q)
//  {
//    return q.back_;
//  }
//};
//
//template <typename Operation>
//class cqueue
//  : private cnoncopyable
//{
//public:
//  // Constructor.
//  cqueue()
//    : front_(0),
//      back_(0)
//  {
//  }
//
//  // Destructor destroys all operations.
//  ~cqueue()
//  {
//    while (Operation* op = front_)
//    {
//      pop();
//      cqueue_access::destroy(op);
//    }
//  }
//
//  // Get the operation at the front of the queue.
//  Operation* front()
//  {
//    return front_;
//  }
//
//  // Pop an operation from the front of the queue.
//  void pop()
//  {
//    if (front_)
//    {
//      Operation* tmp = front_;
//      front_ = cqueue_access::next(front_);
//      if (front_ == 0)
//	  {
//		   back_ = 0;
//	  }
//      cqueue_access::next(tmp, static_cast<Operation*>(0));
//    }
//  }
//
//  // Push an operation on to the back of the queue.
//  void push(Operation* h)
//  {
//    cqueue_access::next(h, static_cast<Operation*>(0)); 
//    if (back_)
//    {
//      cqueue_access::next(back_, h);
//      back_ = h;
//    }
//    else
//    {
//      front_ = back_ = h;
//    }
//  }
//
//  // Push all operations from another queue on to the back of the queue. The
//  // source queue may contain operations of a derived type.
//  template <typename OtherOperation>
//  void push(cqueue<OtherOperation>& q)
//  {
//    if (Operation* other_front = cqueue_access::front(q))
//    {
//      if (back_)
//	  {
//		   cqueue_access::next(back_, other_front);
//	  }
//      else
//	  {
//		   front_ = other_front;
//	  }
//      back_ = cqueue_access::back(q);
//      cqueue_access::front(q) = 0;
//      cqueue_access::back(q) = 0;
//    }
//  }
//
//  // Whether the queue is empty.
//  bool empty() const
//  {
//    return front_ == 0;
//  }
//
//private:
//  friend class cqueue_access;
//
//  // The front of the queue.
//  Operation* front_;
//
//  // The back of the queue.
//  Operation* back_;
//};

} // namespace chen



#endif // _C_QUEUE_H_
