/***********************************************************************************************
		created: 		2019-04-06

		author:			chensong

		purpose:		select_reactor
************************************************************************************************/


#ifndef _C_SELECT_REACTOR_H_
#define _C_SELECT_REACTOR_H_
#include "cnoncopyable.h"
#include "cnet_types.h"
#include "cnet_session.h"
#include "cfd_set_adapter.h"
#include <set>
#include <vector>
#if defined(_MSC_VER)


namespace chen {
	class cselect_reactor /*:private cnoncopyable*/
	{
	private:

		enum op_types {
			read_op = 0, write_op = 1, except_op = 2,
			max_select_ops = 3, connect_op = 3, max_ops = 4
		};

		struct csocket_exter_para
		{
			socket_type		socket;
			void*			ptr;
			int32			index;
			csocket_exter_para() : socket(INVALID_SOCKET), ptr(NULL), index(0){}
			void reset()
			{
				socket = INVALID_SOCKET;
				ptr = NULL;
				index = 0;
			}
		};
		struct csocket_active_para
		{
			uint32	events;
			void*	ptr;
		};

	public:
		explicit cselect_reactor();
		~cselect_reactor() {}
	public:
		
	public:
		bool init(socket_type listenfds,uint32 active_num);
		void destroy();
	public:
		void * get_para(uint32 index);
	public:
		bool	is_read(uint32 index) const ;
		bool	is_write(uint32 index)const;
		bool	is_exception(uint32 index) const;
		
	public:
		// register
		/**
		*  @param  descriptor	:  ע���ȡ�ļ���������select��    select --->  λͼ 
		*  @param  para			:  ��Ӧ�ѵĴ洢�ػ���ָ��   
		**/
		void	register_read_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_READFDS, para); }
		void	register_write_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_WRITFDS, para); }
		void	register_readwrite_descriptor(socket_type& descriptor, void* para) { _register_descriptor(descriptor, E_READFDS | E_WRITFDS, para); }
		// remove file
		/**
		*   ע�� �ļ�������
		*   @param  descriptor		: �ļ�������
		**/
		void deregister_descriptor(socket_type& descriptor);
		/*void shutdown();*/
	public:
		int32 select(uint32 ms);
	private:
		int32 _register_descriptor(socket_type& descriptor, uint32 st, void * session);
	private:
		cselect_reactor(const cselect_reactor&);
		cselect_reactor& operator=(const cselect_reactor&);
	private:
		socket_type 				m_maxfd;         // ���ļ����� listen
		uint32						m_curfd_count;  //��ǰ������
		uint32						m_maxfd_count;  // �����������
		cfd_set_adapter				m_fd_set_adapter[max_select_ops];
		cfd_set_adapter				m_active_fd_set_adapter[max_select_ops];
		csocket_exter_para*			m_paras;      // �¼�����
		csocket_active_para*		m_active_paras;  //��Ӧ������
	};

} // namespace chen
#endif // #if defined(_MSC_VER)
#endif //!#define _C_SELECT_REACTOR_H_