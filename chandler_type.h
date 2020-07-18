/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		chandler_type
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


#ifndef _C_HANDLER_TYPE_
#define _C_HANDLER_TYPE_

namespace chen {
	
		/// Default handler type traits provided for all handlers.
		/**
		* The handler_type traits class is used for determining the concrete handler
		* type to be used for an asynchronous operation. It allows the handler type to
		* be determined at the point where the specific completion handler signature
		* is known.
		*
		* This template may be specialised for user-defined handler types.
		*/
		template <typename Handler, typename Signature>
		struct handler_type
		{
			/// The handler type for the specific signature.
			typedef Handler type;
		};

#if !defined(GENERATING_DOCUMENTATION)

		template <typename Handler, typename Signature>
		struct handler_type<const Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<volatile Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const volatile Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<volatile Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const volatile Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<Handler&, Signature>
			: handler_type<Handler, Signature> {};

#if defined(CHEN_ASIO_HAS_MOVE)
		template <typename Handler, typename Signature>
		struct handler_type<Handler&&, Signature>
			: handler_type<Handler, Signature> {};
#endif // defined(CHEN_ASIO_HAS_MOVE)

		template <typename ReturnType, typename Signature>
		struct handler_type<ReturnType(), Signature>
			: handler_type<ReturnType(*)(), Signature> {};

		template <typename ReturnType, typename Arg1, typename Signature>
		struct handler_type<ReturnType(Arg1), Signature>
			: handler_type<ReturnType(*)(Arg1), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Signature>
		struct handler_type<ReturnType(Arg1, Arg2), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Arg4, typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3, Arg4), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3, Arg4), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Arg4, typename Arg5, typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3, Arg4, Arg5), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3, Arg4, Arg5), Signature> {};

#endif // !defined(GENERATING_DOCUMENTATION)

	
} // namespace chen


#define CHNE_ASIO_HANDLER_TYPE(h, sig) \
  typename handler_type<h, sig>::type

#endif // _C_HANDLER_TYPE_
