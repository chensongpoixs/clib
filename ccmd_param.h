/********************************************************************
				created:	2019-05-13

				author:		chensong

				purpose:	字符串读取工具
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
*********************************************************************/
#ifndef _C_CMD_PARAM_H_
#define _C_CMD_PARAM_H_

#include <boost/lexical_cast.hpp>
#include "cnoncopyable.h"
#include <string>
#include <vector>
#include "cnet_types.h"

namespace chen {

	class ccmd_param :private cnoncopyable
	{
	public:
		ccmd_param() : m_index(-1) {}
		~ccmd_param() {}

		void parse_cmd(const std::string& code);

		void set_cmd(const std::string& cmd) { m_cmd = cmd;	}

		const std::string& get_cmd() const { return m_cmd; }

		void push_param(const std::string& tokens) { m_tokens.push_back(tokens); }

		int get_param_num() const {	return static_cast<int>(m_tokens.size()); }

		void get_all_params(std::string& cmd) const;

		void get_params(std::string& cmd, int start_index) const;

		void get_remain_paras(std::string& cmd) const;

		template<typename T>
		bool get_param(int index, T& param) const
		{
			try
			{
				if (index >= get_param_num())
				{
					return false;
				}

				param = boost::lexical_cast<T>(m_tokens[index]);
				return true;
			}
			catch (boost::bad_lexical_cast&)
			{
				return false;
			}
			return true;
		}

		template<typename T>
		bool get_next_param(T& param) const
		{
			++m_index;
			return get_param(m_index, param);
		}
		const std::string & get_param(int32 index) const { return  m_tokens[index]; }

	private:
		std::string				m_cmd;
		std::vector<std::string> m_tokens;
		mutable int m_index;
	};

} // namespace chen

#endif // _C_CMD_PARAM_H_
