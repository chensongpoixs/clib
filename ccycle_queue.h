/********************************************************************
	created:	2015/04/10

	author:		chensong
	desc:		cycle queue with fix size
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

#ifndef _C_CYCLE_QUEUE_H_
#define _C_CYCLE_QUEUE_H_

namespace chen
{
	template<typename T>
	class ccycle_queue
	{
		//-- constructor/destructor
	public:
		ccycle_queue();
		~ccycle_queue();

		//-- member function
	public:
		//-- prop
		bool init(int uMaxNum);
		void destroy();

		bool empty() const { return (0 == m_size); }
		bool full() const { return (m_size == m_maxSize); }
		int	size() const { return m_size; }
		int	get_max_size() const { return m_maxSize; }

		bool push(const T& data);
		bool pop(T& data);
		void clear();

	private:
		ccycle_queue(const ccycle_queue&);
		ccycle_queue& operator=(const ccycle_queue&);

	private:
		T*	m_datas;
		int	m_maxSize;
		int m_size;
		int	m_head;
		int	m_tail;
	};

	template<typename T>
	ccycle_queue<T>::ccycle_queue() : m_datas(NULL), m_maxSize(0), m_size(0), m_head(0), m_tail(0)
	{

	}

	template<typename T>
	ccycle_queue<T>::~ccycle_queue()
	{
		destroy();
	}

	template<typename T>
	void ccycle_queue<T>::destroy()
	{
		if (m_datas)
		{
			delete[] m_datas;
			m_datas = NULL;
		}
		m_maxSize = 0;
		m_head = 0;
		m_tail = 0;
	}

	template<typename T>
	bool ccycle_queue<T>::init(int uMaxNum)
	{
		m_datas = new T[uMaxNum];
		if (!m_datas)
		{
			return false;
		}
		m_maxSize = uMaxNum;
		return true;
	}
	
	template<typename T>
	bool ccycle_queue<T>::push(const T& data)
	{
		if (!full())
		{
			m_datas[m_tail] = data;
			m_tail = (m_tail + 1) % m_maxSize;
			++m_size;
			return true;
		}
		return false;
	}

	template<typename T>
	bool ccycle_queue<T>::pop(T& data)
	{
		if (!empty())
		{
			data = m_datas[m_head];
			m_head = (m_head + 1) % m_maxSize;
			--m_size;
			return true;
		}
		return false;
	}

	template<typename T>
	void ccycle_queue<T>::clear()
	{
		m_size = 0;
		m_head = 0;
		m_tail = 0;
	}

}
#endif //!#define _C_CYCLE_QUEUE_H_