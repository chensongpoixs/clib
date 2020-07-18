/***********************************************************************************************
created: 		2020-05-17

author:			chensong

purpose:		cwin_event
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
#include "cwin_event.h"
#if defined(_MSC_VER)
namespace chen {

	cwin_event::cwin_event()
		: state_(0)
	{
#if defined(BOOST_ASIO_WINDOWS_APP)
		events_[0] = ::CreateEventExW(0, 0, CREATE_EVENT_MANUAL_RESET, 0);
#else // defined(BOOST_ASIO_WINDOWS_APP)
		events_[0] = ::CreateEventW(0, true, false, 0);
#endif // defined(BOOST_ASIO_WINDOWS_APP)
		if (!events_[0])
		{
			DWORD last_error = ::GetLastError();
			/*boost::system::error_code ec(last_error,
				boost::asio::error::get_system_category());
			boost::asio::detail::throw_error(ec, "event");*/
		}

#if defined(BOOST_ASIO_WINDOWS_APP)
		events_[1] = ::CreateEventExW(0, 0, 0, 0);
#else // defined(BOOST_ASIO_WINDOWS_APP)
		events_[1] = ::CreateEventW(0, false, false, 0);
#endif // defined(BOOST_ASIO_WINDOWS_APP)
		if (!events_[1])
		{
			DWORD last_error = ::GetLastError();
			::CloseHandle(events_[0]);
			/*boost::system::error_code ec(last_error,
				boost::asio::error::get_system_category());
			boost::asio::detail::throw_error(ec, "event");*/
		}
	}

	cwin_event::~cwin_event()
	{
		::CloseHandle(events_[0]);
		::CloseHandle(events_[1]);
	}

}
#endif