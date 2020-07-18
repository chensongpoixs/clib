/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		cwinapi_thread
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

#ifndef _C_WINAPI_THREAD_H_
#define _C_WINAPI_THREAD_H_
#include "cnoncopyable.h"
#if defined(_MSC_VER)
#include <Windows.h>
#include <memory>
namespace chen {

	DWORD WINAPI cwinapi_thread_function(LPVOID arg);

	class cwinapi_thread
		: private cnoncopyable
	{
	public:
		// Constructor.
		template <typename Function>
		cwinapi_thread(Function f, unsigned int = 0)
		{
			std::auto_ptr<func_base> arg(new func<Function>(f));
			DWORD thread_id = 0;
			thread_ = ::CreateThread(0, 0, cwinapi_thread_function,
				arg.get(), 0, &thread_id);
			if (!thread_)
			{
				DWORD last_error = ::GetLastError();
				//�׳��쳣
				/*boost::system::error_code ec(last_error,
					boost::asio::error::get_system_category());
				boost::asio::detail::throw_error(ec, "thread");*/
			}
			arg.release();
		}

		// Destructor.
		~cwinapi_thread()
		{
			::CloseHandle(thread_);
		}

		// Wait for the thread to exit.
		void join()
		{
#if defined(WINDOWS_APP)
			::WaitForSingleObjectEx(thread_, INFINITE, false);
#else // defined(WINDOWS_APP)
			::WaitForSingleObject(thread_, INFINITE);
#endif // defined(WINDOWS_APP)
		}

	private:
		friend DWORD WINAPI cwinapi_thread_function(LPVOID arg);

		class func_base
		{
		public:
			virtual ~func_base() {}
			virtual void run() = 0;
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

		::HANDLE thread_;
	};

	inline DWORD WINAPI cwinapi_thread_function(LPVOID arg)
	{
		std::auto_ptr<cwinapi_thread::func_base> func(static_cast<cwinapi_thread::func_base*>(arg));
		func->run();
		return 0;
	}
}
#endif // !#if defined(_MSC_VER)


#endif //!_C_WINAPI_THREAD_H_