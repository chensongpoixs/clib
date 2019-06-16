/***********************************************************************************************
created: 		2019-06-16

author:			chensong

purpose:		fd_set_adapter
************************************************************************************************/
#ifndef _C_FD_SET_ADAPTER_H_
#define _C_FD_SET_ADAPTER_H_
#include "cnet_types.h"
#if defined(_MSC_VER)

namespace chen {
	class cfd_set_adapter
	{
	private:
		enum { default_fd_set_size = 1024 };
		struct _fd_set {
			uint32		m_count; //typedef unsigned int    u_int;
			socket_type m_fd_array[1];//动态申请数组大小
		};
	public:
		explicit cfd_set_adapter();
		~cfd_set_adapter();
	public:
		bool	init();
		void	destroy();
	public:
		bool set(socket_type descriptor);
		
		// FD_ISSET
		bool is_set(socket_type descriptor) const;

		// []
		operator fd_set*();
		
		void reset();


		socket_type max_descriptor() const
		{
			return m_max_descriptor_fd;
		}
	private:
		//扩容
		void reserve(uint32 size);
	private:
		cfd_set_adapter(const cfd_set_adapter&);
		cfd_set_adapter& operator=(const cfd_set_adapter&);
	private:
		_fd_set*			m_fd_set;
		uint32				m_capacity;// 数组的大小
		uint32				m_max_descriptor_fd;
	};
} // namespace chen
#endif // #if defined(_MSC_VER)
#endif // !#define _C_FD_SET_ADAPTER_H_