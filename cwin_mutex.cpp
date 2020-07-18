/***********************************************************************************************
created: 		2020-05-17

author:			chensong

purpose:		cwin_mutex
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

#include <iostream>
#if defined(_MSC_VER)
#include "cnoncopyable.h"
#include <Windows.h>
#include "cwin_mutex.h"

namespace chen {

	cwin_mutex::cwin_mutex()
	{
		int error = do_init();
		/*boost::system::error_code ec(error,
			boost::asio::error::get_system_category());
		boost::asio::detail::throw_error(ec, "mutex");*/
	}

	int cwin_mutex::do_init()
	{
#if defined(__MINGW32__)
		// Not sure if MinGW supports structured exception handling, so for now
		// we'll just call the Windows API and hope.
# if defined(UNDER_CE)
		::InitializeCriticalSection(&crit_section_);
# elif defined(BOOST_ASIO_WINDOWS_APP)
		::InitializeCriticalSectionEx(&crit_section_, 0x80000000, 0);
# else
		if (!::InitializeCriticalSectionAndSpinCount(&crit_section_, 0x80000000))
			return ::GetLastError();
# endif
		return 0;
#else
		__try
		{
# if defined(UNDER_CE)
			::InitializeCriticalSection(&crit_section_);
# elif defined(BOOST_ASIO_WINDOWS_APP)
			if (!::InitializeCriticalSectionEx(&crit_section_, 0, 0))
				return ::GetLastError();
# else
			if (!::InitializeCriticalSectionAndSpinCount(&crit_section_, 0x80000000))
				return ::GetLastError();
# endif
		}
		__except (GetExceptionCode() == STATUS_NO_MEMORY
			? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			return ERROR_OUTOFMEMORY;
		}

		return 0;
	}
#endif
} // 



#endif // !#if defined(_MSC_VER)
