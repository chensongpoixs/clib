/***********************************************************************************************
	created: 		2019-02-26
	
	author:			chensong
					
	purpose:		queue
	
	Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
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

#ifndef _C_OBJECT_POOL_H_
#define _C_OBJECT_POOL_H_

#include "cnoncopyable.h"

namespace chen {

template <typename Object>
class cobject_pool;

class cobject_pool_access
{
public:
  template <typename Object>
  static Object* create()
  {
    return new Object;
  }

  template <typename Object>
  static void destroy(Object* o)
  {
    delete o;
  }

  template <typename Object>
  static Object*& next(Object* o)
  {
    return o->next_;
  }

  template <typename Object>
  static Object*& prev(Object* o)
  {
    return o->prev_;
  }
};

template <typename Object>
class cobject_pool
  : private cnoncopyable
{
public:
  // Constructor.
  cobject_pool()
    : live_list_(0),
      free_list_(0)
  {
  }

  // Destructor destroys all objects.
  ~cobject_pool()
  {
    destroy_list(live_list_);
    destroy_list(free_list_);
  }

  // Get the object at the start of the live list.
  Object* first()
  {
    return live_list_;
  }

  // Allocate a new object.
  Object* alloc()
  {
    Object* o = free_list_;
    if (o)
      free_list_ = cobject_pool_access::next(free_list_);
    else
      o = cobject_pool_access::create<Object>();

    cobject_pool_access::next(o) = live_list_;
    cobject_pool_access::prev(o) = 0;
    if (live_list_)
      cobject_pool_access::prev(live_list_) = o;
    live_list_ = o;

    return o;
  }

  // Free an object. Moves it to the free list. No destructors are run.
  void free(Object* o)
  {
    if (live_list_ == o)
      live_list_ = cobject_pool_access::next(o);

    if (cobject_pool_access::prev(o))
    {
      cobject_pool_access::next(cobject_pool_access::prev(o))
        = cobject_pool_access::next(o);
    }

    if (cobject_pool_access::next(o))
    {
      cobject_pool_access::prev(cobject_pool_access::next(o))
        = cobject_pool_access::prev(o);
    }

    cobject_pool_access::next(o) = free_list_;
    cobject_pool_access::prev(o) = 0;
    free_list_ = o;
  }

private:
  // Helper function to destroy all elements in a list.
  void destroy_list(Object* list)
  {
    while (list)
    {
      Object* o = list;
      list = cobject_pool_access::next(o);
      cobject_pool_access::destroy(o);
    }
  }

  // The list of live objects.
  Object* live_list_;

  // The free list.
  Object* free_list_;
};

} // namespace chen
#endif // _C_OBJECT_POOL_H_
