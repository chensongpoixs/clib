/***********************************************************************************************
created: 		2019-06-16

author:			chensong

purpose:		fd_set_adapter
************************************************************************************************/
#include "cfd_set_adapter.h"
#include "csocket_ops.h"
#include "clog.h"
#if defined(_MSC_VER)
namespace chen {
	cfd_set_adapter::cfd_set_adapter()
		: m_fd_set(NULL)
		, m_capacity(default_fd_set_size)
		, m_max_descriptor_fd(0)
	{
	}
	cfd_set_adapter::~cfd_set_adapter()
	{
	}
	bool cfd_set_adapter::init()
	{
		m_fd_set = static_cast<_fd_set*>(::operator new(
			sizeof(_fd_set) - sizeof(socket_type)
			+ sizeof(socket_type) * (m_capacity)));
		m_fd_set->m_count = 0;
		
		return true;
	}
	void cfd_set_adapter::destroy()
	{
		::operator delete(m_fd_set);
		m_max_descriptor_fd = 0;
		m_capacity = 0;
	}
	bool cfd_set_adapter::set(socket_type descriptor)
	{
		//ERROR_LOG("count = %lu", m_fd_set.m_count);
		for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			if (m_fd_set->m_fd_array[i] == descriptor)
			{
				return true;
			}
		}

		reserve(m_fd_set->m_count + 1);
		m_fd_set->m_fd_array[m_fd_set->m_count++] = descriptor;
	/*	for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			if (m_fd_set.m_fd_array[i] == descriptor)
			{
				return true;
			}
		}
		if (m_fd_set.m_count < default_fd_set_size)
		{
			m_fd_set.m_fd_array[m_fd_set.m_count++] = descriptor;
			return true;
		}*/
		return true;
	}
	bool cfd_set_adapter::is_set(socket_type descriptor) const
	{
		//int __stdcall _WSAFDIsSet(SOCKET fd, fd_set *a2)
		//{
		//	int result; // eax@1 
		//	u_int v3; // ecx@1 
		//	char *v4; // edx@2 

		//	v3 = a2->fd_count;
		//	result = 0;
		//	if (a2->fd_count)
		//	{
		//		v4 = (char *)&a2->fd_array[v3];
		//		do
		//		{
		//			v4 -= 4;
		//			if (*(_DWORD *)v4 == fd)
		//				result = 1;
		//			--v3;
		//		} while (v3);
		//	}
		//	return result;
		//}
		return !!__WSAFDIsSet(descriptor,
			const_cast<fd_set*>(reinterpret_cast<const fd_set*>(m_fd_set)));
	}
	
	cfd_set_adapter::operator fd_set*()
	{
		return reinterpret_cast<fd_set*>(m_fd_set);
	}

	void cfd_set_adapter::reset()
	{
		m_fd_set->m_count = 0;
		m_max_descriptor_fd = 0;
	}

	void cfd_set_adapter::reserve(uint32 size)
	{
		
		if (size <= m_capacity)
		{
			return;
		}
		// ¶¯Ì¬À©ÈÝ
		uint32 new_capacity = m_capacity + m_capacity / 2;
		if (new_capacity < size)
		{
			new_capacity = size;
		}

		_fd_set* new_fd_set = static_cast<_fd_set*>(::operator new(
			sizeof(_fd_set) - sizeof(socket_type)
			+ sizeof(socket_type) * (new_capacity)));

		new_fd_set->m_count = m_fd_set->m_count;
		for (uint32 i = 0; i < m_fd_set->m_count; ++i)
		{
			new_fd_set->m_fd_array[i] = m_fd_set->m_fd_array[i];
		}

		::operator delete(m_fd_set);
		m_fd_set = new_fd_set;
		m_capacity = new_capacity;
	}
} // namespace chen
#endif // #if defined(_MSC_VER)