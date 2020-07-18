/***********************************************************************************************
created: 		2019-12-27

author:			chensong

purpose:		cmsg_buffer
�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧����������ֻᱻ��������ں���������ĳ�������ҹ������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶���ߡ�
�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
************************************************************************************************/
#include "cmsg_buffer.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
namespace chen {
	cmsg_send_buffer::cmsg_send_buffer(uint32 max_size)
		: m_max_size(max_size)
		, m_size(0)
		, m_send_size(0)
	{
	}
	cmsg_send_buffer::~cmsg_send_buffer()
	{
	}
	cmsg_send_buffer * cmsg_send_buffer::alloc_me(uint32 size)
	{
		void * p = ::malloc(sizeof(cmsg_send_buffer)- 1 + size);
		if (!p)
		{
			return NULL;
		}
		cmsg_send_buffer * q = new(p) cmsg_send_buffer(size);
		
		return q;
	}
	void cmsg_send_buffer::free_me(cmsg_send_buffer * ptr)
	{
		::free(ptr);
	}
	uint32 cmsg_send_buffer::append(const void * p, uint32 len)
	{
		if (len > avail())
		{
			len = avail();
		}
		memcpy(m_buffer + m_size, p, len);
		m_size += len;
		return len;
	}
	void cmsg_send_buffer::reset()
	{
		m_size = 0;
		m_send_size = 0;
	}
	/*void cmsg_send_buffer::resert()
	{
	}*/
	cmsg_receive_buffer::cmsg_receive_buffer(uint32 size)
		: m_size(size)
		, m_data_size(0)
		/*, m_send_size(0)*/
	{
	}
	cmsg_receive_buffer::~cmsg_receive_buffer()
	{
	}
	cmsg_receive_buffer * cmsg_receive_buffer::alloc_me(uint32 size)
	{
		void * p = ::malloc(sizeof(cmsg_receive_buffer) - 1 + size);
		if (!p)
		{
			return NULL;
		}
		cmsg_receive_buffer * q = new(p) cmsg_receive_buffer(size);
		return q;
	}
	void cmsg_receive_buffer::free_me(cmsg_receive_buffer * ptr)
	{
		::free(ptr);
	}
	uint32 cmsg_receive_buffer::append(void * p, uint32 len)
	{
		if (len > avail())
		{
			len = avail();
		}
		memcpy(m_buffer + m_data_size, p, len);
		m_data_size += len;
		return len;
	}
	void cmsg_receive_buffer::resert()
	{
		m_data_size = 0;
		/*m_send_size = 0;*/
	}
} // !namespace chen

