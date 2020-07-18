/***********************************************************************************************
			created: 		2019-05-15

			author:			chensong

			purpose:		http_request_helper
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


#include "chttp_request_helper.h"
#include "clog.h"

namespace chen {
	void http_address_parse(const std::string& address, std::string& server_address, std::string& method_path)
	{
		std::string::size_type pos = address.find_first_of('/');
		if (pos != std::string::npos)
		{
			server_address = address.substr(0, pos);
			method_path = address.substr(pos);
		}
		else
		{
			server_address = address;
			method_path = "/";
		}
	}

	namespace chttp_request_helper
	{
		//规范见 RFC 1738
		struct cencodeURIDefineNode {
			size_t len;
			char ta[4];
		};

		static cencodeURIDefineNode g_URIEncodeMap[256] = {
			{ 3, "%00" },
			{ 3, "%01" },
			{ 3, "%02" },
			{ 3, "%03" },
			{ 3, "%04" },
			{ 3, "%05" },
			{ 3, "%06" },
			{ 3, "%07" },
			{ 3, "%08" },
			{ 3, "%09" },
			{ 3, "%0A" },
			{ 3, "%0B" },
			{ 3, "%0C" },
			{ 3, "%0D" },
			{ 3, "%0E" },
			{ 3, "%0F" },
			{ 3, "%10" },
			{ 3, "%11" },
			{ 3, "%12" },
			{ 3, "%13" },
			{ 3, "%14" },
			{ 3, "%15" },
			{ 3, "%16" },
			{ 3, "%17" },
			{ 3, "%18" },
			{ 3, "%19" },
			{ 3, "%1A" },
			{ 3, "%1B" },
			{ 3, "%1C" },
			{ 3, "%1D" },
			{ 3, "%1E" },
			{ 3, "%1F" },
			{ 3, "%20" },
			{ 1, "!" },
			{ 3, "%22" },
			{ 3, "%23" },
			{ 3, "%24" },
			{ 3, "%25" },
			{ 3, "%26" },
			{ 1, "'" },
			{ 1, "(" },
			{ 1, ")" },
			{ 1, "*" },
			{ 3, "%2B" },
			{ 3, "%2C" },
			{ 1, "-" },
			{ 1, "." },
			{ 3, "%2F" },
			{ 1, "0" },
			{ 1, "1" },
			{ 1, "2" },
			{ 1, "3" },
			{ 1, "4" },
			{ 1, "5" },
			{ 1, "6" },
			{ 1, "7" },
			{ 1, "8" },
			{ 1, "9" },
			{ 3, "%3A" },
			{ 3, "%3B" },
			{ 3, "%3C" },
			{ 3, "%3D" },
			{ 3, "%3E" },
			{ 3, "%3F" },
			{ 3, "%40" },
			{ 1, "A" },
			{ 1, "B" },
			{ 1, "C" },
			{ 1, "D" },
			{ 1, "E" },
			{ 1, "F" },
			{ 1, "G" },
			{ 1, "H" },
			{ 1, "I" },
			{ 1, "J" },
			{ 1, "K" },
			{ 1, "L" },
			{ 1, "M" },
			{ 1, "N" },
			{ 1, "O" },
			{ 1, "P" },
			{ 1, "Q" },
			{ 1, "R" },
			{ 1, "S" },
			{ 1, "T" },
			{ 1, "U" },
			{ 1, "V" },
			{ 1, "W" },
			{ 1, "X" },
			{ 1, "Y" },
			{ 1, "Z" },
			{ 3, "%5B" },
			{ 3, "%5C" },
			{ 3, "%5D" },
			{ 3, "%5E" },
			{ 1, "_" },
			{ 3, "%60" },
			{ 1, "a" },
			{ 1, "b" },
			{ 1, "c" },
			{ 1, "d" },
			{ 1, "e" },
			{ 1, "f" },
			{ 1, "g" },
			{ 1, "h" },
			{ 1, "i" },
			{ 1, "j" },
			{ 1, "k" },
			{ 1, "l" },
			{ 1, "m" },
			{ 1, "n" },
			{ 1, "o" },
			{ 1, "p" },
			{ 1, "q" },
			{ 1, "r" },
			{ 1, "s" },
			{ 1, "t" },
			{ 1, "u" },
			{ 1, "v" },
			{ 1, "w" },
			{ 1, "x" },
			{ 1, "y" },
			{ 1, "z" },
			{ 3, "%7B" },
			{ 3, "%7C" },
			{ 3, "%7D" },
			{ 1, "~" },
			{ 3, "%7F" },
			{ 3, "%80" },
			{ 3, "%81" },
			{ 3, "%82" },
			{ 3, "%83" },
			{ 3, "%84" },
			{ 3, "%85" },
			{ 3, "%86" },
			{ 3, "%87" },
			{ 3, "%88" },
			{ 3, "%89" },
			{ 3, "%8A" },
			{ 3, "%8B" },
			{ 3, "%8C" },
			{ 3, "%8D" },
			{ 3, "%8E" },
			{ 3, "%8F" },
			{ 3, "%90" },
			{ 3, "%91" },
			{ 3, "%92" },
			{ 3, "%93" },
			{ 3, "%94" },
			{ 3, "%95" },
			{ 3, "%96" },
			{ 3, "%97" },
			{ 3, "%98" },
			{ 3, "%99" },
			{ 3, "%9A" },
			{ 3, "%9B" },
			{ 3, "%9C" },
			{ 3, "%9D" },
			{ 3, "%9E" },
			{ 3, "%9F" },
			{ 3, "%A0" },
			{ 3, "%A1" },
			{ 3, "%A2" },
			{ 3, "%A3" },
			{ 3, "%A4" },
			{ 3, "%A5" },
			{ 3, "%A6" },
			{ 3, "%A7" },
			{ 3, "%A8" },
			{ 3, "%A9" },
			{ 3, "%AA" },
			{ 3, "%AB" },
			{ 3, "%AC" },
			{ 3, "%AD" },
			{ 3, "%AE" },
			{ 3, "%AF" },
			{ 3, "%B0" },
			{ 3, "%B1" },
			{ 3, "%B2" },
			{ 3, "%B3" },
			{ 3, "%B4" },
			{ 3, "%B5" },
			{ 3, "%B6" },
			{ 3, "%B7" },
			{ 3, "%B8" },
			{ 3, "%B9" },
			{ 3, "%BA" },
			{ 3, "%BB" },
			{ 3, "%BC" },
			{ 3, "%BD" },
			{ 3, "%BE" },
			{ 3, "%BF" },
			{ 3, "%C0" },
			{ 3, "%C1" },
			{ 3, "%C2" },
			{ 3, "%C3" },
			{ 3, "%C4" },
			{ 3, "%C5" },
			{ 3, "%C6" },
			{ 3, "%C7" },
			{ 3, "%C8" },
			{ 3, "%C9" },
			{ 3, "%CA" },
			{ 3, "%CB" },
			{ 3, "%CC" },
			{ 3, "%CD" },
			{ 3, "%CE" },
			{ 3, "%CF" },
			{ 3, "%D0" },
			{ 3, "%D1" },
			{ 3, "%D2" },
			{ 3, "%D3" },
			{ 3, "%D4" },
			{ 3, "%D5" },
			{ 3, "%D6" },
			{ 3, "%D7" },
			{ 3, "%D8" },
			{ 3, "%D9" },
			{ 3, "%DA" },
			{ 3, "%DB" },
			{ 3, "%DC" },
			{ 3, "%DD" },
			{ 3, "%DE" },
			{ 3, "%DF" },
			{ 3, "%E0" },
			{ 3, "%E1" },
			{ 3, "%E2" },
			{ 3, "%E3" },
			{ 3, "%E4" },
			{ 3, "%E5" },
			{ 3, "%E6" },
			{ 3, "%E7" },
			{ 3, "%E8" },
			{ 3, "%E9" },
			{ 3, "%EA" },
			{ 3, "%EB" },
			{ 3, "%EC" },
			{ 3, "%ED" },
			{ 3, "%EE" },
			{ 3, "%EF" },
			{ 3, "%F0" },
			{ 3, "%F1" },
			{ 3, "%F2" },
			{ 3, "%F3" },
			{ 3, "%F4" },
			{ 3, "%F5" },
			{ 3, "%F6" },
			{ 3, "%F7" },
			{ 3, "%F8" },
			{ 3, "%F9" },
			{ 3, "%FA" },
			{ 3, "%FB" },
			{ 3, "%FC" },
			{ 3, "%FD" },
			{ 3, "%FE" },
			{ 3, "%FF" }
		};

		chttp_para::chttp_para() : m_buf(NULL)
			, m_buf_size(0)
			, m_pos(0)
			, m_error(false)
		{

		}

		chttp_para::~chttp_para()
		{
			destroy();
		}

		bool chttp_para::init(uint32 buf_size)
		{
			if (m_buf) return true;

			m_buf = new char[buf_size];
			if (!m_buf)
			{
				ERROR_EX_LOG("new m_buf failed!");
				return false;
			}

			memset(m_buf, 0, buf_size);
			memset(m_temp_buf, 0, sizeof(m_temp_buf));

			m_buf_size = buf_size;

			return true;
		}

		void chttp_para::destroy()
		{
			if (m_buf)
			{
				delete[] m_buf;
				m_buf = NULL;
			}

			m_buf_size = 0;
			m_pos = 0;
			m_error = false;
		}

		void chttp_para::reset()
		{
			m_pos = 0;
			m_error = false;
		}

		void chttp_para::write(const void* buf, uint32 len)
		{
			if (!m_error && m_pos + len <= m_buf_size)
			{
				memcpy(m_buf + m_pos, buf, len);
				m_pos += len;
				return;
			}
			set_error();
		}

		void chttp_para::set_error()
		{
			m_error = true;
			ERROR_EX_LOG("buf not enouth, %u", m_buf_size);
		}

		void chttp_para::write_value(const char* p, uint32 len)
		{
			if (has_error())
			{
				return;
			}

			if (len == 0)
			{
				return;
			}
			uint32 remain_len = m_buf_size - m_pos;
			if (remain_len < len * 3 + 1)
			{
				set_error();
				return;
			}

			const unsigned char* buf = (unsigned char *)p;

			cencodeURIDefineNode* node_ptr = NULL;
			for (uint32 i = 0; i < len; ++i)
			{
				node_ptr = &g_URIEncodeMap[buf[i]];

				memcpy(m_buf + m_pos, (*node_ptr).ta, (*node_ptr).len);
				m_pos += (*node_ptr).len;
			}
		}


		chttp_request::chttp_request() : m_buf(NULL)
			, m_buf_size(0)
			, m_pos(0)
			, m_error(false)
		{

		}

		chttp_request::~chttp_request()
		{
			destroy();
		}

		bool chttp_request::init(uint32 buf_size)
		{
			const int request_buf_size = buf_size + 1024;

			if (m_buf) return true;

			if (!m_para.init(buf_size))
			{
				return false;
			}

			m_buf = new char[request_buf_size];
			if (!m_buf)
			{
				ERROR_EX_LOG("new m_buf failed!");
				return false;
			}

			memset(m_buf, 0, request_buf_size);
			memset(m_temp_buf, 0, sizeof(m_temp_buf));

			m_buf_size = request_buf_size;

			return true;
		}

		void chttp_request::destroy()
		{
			if (m_buf)
			{
				delete[] m_buf;
				m_buf = NULL;
			}

			m_buf_size = 0;
			m_pos = 0;
			m_error = false;

			m_para.destroy();
		}

		void chttp_request::reset()
		{
			m_pos = 0;
			m_error = false;
		}

		void chttp_request::write(const void* buf, uint32 len)
		{
			if (!m_error && m_pos + len <= m_buf_size)
			{
				memcpy(m_buf + m_pos, buf, len);
				m_pos += len;
				return;
			}
			set_error();
		}

		void chttp_request::make_get(const std::string& host, const std::string& path, const std::string& filename)
		{
			reset();
			(*this) << "GET " << path << filename << '?' << m_para << " HTTP/1.0\r\n"
				<< "Host: " << host << "\r\n"
				<< "Accept: */*\r\n"
				<< "Content-Type: application/x-www-form-urlencoded\r\n"
				<< "Connection: close\r\n\r\n"
				;

		}
		void chttp_request::make_post(const std::string& host, const std::string& path, const std::string& filename)
		{
			reset();
			(*this) << "POST " << path << filename << " HTTP/1.0\r\n"
				<< "Host: " << host << "\r\n"
				<< "Accept: */*\r\n"
				<< "Content-Type: application/x-www-form-urlencoded\r\n"
				<< "Content-Length: " << m_para.get_size() << "\r\n"
				<< "Connection: close\r\n\r\n"
				<< m_para
				;

		}

		void chttp_request::set_error()
		{
			m_error = true;
			ERROR_EX_LOG("buf not enouth, %u", m_buf_size);
		}

		chttp_response::chttp_response() : m_buf(NULL)
			, m_buf_size(0)
			, m_code(0)
			, m_content_index(0)
			, m_content_size(0)
		{}

		bool chttp_response::parse(const void* buf, uint32 len)
		{
			m_buf = (const char*)buf;
			m_buf_size = len;
			m_code = EHRCode_Err;
			m_err_msg = "len to small";
			m_content_index = 0;
			m_content_size = 0;

			if (len < 4)
			{
				return false;
			}

			m_buf += 4;
			m_buf_size -= 4;

			m_code = *reinterpret_cast<const int32 *>(buf);

			if (m_code != EHRCode_Succ)
			{
				m_err_msg.assign(m_buf, m_buf_size);
				return false;
			}

			//找到第一行 HTTP/1.1 200 OK
			if (m_buf_size < 15)
			{
				m_err_msg = "result to short";
				return false;
			}
			{
				chttp_string_reader string_reader(m_buf, m_buf_size);
				std::string str;

				//get version
				string_reader.get_str(str);
				if (str.length() < 5 || str.substr(0, 5) != "HTTP/")
				{
					m_err_msg = "not http";
					return false;
				}

				//get code
				str.clear();
				string_reader.get_str(str);
				m_code = atoi(str.c_str());
				if (m_code != EHRCode_Succ)
				{
					m_err_msg.clear();
					string_reader.get_str(m_err_msg);
					return false;
				}
			}

			static const uint32 split_size = 4; // \r\n\r\n

			if (m_buf_size < split_size)
			{
				m_err_msg = "result to short";
				return false;
			}
			//find content
			for (uint32 i = 0; i < m_buf_size - split_size; ++i)
			{
				if (m_buf[i] == '\r' && m_buf[i + 1] == '\n' && m_buf[i + 2] == '\r' && m_buf[i + 3] == '\n')
				{
					m_content_index = i + split_size;
					m_content_size = m_buf_size - m_content_index;
					return true;
				}
			}
			m_err_msg = "no content flag found";
			return false;
		}

		chttp_string_reader::chttp_string_reader(const char* buf, uint32 len)
			: m_buf(buf), m_buf_size(len), m_pos(0) {}

		void chttp_string_reader::get_str(std::string& str)
		{
			uint32 i;
			for (i = m_pos; i < m_buf_size; ++i)
			{
				if (m_buf[i] != ' ')
				{
					str.append(1, m_buf[i]);
				}
				else
				{
					if (i + 1 < m_buf_size)
					{
						++i;
					}
					break;
				}
			}
			while (i + 1 < m_buf_size && m_buf[i] == ' ')
			{
				++i;
			}
			m_pos = i;
		}
		void chttp_string_reader::get_line(std::string& str)
		{
			uint32 i;
			for (i = m_pos; i < m_buf_size; ++i)
			{
				if (m_buf[i] != '\r' || m_buf[i] != '\n')
				{
					str.append(1, m_buf[i]);
				}
				else
				{
					if (i + 1 < m_buf_size)
					{
						++i;
					}
					break;
				}
			}
			while (i + 1 < m_buf_size && (m_buf[i] == '\r' || m_buf[i] == '\n'))
			{
				++i;
			}
			m_pos = i;
		}
	}
}//namespace chen

