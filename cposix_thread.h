/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		cposix_thread
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

#ifndef _C_POSIX_THREAD_H_
#define _C_POSIX_THREAD_H_
#include "cnoncopyable.h"
#if defined(__linux__)
#include <pthread.h>
namespace chen {
	extern "C"
	{
		void* cposix_thread_function(void* arg);
	}

	class cposix_thread
		: private cnoncopyable
	{
	public:
		// Constructor.
		template <typename Function>
		cposix_thread(Function f, unsigned int = 0)
			: joined_(false)
		{
			start_thread(new func<Function>(f));
		}

		// Destructor.
		~cposix_thread();

		// Wait for the thread to exit.
		void join();

	private:
		friend void* cposix_thread_function(void* arg);

		class func_base
		{
		public:
			virtual ~func_base() {}
			virtual void run() = 0;
		};

		struct auto_func_base_ptr
		{
			func_base* ptr;
			~auto_func_base_ptr() { delete ptr; }
		};

		template <typename Function>
		class func
			: public func_base
		{
		public:
			func(Function f)
				: f_(f)
			{
			}

			virtual void run()
			{
				f_();
			}

		private:
			Function f_;
		};

		void start_thread(func_base* arg);

		::pthread_t thread_;
		bool joined_;
	};
}
#endif // !#if defined(__linux__)#endif // 
#endif // !#define _C_POSIX_THREAD_H_