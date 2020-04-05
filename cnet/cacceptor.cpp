/***********************************************************************************************
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
#include "cacceptor.h"
#include "clog.h"
#include "cnet_types.h"
#include <cerrno>
namespace chen {
	cacceptor * cacceptor::construct()
	{
		return new cacceptor();
	}
	void cacceptor::destroy(cacceptor * p)
	{
		delete p;
	}
	bool 	cacceptor::init(const char * ip, uint32 port)
	{
		if (!ip )
		{
			ERROR_EX_LOG("ip not");
			return false;
		}
		if (port <= 0)
		{
			ERROR_EX_LOG("port not ");
			return false;
		}
		m_ip = ip;
		m_port = port;
		return true;
	}
	bool	cacceptor::create()
	{   
		m_sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd == INVALID_SOCKET)
		{
			ERROR_EX_LOG("create socket ");
			return false;
		}
		// nlonl
		if (!csocket_ops::set_nonblocking(m_sockfd))
		{
			ERROR_EX_LOG("set create socket nonblocking ");
			return false;
		}
		return true;
	}
	bool cacceptor::async_connect(/*socket_type& 	sockfd*/)
	{
		//sockfd = csocket_ops::socket(AF_INET, SOCK_STREAM, 0);
		//if (sockfd == INVALID_SOCKET)
		//{
		//	ERROR_EX_LOG("create socket ");
		//	return false;
		//}
		//// nlonl
		//if (!csocket_ops::set_nonblocking(sockfd))
		//{
		//	ERROR_EX_LOG("set create socket nonblocking ");
		//	return false;
		//}
		struct sockaddr_in addrSrv;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(m_port);
		addrSrv.sin_addr.s_addr = ::inet_addr(m_ip.c_str());  //要连接的服务器的ip地址


		/*SOCKET listenfd, maxfd;
		listenfd = ::socket(AF_INET, SOCK_STREAM, 0);      */              //建立客户端流式套接口
		return csocket_ops::async_connect(m_sockfd, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
		// return true;
		//return  csocket_ops::async_send(m_sockfd, "", 1, 0) > 0 ? true :false;
		//int32 ret = ::connect(m_sockfd, (struct sockaddr *)&addrSrv, sizeof(addrSrv));      //与服务器进行连接
		//if (ret == 0)
		//{
		//	return true;
		//} 
		//ERROR_EX_LOG("connect ret =%d, error = %d", ret, errno);
		//ERROR_EX_LOG("WSAGetLastError = %d", WSAGetLastError()); /// 10035  ip port --> 网络不通
		// 10036 已经连接的socket
		//return false;
	}
	void 	cacceptor::destroy()
	{
		if (m_sockfd)
		{
			shutdown();
		}
	}
	
	/*bool 	cacceptor::open()
	{
		
			
		return true;
	}*/
	bool 	cacceptor::bind()
	{
		struct sockaddr_in sock_addr;
		memset( &sock_addr , 0 , sizeof(sock_addr) );

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = inet_addr(m_ip.c_str()); //htonl(INADDR_ANY);	// inet_addr(addr);
		sock_addr.sin_port        = htons(m_port);
		/*if (inet_aton(m_ip, &sock_addr.sin_addr) == 0)
		{
			ERROR_EX_LOG("bind convert ip = %s failed", m_ip);
			return InValidIp;
		}*/
		//int64 ret = csocket_ops::bind(m_sockfd, (const struct sockaddr *)&sock_addr, sizeof(sock_addr));
		if (!csocket_ops::bind(m_sockfd, (const struct sockaddr *)&sock_addr, sizeof(sock_addr)) )
		{
			ERROR_EX_LOG("bind  ip %s, port = %u", m_ip.c_str(), m_port);
			return false;
		}
		return true;
	}
	
	bool 	cacceptor::listen()
	{
		if (csocket_ops::listen(m_sockfd) == SOCKET_ERROR)
		{
			ERROR_EX_LOG("listen %lu", m_sockfd);
			return false;
		}
		return	true;
	}
	bool cacceptor::set_recvbuf_sendbuf(int64 recv_buff_size, int64 send_buff_size)
	{
		csocket_ops::setsocketopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &recv_buff_size, sizeof(recv_buff_size));
		csocket_ops::setsocketopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &recv_buff_size, sizeof(recv_buff_size));
		//  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
		return true;
	}
	bool cacceptor::set_nonagle(bool on)
	{
		int32 opt = on ? 1 : 0;
		int32 ret = csocket_ops::setsocketopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, static_cast<socklen_t>(sizeof(opt)));
		if (ret == SOCKET_ERROR)
		{
			ERROR_EX_LOG(" No nagle failed !");
			return false;
		}
		return true;
	}
	socket_type& 	cacceptor::get_sockfd()
	{
		return m_sockfd;
	}
	
	bool 	cacceptor::accept(socket_type& socket, struct sockaddr_in& sock_addr, int32& err_code)
	{

		uint32 addrlen = sizeof(sock_addr);
		socket = csocket_ops::accept(m_sockfd, (struct sockaddr *)(&(sock_addr)),(socklen_t*) &addrlen, err_code);
		if (socket == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}
	
	void 	cacceptor::shutdown()
	{
		::shutdown(m_sockfd, 0);
		//csocket_ops::close_socket(m_sockfd);
	}
} // namespace chen