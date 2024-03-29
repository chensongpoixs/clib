﻿/***********************************************************************************************
	created: 		2019-03-06
	
	author:			chensong
					
	purpose:		acceptor
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
#ifndef _C_ACCEPTOR_H_
#define _C_ACCEPTOR_H_
#include "cnoncopyable.h"
#include "csocket_ops.h"
#include "cnet_types.h"
namespace chen {
	class cacceptor : public cnoncopyable
	{
	public:
		explicit cacceptor() :m_sockfd(-1), m_port(0) { m_ip.clear(); }
		~cacceptor() {}
	public:
		static cacceptor * construct();
		static void        destroy(cacceptor * p);
		bool 	init(const char * ip, uint32 port);
		void 	destroy();
	public: 
		// IPv4
		bool			create();
        //SO_RCVBUF	int	TCP接受缓冲区大小
        //SO_SNDBUF	int	TCP发送缓冲区大小
        //recv 和 send 返回筹码 10035
        bool			set_recvbuf_sendbuf(int64 recv_buff_size, int64 send_buff_size);
        //TCP / IP协议中针对TCP默认开启了Nagle算法。Nagle算法通过减少需要传输的数据包，来优化网络。在内核实现中，数据包的发送和接受会先做缓存，分别对应于写缓存和读缓存。
        //
        //	启动TCP_NODELAY，就意味着禁用了Nagle算法，允许小包的发送。对于延时敏感型，同时数据传输量比较小的应用，开启TCP_NODELAY选项无疑是一个正确的选择。比如，对于SSH会话，用户在远程敲击键盘发出指令的速度相对于网络带宽能力来说，绝对不是在一个量级上的，所以数据传输非常少；而又要求用户的输入能够及时获得返回，有较低的延时。如果开启了Nagle算法，就很可能出现频繁的延时，导致用户体验极差。当然，你也可以选择在应用层进行buffer，比如使用java中的buffered stream，尽可能地将大包写入到内核的写缓存进行发送；vectored I / O（writev接口）也是个不错的选择。
        //
        //	对于关闭TCP_NODELAY，则是应用了Nagle算法。数据只有在写缓存中累积到一定量之后，才会被发送出去，这样明显提高了网络利用率（实际传输数据payload与协议头的比例大大提高）。但是这由不可避免地增加了延时；与TCP delayed ack这个特性结合，这个问题会更加显著，延时基本在40ms左右。当然这个问题只有在连续进行两次写操作的时候，才会暴露出来。
        //
        //	连续进行多次对小数据包的写操作，然后进行读操作，本身就不是一个好的网络编程模式；在应用层就应该进行优化。
        //
        //
        //	对于既要求低延时，又有大量小数据传输，还同时想提高网络利用率的应用，大概只能用UDP自己在应用层来实现可靠性保证了。好像企鹅家就是这么干的。
        //
        //
        //
        //	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //
        //
        //
        //	TCP / IP协议中，无论发送多少数据，总是要在数据前面加上协议头，同时，对方接收到数据，也需要发送ACK表示确认。为了尽可能的利用网络带宽，TCP总是希望尽可能的发送足够大的数据。（一个连接会设置MSS参数，因此，TCP / IP希望每次都能够以MSS尺寸的数据块来发送数据）。
        //
        //	Nagle算法就是为了尽可能发送大块数据，避免网络中充斥着许多小数据块。
        //
        //	Nagle算法的基本定义是任意时刻，最多只能有一个未被确认的小段。 所谓“小段”，指的是小于MSS尺寸的数据块，所谓“未被确认”，是指一个数据块发送出去后，没有收到对方发送的ACK确认该数据已收到。
        //
        //	举个例子，比如之前的blog中的实验，一开始client端调用socket的write操作将一个int型数据(称为A块)写入到网络中，由于此时连接是空闲的（也就是说还没有未被确认的小段），因此这个int型数据会被马上发送到server端，接着，client端又调用write操作写入‘ / r / n’（简称B块），这个时候，A块的ACK没有返回，所以可以认为已经存在了一个未被确认的小段，所以B块没有立即被发送，一直等待A块的ACK收到（大概40ms之后），B块才被发送。整个过程如图所示：
        //
        //
        //	这里还隐藏了一个问题，就是A块数据的ACK为什么40ms之后才收到？这是因为TCP / IP中不仅仅有nagle算法，还有一个ACK延迟机制 。当Server端收到数据之后，它并不会马上向client端发送ACK，而是会将ACK的发送延迟一段时间（假设为t），它希望在t时间内server端会向client端发送应答数据，这样ACK就能够和应答数据一起发送，就像是应答数据捎带着ACK过去。在我之前的时间中，t大概就是40ms。这就解释了为什么'/r/n'(B块)总是在A块之后40ms才发出。
        //
        //	如果你觉着nagle算法太捣乱了，那么可以通过设置TCP_NODELAY将其禁用 。当然，更合理的方案还是应该使用一次大数据的写操作，而不是多次小数据的写操作。
        bool	set_nonagle(bool on = true);
        socket_type& 	get_sockfd();
	public:
	    // server
		//bool 			open();
		bool 			bind();
		bool		 	accept(socket_type& socket, struct sockaddr_in& sock_addr, int32& err_code);
		// 并发数
		bool 			listen();
	public:
	    // client
        bool		    async_connect(/*socket_type& 	sockfd*/);
	    bool            set_nosigpipe(socket_type & sockettype);
		// client sync connect  wait
		bool            sync_wait();




	private:
		void 			shutdown();
	private:
		socket_type 	m_sockfd; 
		std::string 	m_ip;		// ip
		uint32			m_port;    // port
	};
}// namespace chen

#endif // _C_ACCEPTOR_H_