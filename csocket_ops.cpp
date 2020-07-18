/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		csocket_ops
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

#include "csocket_ops.h"
#include "cnet_types.h"
#include "clog.h"

#if defined(__APPLE__)
#include <netdb.h>
#endif
namespace chen {
	namespace csocket_ops {
		
		socket_type socket(int32 domain, int32 type, int32 protocol)
		{
			return ::socket(domain, type, protocol);
		}
		
		bool bind(socket_type descriptor,const  struct sockaddr * addr, uint32 addrlen)
		{
			if (::bind(descriptor, addr, addrlen) == SOCKET_ERROR)
			{
				ERROR_EX_LOG("bind fail error = %ld, error = %s", errno, strerror(errno));
				return false;
			}
			return true;
		}

		//13 EACCES, EPERM 用户试图在套接字广播标志没有设置的情况下连接广播地址或由于防火墙策略导致连接失败。
		//98 EADDRINUSE本地地址处于使用状态。
		//97 EAFNOSUPPORT参数serv_add中的地址非合法地址。
		//11 EAGAIN没有足够空闲的本地端口。
		//114 EALREADY套接字为非阻塞套接字，并且原来的连接请求还未完成。
		//9EBADF 非法的文件描述符。
		//111 ECONNREFUSED远程地址并没有处于监听状态。
		//14 EFAULT 指向套接字结构体的地址非法。
		//115 EINPROGRESS套接字为非阻塞套接字，且连接请求没有立即完成。
		//4 EINTR 系统调用的执行由于捕获中断而中止。
		//106 EISCONN 已经连接到该套接字。
		//101 ENETUNREACH网络不可到达。
		//88 ENOTSOCK 文件描述符不与套接字相关。
		//110 ETIMEDOUT 连接超时
		bool async_connect(socket_type descriptor, const sockaddr * addr, uint32 addrlen)
		{
            {
//                int s = -1, rv;
//                char portstr[6];  /* strlen("65535") + 1; */
//                struct addrinfo hints, *servinfo, *bservinfo, *p, *b;
//                // port 1024 ->>>
//                snprintf(portstr,sizeof(portstr),"%d",1025);
//                memset(&hints,0,sizeof(hints));
//                hints.ai_family = AF_UNSPEC;
//                hints.ai_socktype = SOCK_STREAM;
//                const char * host = "127.0.0.1";
//                if ((rv = ::getaddrinfo(host, portstr, &hints, &servinfo)) != 0)
//                {
//                    return false;
//                }
//                for (p = servinfo; p != NULL; p = p->ai_next)
//                {
//                    /* Try to create the socket and to connect it.
//                     * If we fail in the socket() call, or on connect(), we retry with
//                     * the next entry in servinfo. */
//                    if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
//                        continue;
////                    // ÖØÓÃ±¾µØµØÖ·
////                    if (anetSetReuseAddr(err,s) == ANET_ERR) goto error;
//                    // ÊÇ·ñÉèÖÃÎÄ¼þÃèÊö·û·Ç×èÈû
//                    if (flags & ANET_CONNECT_NONBLOCK && anetNonBlock(err,s) != ANET_OK)
//                        goto error;
//                    if (source_addr) {
//                        int bound = 0;
//                        /* Using getaddrinfo saves us from self-determining IPv4 vs IPv6 */
//                        if ((rv = getaddrinfo(source_addr, NULL, &hints, &bservinfo)) != 0)
//                        {
//                            anetSetError(err, "%s", gai_strerror(rv));
//                            goto error;
//                        }
//                        for (b = bservinfo; b != NULL; b = b->ai_next) {
//                            if (bind(s, b->ai_addr, b->ai_addrlen) != -1) {
//                                bound = 1;
//                                break;
//                            }
//                        }
//                        freeaddrinfo(bservinfo);
//                        if (!bound) {
//                            anetSetError(err, "bind: %s", strerror(errno));
//                            goto error;
//                        }
//                    }
//                    if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
//                        /* If the socket is non-blocking, it is ok for connect() to
//                         * return an EINPROGRESS error here. */
//                        if (errno == EINPROGRESS && flags & ANET_CONNECT_NONBLOCK)
//                            goto end;
//                        close(s);
//                        s = ANET_ERR;
//                        continue;
//                    }
//
//                    /* If we ended an iteration of the for loop without errors, we
//                     * have a connected socket. Let's return to the caller. */
//                    goto end;
//                }
//                if (p == NULL)
//                    anetSetError(err, "creating socket: %s", strerror(errno));
//
//                error:
//                if (s != ANET_ERR) {
//                    close(s);
//                    s = ANET_ERR;
//                }
//
//                end:
//                freeaddrinfo(servinfo);
//
//                /* Handle best effort binding: if a binding address was used, but it is
//                 * not possible to create a socket, try again without a binding address. */
//                if (s == ANET_ERR && source_addr && (flags & ANET_CONNECT_BE_BINDING)) {
//                    return anetTcpGenericConnect(err,addr,port,NULL,flags);
//                } else {
//                    return s;
//                }



            }
			if (::connect(descriptor, addr, addrlen) == SOCKET_ERROR)
			{
				bool socket_ret = false;
#if defined(__linux__) || defined(__APPLE__)

			//	服务器开发中网络编程是一项基本技能也是重要的技能，在高并发socket编程中我们经常会使用到异步socket也就是非阻塞socket。下面记录下非阻塞socket中如何判断connect连接是否成。在非阻塞socket调用connect函数之后可能出现的情况有：
			//
			//	1 connect函数直接返回0，那么恭喜你连接直接成功了，不用考虑其他问题了。
			//
			//	2 连接返回错误，errno等于EINPROGRESS，这种情况表示socket正在连接，需要后续使用epoll_wait 或者select函数进行确认，当然也有很多其他不常用的方法比如write ，send等等，但是服务器异步socket几乎都不可能用这些方法。准确的来说Linux中高并发服务器都是使用epoll模型，所以使用epoll_wait函数。
			//
			//	3 连接返回错误，errno 不等于EINPROGRESS，表示connect函数调用失败了。
			//
			//	对于connect函数出现上述中的第二种情况那应该如何确认socket是否连接成功了，下面将讲述下epoll_wait 和select函数调用的不同处理情况：
			//
			//	1 epoll 模型：将连接的socket加入epoll的监听事件中，然后再调用epoll_wait函数等待事件触发，如果连接的socket的端口没有监听或者连接的ip不可达都会触发EPOLLERR事件，socket连接成功一般触发EPOLLOUT事件，但是如果对方在连接成功后立马给你回一条数据，那么可能同时触发EPOLLIN 和EPOLLOUT事件，网络编程的书上一般都说连接失败socket会变的可读可写，但是epoll模型中我不知道怎么触发这种情况，调试没有试出来。
			//
			//	2 select模型：同样将连接的socket加入读写错误三个事件中，调用select函数，如果socket连接失败那么socket会变的同时可读和可写，如果socket连接成功可能会出现两种情况，第一是socket可写，第二就是socket同时可读可写。那么现在问题来了，当socket同时可读可写的时候我们怎么判断socket到底是连接成功了还是失败了呢。所以我们在socket同时可读可写的时候需要调用另外一个函数来确定socket是否连接成功，那就是getsockopt函数，getsockopt函数有一个选项为SO_ERROR，通过该选项我们可以判断出socket是否连接成功，注意不是获取该函数的返回值而是函数的参数返回值，函数调用中的第四个参数会返回socket连接错误的错误码，如果成功错误码为0，否则不为0。该函数书上说调用之后会把错误同时赋值给errno但是我发现没有，只能通过函数调用的参数获取错误。就记录这么多了，该洗洗睡了。
					
				switch (errno) 
				{
					case EALREADY:
						break;
					case EINPROGRESS:  //套接字为非阻塞套接字，且连接请求没有立即完成。
					{
//						struct timeval time_out;
//						time_out.tv_sec = 0;
//						time_out.tv_usec = 15000;
//						fd_set write_fds;
//						FD_ZERO(&write_fds);
//						FD_SET(descriptor, &write_fds);
////						fd_set except_fds;
////						FD_ZERO(&except_fds);
////						FD_SET(descriptor, &except_fds);
//						int32 ret = 0;
//						ret = csocket_ops::select(descriptor+1, NULL, &write_fds, NULL, &time_out);
//						FD_ZERO(&write_fds);
////						FD_ZERO(&except_fds);
//						if (ret > 0)
						{
							//return true;
							//break;
							socket_ret = true;
						}
						break;
					}
					case ECONNREFUSED:
						break;
					case EISCONN:
						break;
					case ETIMEDOUT: //连接超时
					ERROR_EX_LOG("connection refused,");
					break;
					case ENETUNREACH:
						break;
					case EADDRINUSE:
						break;
					case EBADF:
						break;
					case EFAULT:
						break;
					case ENOTSOCK:
						break;
					default:
					{
						break;
					}
				}//end of switch
#elif  defined(_MSC_VER)

			//	说明：第一个参数是套接字，第二个是包含地址信息的sockaddr_in结构体，第三个 是sockaddr_in结构体长度；
			//
			//		返回值 : 正常返回0， 错误返回SOCKET_ERR, 也就是 - 1， 
			//
			//		可用WSAGetlastError()获取到错误码；
			//
			//		需要注意的是 :
			//	1.只有对于阻塞套接字，返回值才可以直接表示连接尝试成功或失败。使用非阻塞套接字时，无法立即完成连接尝试。 在这种情况下，connect将返回SOCKET_ERROR，WSAGetLastError将返回WSAEWOULDBLOCK。
			//		2.对于非阻塞套接字，无法立即完成连接尝试，也就是说， 在这种情况下，connect将返回SOCKET_ERROR，WSAGetLastError将返回WSAEWOULDBLOCK。这时要判断是否连接成功，可以使用select函数, 通过检查套接字是否可写来确定连接请求的是否已 成功。也就是设置select的第三个参数，判断返回的可写的套接字个数，如果大于0，则 表示有成功连接 :
			//	int nRet = select(0, 0, &fd, 0, &tConnectTimeOut);
			//
			//	if (nRet > 0)
			//		return true;
			//
			//另外两种方法判断非阻塞套接字是否成功，如:
			//
			//	1.  如果应用程序使用WSAAsyncSelect表示对连接事件感兴趣，则应用程序将收到FD_CONNECT通知，指示连接操作已完成（成功与否）
			//
			//	2. 如果应用程序使用WSAEventSelect来指示对连接事件的兴趣，那么将发出关联的事件对象的信号，指示连接操作已完成（成功与否）。
					
				int32 iErr = WSAGetLastError();
				//ERROR_EX_LOG("WSAGetLastError = %d", iErr); /// 10035  ip port --> 网络不通
				switch (iErr)
				{
					case WSANOTINITIALISED:
						break;
					case WSAENETDOWN:
						break;
					case WSAEADDRINUSE:
						break;
					case WSAEINTR:
						break;
					case WSAEINPROGRESS:
						break;
					case WSAEALREADY:
						break;
					case WSAEADDRNOTAVAIL:
						break;
					case WSAEAFNOSUPPORT:
						break;
					case WSAECONNREFUSED:
						break;
					case WSAEFAULT:
						break;
					case WSAEINVAL:
						break;
					case WSAEISCONN:
						break;
					case WSAENETUNREACH:
						break;
					case WSAENOBUFS:
						break;
					case WSAENOTSOCK:
						break;
					case WSAETIMEDOUT: //连接超时  TCP 客户端没有收到SYN分支的响应
						ERROR_EX_LOG("connection refused,");
						break;
					case WSAEWOULDBLOCK: // server accept  时 
					{
						//ERROR_EX_LOG(" not service");
						struct timeval time_out;
						time_out.tv_sec = 0;
						time_out.tv_usec = 1500;
						fd_set write_fds;
						FD_ZERO(&write_fds);
						FD_SET(descriptor, &write_fds);
						//fd_set except_fds;
						//FD_ZERO(&except_fds);
						//FD_SET(descriptor, &except_fds);
						int32 ret = 0;
						ret = csocket_ops::select(0, NULL, &write_fds, NULL, &time_out);
						FD_ZERO(&write_fds);
						//FD_ZERO(&except_fds);
						if (ret > 0)
						{
							//return true;
							//break;
							socket_ret = true;
						}
						break;
					};
					default:
					{
						break;
					};
				};//end of switch
#endif
				return socket_ret;
			}

			return true;
		}
		// socket_type accept(socket_type sockfd, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code);
		socket_type accept(socket_type descriptor, struct sockaddr *addr, socklen_t *addrlen, int32 &error_code)
		{
			socket_type tmep = ::accept(descriptor, addr, addrlen);
#if defined(_MSC_VER)

			error_code = WSAGetLastError();
			if (error_code == WSAEWOULDBLOCK)
			{
			}
#endif // #if defined(_MSC_VER)			
			
			return tmep;
		}
	

		int32 listen(socket_type descriptor, int32 backlog)
		{
			return ::listen(descriptor, backlog);
		}
		
		int32 async_send(socket_type descriptor, const void *buf, int32 len, int32 flags)
		{
			return ::send(descriptor,(const char *) buf, len, flags);
		}
		 
		 // async recv
		int32 async_recv(socket_type descriptor, void *buf, int32 len, int32 flags)
		 {
			int32 recv_len = ::recv(descriptor, (char *)buf, len, flags);

			 //if (recv_len == SOCKET_ERROR) {
				// int32 id = WSAGetLastError();
				// switch (id)
				// {
				// case WSANOTINITIALISED: ERROR_EX_LOG("not initialized\n"); break;
				// case WSASYSNOTREADY: ERROR_EX_LOG("sub sys not ready\n"); break;
				// case WSAHOST_NOT_FOUND: ERROR_EX_LOG("name server not found\n"); break;
				// case WSATRY_AGAIN: ERROR_EX_LOG("server fail\n"); break;
				// case WSANO_RECOVERY: ERROR_EX_LOG("no recovery\n"); break;
				// case WSAEINPROGRESS: ERROR_EX_LOG("socket blocked by other prog\n"); break;
				// case WSANO_DATA: ERROR_EX_LOG("no data record\n"); break;
				// case WSAEINTR: ERROR_EX_LOG("blocking call canciled\n"); break;
				// case WSAEPROCLIM: ERROR_EX_LOG("limit exceeded\n");
				// case WSAEFAULT: ERROR_EX_LOG("lpWSAData in startup not valid\n");
				// default: ERROR_EX_LOG("unknown error id = %d\n", id); break;
				// };
				// ERROR_EX_LOG("receive error.\n");
				// 
			 //}
			 return recv_len;
		 }
		 
		 int32 select(int32 maxfd, fd_set* read, fd_set *write, fd_set *exce, const struct timeval* time_out)
		 {
			 return ::select(maxfd, read, write, exce, (struct timeval*)time_out);
		 }
		 
		 bool set_nonblocking(socket_type descriptor, bool on)
		 {

			 /***设套接字为非阻塞模式***/ 
#if defined(_MSC_VER)
			 unsigned long ul = 1;
			 int32	flags = ::ioctlsocket(descriptor, FIONBIO, &ul); //设置套接字非阻塞模式
			 if (flags == -1)
			 {
				 return false;
			 }

#elif defined(__GNUC__)

			 int32 flags = ::fcntl(descriptor, F_GETFL, 0);
			 if (flags == -1)
			 {
				 return false;
			 }

			 if (on)
			 {   // //非阻塞事件
				 flags |= O_NONBLOCK;
			 }
			 else
			 {
				 flags &= ~O_NONBLOCK;
			 }
			 if (-1 == ::fcntl(descriptor, F_SETFL, flags))
			 {
				 return false;
			 }
			
#else
#pragma error "unknow platform!!!"

#endif
			
			 return true;
		 }

		 bool close_socket(socket_type descriptor)
		 {
#if defined(_MSC_VER)

			 if (::closesocket(descriptor) == SOCKET_ERROR)
			 {
				 //int32 iErr = WSAGetLastError() ;
				 return false;
			 }

#elif defined(__linux__) || defined(__APPLE__)

			 if (::close(descriptor) < 0)
			 {
				 //errno
				return false;
			}
#else

#pragma error "unknow platform!!!"

#endif
			 return true;
		 }

		 int32 getsocketopt(socket_type descriptor, int32 level, int32 optname, void * optval, int32 optlen)
		 {
			 
			 int result = ::getsockopt(descriptor, level, optname, (char*)optval, (socklen_t*)&optlen);
			 return result;
		 }
		 
		 int32 setsocketopt(socket_type descriptor, int32 level, int32 optname, void * optval, int32 optlen)
		 {
			 int32 result = ::setsockopt(descriptor, level, optname, (char*)optval, optlen);
			 return result;
		 }

		 int32 shutdown(socket_type descriptor, int32 how)
		 {
			 return ::shutdown(descriptor, how);
		 }
	}
}
