/***********************************************************************************************
created: 		2019-06-15

author:			chensong

purpose:		safe_queue
�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧������������ֻᱻ��������ں���������ĳ�������ҹ��������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶����ߡ�
�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
************************************************************************************************/
#include "csafe_queue.h"

namespace chen {
	csafe_queue::csafe_queue()
		: m_head_msg_ptr(NULL)
		, m_tail_msg_ptr(NULL)
		, m_size (0)
	{
	}
	csafe_queue::~csafe_queue()
	{
	}
	void csafe_queue::push(cnet_msg * msg_ptr)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		// - -> l
		//      l -> ll 
		//           ll -> lll
		if (is_full())
		{   //����
			m_head_msg_ptr->push(msg_ptr);
			m_head_msg_ptr = msg_ptr;
		}
		else
		{
			m_head_msg_ptr = m_tail_msg_ptr = msg_ptr;
			m_head_msg_ptr->push(msg_ptr);
			//m_head_msg_ptr = msg_ptr;
		}

		++m_size;
	}
	void csafe_queue::pop(cnet_msg* msg_ptr)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		if (is_full())
		{
			msg_ptr = m_tail_msg_ptr->get_next_msg();
			if (!msg_ptr)
			{
				return;
			}
			m_tail_msg_ptr = msg_ptr->get_next_msg();
			--m_size;
		}
	}
} // namespace chen
