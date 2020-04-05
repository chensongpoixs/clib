/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	�ַ�����������
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
*********************************************************************/
#include "cdigit2str.h"
#include <algorithm>
#include <cstdio>

#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	template<typename T>
	int digit2str_dec(char* buf, int buf_size, T value)
	{
		T i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}


	template<>
	int digit2str_dec<float>(char* buf, int buf_size, float value)
	{
		if (value < 0)
		{
			if (buf_size < 9)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		else
		{
			if (buf_size < 8)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		
		return sprintf(buf, "%.2g", value);
	}

	template<>
	int digit2str_dec<double>(char* buf, int buf_size, double value)
	{
		if (value < 0)
		{
			if (buf_size < 9)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		else
		{
			if (buf_size < 8)  // 7 + num , 8 - num
			{
				return 0;
			}
		}
		return sprintf(buf, "%.2g", value);
	}

	
} // namespace chen