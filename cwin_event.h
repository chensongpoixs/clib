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


#ifndef _C_WIN_EVENT_H_
#define _C_WIN_EVENT_H_


#include <iostream>
#if defined(_MSC_VER)
#include "cnoncopyable.h"
#include <Windows.h>

namespace chen {

	class cwin_event : private cnoncopyable
	{
	public:
		// Constructor.
		cwin_event();

		// Destructor.
		~cwin_event();

		// Signal the event. (Retained for backward compatibility.)
		template <typename Lock>
		void signal(Lock& lock)
		{
			this->signal_all(lock);
		}

		// Signal all waiters.
		template <typename Lock>
		void signal_all(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			(void)lock;
			state_ |= 1;
			::SetEvent(events_[0]);
		}

		// Unlock the mutex and signal one waiter.
		template <typename Lock>
		void unlock_and_signal_one(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			state_ |= 1;
			bool have_waiters = (state_ > 1);
			lock.unlock();
			if (have_waiters)
				::SetEvent(events_[1]);
		}

		// If there's a waiter, unlock the mutex and signal it.
		template <typename Lock>
		bool maybe_unlock_and_signal_one(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			state_ |= 1;
			if (state_ > 1)
			{
				lock.unlock();
				::SetEvent(events_[1]);
				return true;
			}
			return false;
		}

		// Reset the event.
		template <typename Lock>
		void clear(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			(void)lock;
			::ResetEvent(events_[0]);
			state_ &= ~std::size_t(1);
		}

		// Wait for the event to become signalled.
		template <typename Lock>
		void wait(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			while ((state_ & 1) == 0)
			{
				state_ += 2;
				lock.unlock();
#if defined(BOOST_ASIO_WINDOWS_APP)
				::WaitForMultipleObjectsEx(2, events_, false, INFINITE, false);
#else // defined(BOOST_ASIO_WINDOWS_APP)
				::WaitForMultipleObjects(2, events_, false, INFINITE);
#endif // defined(BOOST_ASIO_WINDOWS_APP)
				lock.lock();
				state_ -= 2;
			}
		}

	private:
		HANDLE events_[2];
		std::size_t state_;
	};

} // 
#endif
#endif // !#define _C_WIN_EVENT_H_