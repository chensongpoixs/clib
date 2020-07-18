/***********************************************************************************************
created: 		2020-05-17

author:			chensong

purpose:		cstd_event
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


#ifndef _C_STD_EVENT_H_
#define _C_STD_EVENT_H_


#include <iostream>
#if defined(__APPLE__)
#include "cnoncopyable.h"
#include <chrono>
#include <condition_variable>
namespace chen {

	class cstd_event : private cnoncopyable
	{
	public:
		// Constructor.
		cstd_event()
			: state_(0)
		{
		}

		// Destructor.
		~cstd_event()
		{
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
			//BOOST_ASIO_ASSERT(lock.locked());
			(void)lock;
			state_ |= 1;
			cond_.notify_all();
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
				cond_.notify_one();
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
				cond_.notify_one();
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
			state_ &= ~std::size_t(1);
		}

		// Wait for the event to become signalled.
		template <typename Lock>
		void wait(Lock& lock)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			unique_lock_adapter u_lock(lock);
			while ((state_ & 1) == 0)
			{
				waiter w(state_);
				cond_.wait(u_lock.unique_lock_);
			}
		}

		// Timed wait for the event to become signalled.
		template <typename Lock>
		bool wait_for_usec(Lock& lock, long usec)
		{
			//BOOST_ASIO_ASSERT(lock.locked());
			unique_lock_adapter u_lock(lock);
			if ((state_ & 1) == 0)
			{
				waiter w(state_);
				cond_.wait_for(u_lock.unique_lock_, std::chrono::microseconds(usec));
			}
			return (state_ & 1) != 0;
		}

	private:
		// Helper class to temporarily adapt a scoped_lock into a unique_lock so that
		// it can be passed to std::condition_variable::wait().
		struct unique_lock_adapter
		{
			template <typename Lock>
			explicit unique_lock_adapter(Lock& lock)
				: unique_lock_(lock.mutex().mutex_, std::adopt_lock)
			{
			}

			~unique_lock_adapter()
			{
				unique_lock_.release();
			}

			std::unique_lock<std::mutex> unique_lock_;
		};

		// Helper to increment and decrement the state to track outstanding waiters.
		class waiter
		{
		public:
			explicit waiter(std::size_t& state)
				: state_(state)
			{
				state_ += 2;
			}

			~waiter()
			{
				state_ -= 2;
			}

		private:
			std::size_t& state_;
		};

		std::condition_variable cond_;
		std::size_t state_;
	};

} // 
#endif
#endif // !#define _C_WIN_EVENT_H_