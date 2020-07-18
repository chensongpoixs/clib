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


#ifndef _C_HTTP_REQUEST_HELPER_H_
#define _C_HTTP_REQUEST_HELPER_H_

#include "cnoncopyable.h"
#include "cnet_types.h"
#include <string>
namespace chen {
	enum EHttpResponseCode
	{
		EHRCode_Succ = 200,
		EHRCode_Timeout = 801,
		EHRCode_Resolve = 802,
		EHRCode_Connect = 803,
		EHRCode_Write = 804,
		EHRCode_Err = 805,  // web内部错误
		EHRCode_JsonPassErr = 806,
	};

	void http_address_parse(const std::string& address, std::string& server_address, std::string& method_path);


	namespace chttp_request_helper
	{
		class chttp_para : public cnoncopyable
		{
		public:
			chttp_para();
			~chttp_para();

		public:
			bool init(uint32 buf_size);
			void destroy();

		public:
			const char* get_buf() const { return m_buf; }

			uint32 get_size() const { return m_pos; }

			bool has_error() const { return m_error; }

			bool no_error() const { return !has_error(); }

			void reset();

		private:
			void write(const void* buf, uint32 len);

			void set_error();

		public:
			chttp_para& operator<<(char value)
			{
				write(&value, sizeof(value));
				return *this;
			}

			chttp_para& operator<<(bool value)
			{
				return operator<<(value ? '1' : '0');
			}

			chttp_para& operator<<(signed char value)
			{
				uint32 pos = sprintf(m_temp_buf, "%d", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(unsigned char value)
			{
				uint32 pos = sprintf(m_temp_buf, "%u", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(signed short value)
			{
				uint32 pos = sprintf(m_temp_buf, "%d", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(unsigned short value)
			{
				uint32 pos = sprintf(m_temp_buf, "%u", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(signed int value)
			{
				uint32 pos = sprintf(m_temp_buf, "%d", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(unsigned int value)
			{
				uint32 pos = sprintf(m_temp_buf, "%u", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(signed long value)
			{
				uint32 pos = sprintf(m_temp_buf, "%ld", value);
				write(m_temp_buf, pos);
				return *this;
			}
			
			chttp_para& operator<<(unsigned long value)
			{
				uint32 pos = sprintf(m_temp_buf, "%lu", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(signed long long value)
			{
				uint32 pos = sprintf(m_temp_buf, "%lld", value);
				write(m_temp_buf, pos);
				return *this;
			}

			chttp_para& operator<<(unsigned long long value)
			{
				uint32 pos = sprintf(m_temp_buf, "%llu", value);
				write(m_temp_buf, pos);
				return *this;
			}

			//must be zero end string, not write the end 0
			chttp_para& operator<<(const char* c_str)
			{
				write(c_str, static_cast<uint32>(strlen(c_str)));
				return *this;
			}

			chttp_para& operator<<(const std::string& value)
			{
				write_value(value.c_str(), static_cast<uint32>(value.length()));
				return *this;
			}

		private:
			void write_value(const char* p, uint32 len);

		private:
			char*			m_buf;
			char			m_temp_buf[24];
			uint32			m_buf_size;
			uint32			m_pos;
			bool			m_error;
		};


		class chttp_request : public cnoncopyable
		{
		public:
			/*explicit*/ chttp_request();
			~chttp_request();

		public:
			bool init(uint32 buf_size);
			void destroy();

		public:
			const char* get_buf() const { return m_buf; }

			uint32 get_size() const { return m_pos; }

			bool has_error() const { return m_error; }

			bool no_error() const { return !has_error(); }

			void reset();

			chttp_para& get_para() { return m_para; }

			void make_get(const std::string& host, const std::string& path, const std::string& filename);
			void make_post(const std::string& host, const std::string& path, const std::string& filename);

		private:
			void write(const void* buf, uint32 len);

			chttp_request& operator<<(const char& value)
			{
				write(&value, sizeof(value));
				return *this;
			}
			chttp_request& operator<<(const unsigned int& value)
			{
				uint32 pos = sprintf(m_temp_buf, "%u", value);
				write(m_temp_buf, pos);
				return *this;
			}
			chttp_request& operator<<(const char* c_str)
			{
				write(c_str, static_cast<uint32>(strlen(c_str)));
				return *this;
			}
			chttp_request& operator<<(const std::string& value)
			{
				write(value.c_str(), static_cast<uint32>(value.length()));
				return *this;
			}

			chttp_request& operator<<(const chttp_para& value)
			{
				write(value.get_buf(), value.get_size());
				return *this;
			}

			void set_error();

		private:
			char*			m_buf;
			char			m_temp_buf[24];
			uint32			m_buf_size;
			uint32			m_pos;
			bool			m_error;
			chttp_para		m_para;
		};


		class chttp_response
		{
		public:
			chttp_response();


		public:

			bool parse(const void* buf, uint32 len);

			int32 get_code() const { return m_code; }

			const char* get_content_ptr() const { return m_buf + m_content_index; }

			uint32 get_content_size() const { return m_content_size; }

			void set_err_msg(const std::string& err_msg) { m_err_msg = err_msg; }
			const std::string& get_err_msg() const { return m_err_msg; }

		private:
			const char*	m_buf;
			uint32		m_buf_size;
			int32		m_code;
			std::string m_err_msg;
			uint32		m_content_index;
			uint32		m_content_size;
		};


		class chttp_string_reader
		{
		public:
			chttp_string_reader(const char* buf, uint32 len);


		public:
			void get_str(std::string& str);
			void get_line(std::string& str);

		private:
			const char*	m_buf;
			uint32		m_buf_size;
			uint32		m_pos;
		};

	};
} // namespace chen

#endif // !#define _C_HTTP_REQUEST_HELPER_H_