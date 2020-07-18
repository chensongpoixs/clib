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
#include "chttp_session.h"
#include "clog.h"
#include "chttp_request_helper.h"
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>

namespace chen {
	chttp_session::chttp_session(boost::asio::io_service & io_service) 
		: m_is_requesting(false)
		, m_is_timing(false)
		, m_is_closeing(false)
		, m_client_ptr(NULL)
		, m_resolver(io_service)
		, m_timer(io_service)
		, m_socket(io_service)
		, m_send_buf(NULL)
		, m_send_size(0)
		, m_receive_buf(NULL)
		, m_receive_pos(0)
	{
	}

	chttp_session::~chttp_session()
	{

	}

	bool chttp_session::init(chttp_client * client_ptr)
	{
		m_client_ptr = client_ptr;

		if (get_recv_buf_size() < 4)
		{
			ERROR_EX_LOG("get_recv_buf_size() < 4");
			return false;
		}

		m_send_buf = new char[get_send_buf_size()];
		if (!m_send_buf)
		{
			ERROR_EX_LOG("new m_send_buf failed");
			return false;
		}

		m_receive_buf = new char[get_recv_buf_size()];
		if (!m_send_buf)
		{
			ERROR_EX_LOG("new m_send_buf failed");
			return false;
		}

		return true;
	}

	void chttp_session::destroy()
	{
		m_is_requesting = false;
		m_is_timing = false;
		m_is_closeing = false;
		m_client_ptr = NULL;

		if (m_send_buf)
		{
			delete[] m_send_buf;
			m_send_buf = NULL;
		}
		m_send_size = 0;

		if (m_receive_buf)
		{
			delete[] m_receive_buf;
			m_receive_buf = NULL;
		}
		m_receive_pos = 0;

		m_msg_client_id = 0;
		m_msg_id = 0;
	}

	bool chttp_session::http_request(const std::string & host, const char * request_ptr, uint32 request_size, uint32 client_id, uint16 msg_id, uint32 time_out_sec)
	{
		// 检查状态
		if (m_is_requesting || m_is_closeing)
		{
			ERROR_EX_LOG("m_is_requesting");
			return false;
		}

		if (request_size > get_send_buf_size())
		{
			ERROR_EX_LOG("request_size too big, %u > %u", request_size, get_send_buf_size());
			return false;
		}

		m_is_requesting = true;
		m_is_timing = true;

		m_msg_client_id = client_id;
		m_msg_id = msg_id;

		// 设置超时
		m_timer.expires_from_now(boost::posix_time::seconds(time_out_sec));
		m_timer.async_wait(std::bind(&chttp_session::check_deadline, this));

		memcpy(m_send_buf, request_ptr, request_size);
		m_send_size = request_size;

		std::string _host(host);
		std::string _service("http");
		std::string::size_type n = _host.find_first_of(':');
		if (n != std::string::npos)
		{
			_service = _host.substr(n + 1);
			_host = _host.substr(0, n);
		}

		boost::asio::ip::tcp::resolver::query query(_host, _service);
		m_resolver.async_resolve(query, boost::bind(&chttp_session::handle_resolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
		return true;
	}

	void chttp_session::close()
	{
		if (m_is_timing)
		{
			m_is_timing = false;
			m_timer.cancel();
		}
		if (!m_is_closeing)
		{
			m_is_closeing = true;
			boost::system::error_code ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			m_socket.close(ec);
		}
	}

	void chttp_session::handle_resolve(const boost::system::error_code & err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	{
		if (err)
		{
			_handle_result(EHRCode_Resolve, err.message());
			return;
		}

		if (!m_is_timing)
		{
			_handle_result(EHRCode_Timeout, "time out");
			return;
		}

		boost::asio::async_connect(m_socket, endpoint_iterator, boost::bind(&chttp_session::handle_connect, this, boost::asio::placeholders::error));
	}

	void chttp_session::handle_connect(const boost::system::error_code& err)
	{
		if (err)
		{
			_handle_result(EHRCode_Connect, err.message());
			return;
		}

		if (!m_is_timing)
		{
			_handle_result(EHRCode_Timeout, "time out");
			return;
		}

		boost::asio::async_write(m_socket, boost::asio::buffer(m_send_buf, m_send_size), boost::bind(&chttp_session::handle_write_request, this, boost::asio::placeholders::error));

	}

	void chttp_session::handle_write_request(const boost::system::error_code & err)
	{
		if (err)
		{
			_handle_result(EHRCode_Write, err.message());
			return;
		}

		if (!m_is_timing)
		{
			_handle_result(EHRCode_Timeout, "time out");
			return;
		}

		*reinterpret_cast<int32*>(m_receive_buf) = EHRCode_Succ;
		m_receive_pos = 4;
		boost::asio::async_read(m_socket, boost::asio::buffer(m_receive_buf + m_receive_pos, get_recv_buf_size() - m_receive_pos), boost::asio::transfer_all(), boost::bind(&chttp_session::handle_read_content, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	}

	void chttp_session::handle_read_content(const boost::system::error_code& err, std::size_t bytes_transferred)
	{
		if (err == boost::asio::error::eof)
		{
			m_receive_pos += bytes_transferred;
			_handle_result(EHRCode_Succ, "");
			return;
		}


		if (err)
		{
			_handle_result(EHRCode_Err, err.message());
			return;
		}

		if (!m_is_timing)
		{
			_handle_result(EHRCode_Timeout, "time out");
			return;
		}

		_handle_result(EHRCode_Err, err.message());
	}

	void chttp_session::check_deadline()
	{
		m_is_timing = false;

		if (!m_is_closeing)
		{
			m_is_closeing = true;
			boost::system::error_code ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			m_socket.close(ec);
		}

		_release();
	}

	void chttp_session::_close()
	{
		if (!m_is_closeing)
		{
			m_is_closeing = true;

			boost::system::error_code ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			m_socket.close(ec);
			m_timer.cancel();
		}

		// 尝试释放资源
		_release();
	}

	void chttp_session::_release()
	{
		// 尝试判断
		if (!m_is_closeing || m_is_requesting || m_is_timing)
		{
			return;
		}


		// 设置session状态
		m_is_closeing = false;
		// 归还session
		m_client_ptr->_return_session(this);
	}

	void chttp_session::_handle_result(uint32 status_code, std::string error_msg)
	{
		if (status_code == EHRCode_Succ)
		{

		}
		else
		{
			*reinterpret_cast<uint32*>(m_receive_buf) = status_code;
			m_receive_pos = 4;
			if (error_msg.length() < get_recv_buf_size() - 4)
			{
				memcpy(m_receive_buf + 4, error_msg.c_str(), error_msg.length());
				m_receive_pos += error_msg.length();
			}
		}

		// 错误回调
		m_client_ptr->on_response(m_msg_client_id, m_msg_id, m_receive_buf, m_receive_pos);

		// 修改请求计数
		m_is_requesting = false;
		m_send_size = 0;
		m_receive_pos = 0;

		// 关闭连接
		_close();
	}

	uint32 chttp_session::get_send_buf_size() const
	{
		return m_client_ptr->get_send_buf_size();
	}

	uint32 chttp_session::get_recv_buf_size() const
	{
		return m_client_ptr->get_recv_buf_size();
	}

}//namespace chen