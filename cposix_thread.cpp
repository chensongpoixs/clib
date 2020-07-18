/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		cposix_thread
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


#if defined(__linux__)
#include "cposix_thread.h"
namespace chen {
	cposix_thread::~cposix_thread()
	{
		if (!joined_)
		{
			::pthread_detach(thread_);
		}
	}

	void cposix_thread::join()
	{
		if (!joined_)
		{
			::pthread_join(thread_, 0);
			joined_ = true;
		}
	}

	void cposix_thread::start_thread(func_base* arg)
	{
		int error = ::pthread_create(&thread_, 0, cposix_thread_function, arg);
		if (error != 0)
		{
			delete arg;
			// TODO@chensong 2020-05-16
			/*boost::system::error_code ec(error,
				boost::asio::error::get_system_category());
			boost::asio::detail::throw_error(ec, "thread");*/
		}
	}

	void* cposix_thread_function(void* arg)
	{
		cposix_thread::auto_func_base_ptr func = {
			static_cast<cposix_thread::func_base*>(arg) };
		func.ptr->run();
		return 0;
	}
}
#endif // !#if defined(__linux__)#endif // 