/***********************************************************************************************
created: 		2019-06-16

author:			chensong

purpose:		fd_set_adapter
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
#ifndef _C_FD_SET_ADAPTER_H_
#define _C_FD_SET_ADAPTER_H_
#include "cnet_types.h"
#if defined(_MSC_VER)

namespace chen {
	class cfd_set_adapter
	{
	private:
		enum { default_fd_set_size = 1024 };
		struct cfd_set {
			uint32		m_count; //typedef unsigned int    u_int;
			socket_type m_fd_array[1];//��̬���������С
		};
	public:
		explicit cfd_set_adapter();
		~cfd_set_adapter();
		
	public:
		bool	init();
		void	destroy();
	public:
		bool set(socket_type descriptor);
		
		// FD_ISSET
		bool is_set(socket_type descriptor) const;

		void remove(socket_type descriptor);

		// []
		operator fd_set*();
		
		void reset();


		socket_type max_descriptor() const
		{
			return m_max_descriptor_fd;
		}
	private:
		//����
		void reserve(uint32 size);
	private:
		cfd_set_adapter(const cfd_set_adapter&);
		cfd_set_adapter& operator=(const cfd_set_adapter&);
	private:
		cfd_set*			m_fd_set;
		uint32				m_capacity;// ����Ĵ�С
		uint32				m_max_descriptor_fd;
	};
} // namespace chen
#endif // #if defined(_MSC_VER)
#endif // !#define _C_FD_SET_ADAPTER_H_