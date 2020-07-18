/***********************************************************************************************
			created: 		2019-05-15

			author:			chensong

			purpose:		http_session
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
#ifndef _C_HTTP_SESSION_H_
#define _C_HTTP_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_define.h"
#include "cnet_types.h"
#include "chttp_client.h"
#include <boost/function.hpp>
namespace chen {
	const uint32 MAX_HTTP_SESSION_NUM = 3000;

	class chttp_session : private cnoncopyable
	{
	public:
		explicit chttp_session(boost::asio::io_service& io_service);
		virtual ~chttp_session();
	public:
		bool init(chttp_client* client_ptr);
		void destroy();

	public:
		bool is_closeing() const { return m_is_closeing; }

		bool http_request(const std::string& host, const char* request_ptr, uint32 request_size, uint32 client_id, uint16 msg_id, uint32 time_out_sec);

		void close();
	public:
		boost::asio::ip::tcp::socket& get_socket() { return m_socket; }

		// 地址解析回调
		void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

		// 连接回调
		void handle_connect(const boost::system::error_code& err);

		// 请求发送完回调
		void handle_write_request(const boost::system::error_code& err);

		// 内容读取回调
		void handle_read_content(const boost::system::error_code& err, std::size_t bytes_transferred);

		// 时钟回调
		void check_deadline();

	private:

		// 关闭指定连接
		void _close();

		// 尝试释放资源
		void _release();

		// 处理结果
		void _handle_result(uint32 status_code, std::string error_msg);

		// 判断状态
		bool _is_requesting() const { return m_is_requesting; }

		uint32 get_send_buf_size() const;
		uint32 get_recv_buf_size() const;
	private:
		bool							m_is_requesting;	// 是否还有异步请求
		bool							m_is_timing;		// 是否正在倒计时
		bool							m_is_closeing;		// 是否已经调用过socket close
		chttp_client*					m_client_ptr;		// 上层client指针，用于回调消息
		boost::asio::ip::tcp::resolver	m_resolver;			// 地址解析
		boost::asio::deadline_timer		m_timer;			// 时钟
		boost::asio::ip::tcp::socket	m_socket;			// session的socket资源

		char*							m_send_buf;
		uint32							m_send_size;
		char*							m_receive_buf;
		uint32							m_receive_pos;
		uint32							m_msg_client_id;			// 用户id
		uint16							m_msg_id;				// 回调消息id
	};
}// namespace chen


#endif // !#define _C_HTTP_SESSION_H_