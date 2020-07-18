/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		cthread
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


#ifndef _C_THREAD_H_
#define _C_THREAD_H_

#if defined(HAS_THREADS)
# include "cnull_thread.h"
#elif defined(_MSC_VER)
//# if defined(BOOST_ASIO_WINDOWS_APP) || defined(UNDER_CE)
#  include "cwinapi_thread.h"
//# else
//#  include <boost/asio/detail/win_thread.hpp>
//# endif
#elif defined(__linux__)
# include "cposix_thread.h"
#elif defined(STD_THREAD)
# include "cstd_thread.h"
#else
# error Only Windows, POSIX and std::thread are supported!
#endif


namespace chen {
#if defined(HAS_THREADS)
	typedef cnull_thread cthread;
#elif defined(_MSC_VER)
//# if defined(BOOST_ASIO_WINDOWS_APP) || defined(UNDER_CE)
	typedef cwinapi_thread cthread;
//# else
	//typedef win_thread cthread;
//# endif
#elif defined(__linux__)
	typedef cposix_thread cthread;
#elif defined(HAS_STD_THREAD)
	typedef cstd_thread cthread;
#endif

} // 

#endif // !#define _C_THREAD_H_