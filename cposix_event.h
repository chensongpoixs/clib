/***********************************************************************************************
created: 		2020-05-17

author:			chensong

purpose:		cposix_event
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


#ifndef _C_POSIX_EVENT_H_
#define _C_POSIX_EVENT_H_


#include <iostream>
#if defined(__linux__)
#include "cnoncopyable.h"
#include <pthread.h>
namespace chen {

	class cposix_event
		: private cnoncopyable
	{
	public:
		// Constructor.
		cposix_event();

		// Destructor.
		~cposix_event()
		{
			::pthread_cond_destroy(&cond_);
		}

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
		//	BOOST_ASIO_ASSERT(lock.locked());
			(void)lock;
			state_ |= 1;
			::pthread_cond_broadcast(&cond_); // Ignore EINVAL.
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
			{
				::pthread_cond_signal(&cond_); // Ignore EINVAL.
			}
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
				::pthread_cond_signal(&cond_); // Ignore EINVAL.
				return true;
			}
			return false;
		}

		// Reset the event.
		template <typename Lock>
		void clear(Lock& lock)
		{
		//	BOOST_ASIO_ASSERT(lock.locked());
			(void)lock;
			state_ &= ~std::size_t(1);// -- 1
		}

		// Wait for the event to become signalled.
		template <typename Lock>
		void wait(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			while ((state_ & 1) == 0)
			{
				state_ += 2;
				::pthread_cond_wait(&cond_, &lock.mutex().mutex_); // Ignore EINVAL.
				state_ -= 2;
			}
		}

	private:
		::pthread_cond_t cond_;
		std::size_t state_;
	};

} // 
#endif
#endif // !#define _C_POSIX_EVENT_H_