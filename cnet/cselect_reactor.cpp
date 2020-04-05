/***********************************************************************************************
			created: 		2019-04-06

			author:			chensong

			purpose:		select_reactor
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

#include "cselect_reactor.h"
#include "clog.h"
#include "csocket_ops.h"
#if defined(_MSC_VER)

namespace chen {
	cselect_reactor::cselect_reactor()
		:	m_paras(NULL)
		,	m_active_paras(NULL)
	{	 
	}	 
	bool cselect_reactor::init(socket_type listenfds, uint32 active_num)
	{
		if (active_num == 0 || active_num  > 0xFFFF)
		{
			ERROR_EX_LOG(" max fd you not active_num =%lu", active_num);
			return false;
		}
		
		for (int32 i = 0; i < max_select_ops; ++i)
		{
			if (!m_fd_set_adapter[i].init())
			{
				ERROR_LOG(" fd set adapter init error");
				return false;
			}
			if (!m_active_fd_set_adapter[i].init())
			{
				ERROR_LOG("active fd set adapter init error");
				return false;
			}
		
		}
		
		m_paras = new csocket_exter_para[active_num];
		if (!m_paras)
		{
			ERROR_EX_LOG(" alloc exter para active_num = %u", active_num);
			return false;
		}

		m_active_paras = new csocket_active_para[active_num];
		if (!m_active_paras)
		{
			ERROR_EX_LOG(" alloc active para maxnum = %lu", active_num);
			return false;
		}

		
		m_curfd_count = 0;
		m_maxfd_count = active_num;
		m_maxfd = listenfds;
		return true;
	}
	void cselect_reactor::destroy()
	{
		for (int32 i = 0; i < max_select_ops; ++i)
		{
			m_fd_set_adapter[i].destroy();
			m_active_fd_set_adapter[i].destroy();
		}
	
		if (m_paras)
		{
			delete[] m_paras;
			m_paras = NULL;
		}
		if (m_active_paras)
		{
			delete[] m_active_paras;
			m_active_paras = NULL;
		}
		m_maxfd_count = 0;
		m_maxfd = 0;
	}
	void * cselect_reactor::get_para(uint32 index)
	{
		return m_active_paras[index].ptr;
	}
	bool cselect_reactor::is_read(uint32 index) const
	{
		return  ( 0 != (m_active_paras[index].events & E_READFDS) );
	}
	bool cselect_reactor::is_write(uint32 index) const
	{
		return (0 != (m_active_paras[index].events & E_WRITFDS));
	}
	bool cselect_reactor::is_exception(uint32 index) const
	{
		return (0 != (m_active_paras[index].events & E_EXCEFDS));
	}
	int32 cselect_reactor::_register_descriptor(socket_type& descriptor, uint32 st, void * session)
	{
		//��Ҫ�޸� m_maxfd ��ֵ
		if (descriptor == INVALID_SOCKET)
		{
			ERROR_EX_LOG("descriptor is invalid");
			return -1;
		}

		if (m_curfd_count >= m_maxfd_count)
		{
			ERROR_EX_LOG("reach max fd count %lu", m_maxfd_count);
			return -1;
		}
		if (( st& E_READFDS) != 0)
		{
			if (m_fd_set_adapter[read_op].is_set(descriptor))
			{
				ERROR_EX_LOG("socket has been set");
				return -1;
			}
			m_fd_set_adapter[read_op].set(descriptor);
		}
		// cur_
		m_fd_set_adapter[write_op].set(descriptor);
		m_fd_set_adapter[except_op].set(descriptor);
	
		m_maxfd = (m_maxfd == INVALID_SOCKET ) ? descriptor : max(descriptor, m_maxfd);
	
		m_paras[m_curfd_count].socket = descriptor;
		m_paras[m_curfd_count].ptr = session;
		++m_curfd_count;
		return 0;
	}
	void cselect_reactor::deregister_descriptor(socket_type& descriptor)
	{
		//��Ҫ�޸� m_maxfd ��ֵ
		if (descriptor == INVALID_SOCKET)
		{
			ERROR_EX_LOG("ERROR net_mgr::deregister_descriptor descriptor (socket_type=%d)", descriptor);
			return;
		}

		if (m_curfd_count <= 0)
		{
			ERROR_EX_LOG("m_curfd_count < 0 when deregister_descriptor descriptor (socket_type=%d)", descriptor);
			return;
		}
		if (m_fd_set_adapter[read_op].is_set(descriptor))
		{
			ERROR_EX_LOG("ERROR net_mgr::deregister_descriptor(socket_type=%d)", descriptor);
			return;
		}
		FD_CLR(descriptor, m_fd_set_adapter[read_op]);
		FD_CLR(descriptor, m_fd_set_adapter[write_op]);
		FD_CLR(descriptor, m_fd_set_adapter[except_op]);

		//Ѱ�����socked
		m_maxfd = INVALID_SOCKET;
		int32 descriptor_index = -1;
		for (int32 i = 0; i < m_curfd_count; ++i)
		{
			//Ѱ��fd
			if (m_paras[i].socket == descriptor)
			{
				descriptor_index = i;
			}
			else if (m_maxfd == INVALID_SOCKET || m_paras[i].socket > m_maxfd)
			{
				//Ѱ�����socket
				m_maxfd = m_paras[i].socket;
			}
		}
		m_curfd_count--;
		if (descriptor_index == -1)
		{
			ERROR_EX_LOG("socket not found");
			return;
		}
		//�����һ��socket�ƶ���ɾ�����Ǹ�socketλ��
		if (descriptor_index != m_curfd_count)
		{
			m_paras[descriptor_index] = m_paras[m_curfd_count];
			m_paras[m_curfd_count].reset();
		}
	}
	/*void cselect_proactor::shutdown()
	{
	}*/
	int32 cselect_reactor::select(uint32 ms)
	{
		if (m_curfd_count == 0)
		{
			return 0;
		}
		struct timeval time_out;
		time_out.tv_sec = 0;
		time_out.tv_usec = ms * 1000;
		
		
		m_active_fd_set_adapter[read_op].reset();
		m_active_fd_set_adapter[write_op].reset();
		m_active_fd_set_adapter[except_op].reset();
		for (uint32 j = 0; j < m_curfd_count; ++j)
		{
			m_active_fd_set_adapter[read_op].set(m_paras[j].socket);
			m_active_fd_set_adapter[write_op].set(m_paras[j].socket);
			m_active_fd_set_adapter[except_op].set(m_paras[j].socket);
		}
		
		int32	num_event = 0;
	//	SYSTEM_LOG("m_maxfd = %d", m_maxfd);
		// �����Ż� write �ļ����� ʹ�� IOʹ�������߳�  select ʹ��һ���߳� ������select�Ϻ�epoll_wait ����һ���߳�ר�Ŷ�ȡ��д��Ĳ���
		int32 numevent = csocket_ops::select(m_maxfd + 1, m_active_fd_set_adapter[read_op], m_active_fd_set_adapter[write_op], m_active_fd_set_adapter[except_op] /*&readfds, &writefds, &excefds*/,  (const timeval *)&time_out);
		if (numevent > 0)
		{
			int32 events;
			for (int32 i = 0; i < m_curfd_count; ++i)
			{
			//	SYSTEM_LOG("numevent  = %d - end, m_curfd_count = %d, num_event = %d , i = %d, socket = %d", numevent, m_curfd_count, num_event, i, m_paras[i].socket);
				events = 0;
				/*if (m_active_paras[i].events == E_NONE)
				{
					continue;
				}*/
				if (m_active_fd_set_adapter[read_op].is_set(m_paras[i].socket)/*m_active_paras[i].events & E_READFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &readfds)*/ /*read_fd_ptr->fd_array[i] == m_paras[i].socket*/)
				{
					//ERROR_EX_LOG("count = %d, read m_curfd_count = %ld,  sockfd =%lu", i, m_curfd_count, m_paras[i].socket);
					events |= E_READFDS;
				}
				if (m_active_fd_set_adapter[write_op].is_set(m_paras[i].socket)/*m_active_paras[i].events & E_WRITFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &writefds)*/ /*write_fd_ptr->fd_array[i] == m_paras[i].socket*/)
				{
					//ERROR_EX_LOG("count = %d, write m_curfd_count = %ld,  sockfd =%lu", i, m_curfd_count, m_paras[i].socket);
					events |= E_WRITFDS;
				}

				if (m_active_fd_set_adapter[except_op].is_set(m_paras[i].socket)/*m_active_paras[i].events & E_EXCEFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &excefds)*/ /*exec_fd_ptr->fd_array[i] == m_paras[i].socket*/)
				{
					//ERROR_EX_LOG("count = %d, exce m_curfd_count = %ld,  sockfd =%lu", i, m_curfd_count, m_paras[i].socket);
					events |= E_EXCEFDS;
				}
				/*m_active_paras[i].socket = i;
				m_active_paras[i].events = events;*/
				if (events != 0)
				{
					m_active_paras[num_event].events = events;
					m_active_paras[num_event].ptr = m_paras[i].ptr;
					++num_event;
				}
			}
		
		}
		//SYSTEM_LOG("numevent  = %d - end, m_curfd_count = %d, num_event = %d", numevent, m_curfd_count, num_event);
		return num_event;
	}

} // namespace chen 




// select ʵ��

//typedef struct {
//	unsigned long *in, *out, *ex;
//	unsigned long *res_in, *res_out, *res_ex;
//} fd_set_bits;
////  long sys_select(int n, fd_set *inp, fd_set *outp, fd_set *exp, struct timeval *tvp)  
//SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
//	fd_set __user *, exp, struct timeval __user *, tvp)
//{
//	struct timespec end_time, *to = NULL;
//	struct timeval tv;
//	int ret;
//	if (tvp) {
//		if (copy_from_user(&tv, tvp, sizeof(tv))) {
//			return -EFAULT;
//		}
//		// ���㳬ʱʱ��  
//		to = &end_time;
//		if (poll_select_set_timeout(to,
//			tv.tv_sec + (tv.tv_usec / USEC_PER_SEC),
//			(tv.tv_usec % USEC_PER_SEC) * NSEC_PER_USEC)) {
//			return -EINVAL;
//		}
//	}
//	ret = core_sys_select(n, inp, outp, exp, to);
//	// ����ʣ��ʱ�䵽�û��ռ�  
//	ret = poll_select_copy_remaining(&end_time, tvp, 1, ret);
//	return ret;
//}
//
//int core_sys_select(int n, fd_set __user *inp, fd_set __user *outp,
//	fd_set __user *exp, struct timespec *end_time)
//{
//	fd_set_bits fds;
//	void *bits;
//	int ret, max_fds;
//	unsigned int size;
//	struct fdtable *fdt;
//	//С����ʹ��ջ�ϵĿռ�,��Լ�ڴ�, �ӿ�����ٶ�  
//	long stack_fds[SELECT_STACK_ALLOC / sizeof(long)];
//
//	ret = -EINVAL;
//	if (n < 0) {
//		goto out_nofds;
//	}
//
//	rcu_read_lock();
//	// ȡ�ý��̶�Ӧ�� fdtable  
//	fdt = files_fdtable(current->files);
//	max_fds = fdt->max_fds;
//	rcu_read_unlock();
//	if (n > max_fds) 
//	{
//		n = max_fds;
//	}
//
//	size = FDS_BYTES(n);
//	bits = stack_fds;
//	if (size > sizeof(stack_fds) / 6) 
//	{
//		// ջ�ϵĿռ䲻��, �����ڴ�, ȫ��ʹ�ö��ϵĿռ�  
//		ret = -ENOMEM;
//		bits = kmalloc(6 * size, GFP_KERNEL);
//		if (!bits) {
//			goto out_nofds;
//		}
//	}
//	fds.in = bits;
//	fds.out = bits + size;
//	fds.ex = bits + 2 * size;
//	fds.res_in = bits + 3 * size;
//	fds.res_out = bits + 4 * size;
//	fds.res_ex = bits + 5 * size;
//
//	// �����û��ռ䵽�ں�  
//	if ((ret = get_fd_set(n, inp, fds.in)) ||
//		(ret = get_fd_set(n, outp, fds.out)) ||
//		(ret = get_fd_set(n, exp, fds.ex))) {
//		goto out;
//	}
//	// ��ʼ��fd set  
//	zero_fd_set(n, fds.res_in);
//	zero_fd_set(n, fds.res_out);
//	zero_fd_set(n, fds.res_ex);
//
//	ret = do_select(n, &fds, end_time);
//
//	if (ret < 0) {
//		goto out;
//	}
//	if (!ret) {
//		// �÷���ֵ�ᱻϵͳ����, ����ͬ���Ĳ������µ���sys_select()  
//		ret = -ERESTARTNOHAND;
//		if (signal_pending(current)) {
//			goto out;
//		}
//		ret = 0;
//	}
//
//	// ���Ƶ��û��ռ�  
//	if (set_fd_set(n, inp, fds.res_in) ||
//		set_fd_set(n, outp, fds.res_out) ||
//		set_fd_set(n, exp, fds.res_ex)) {
//		ret = -EFAULT;
//	}
//
//out:
//	if (bits != stack_fds) {
//		kfree(bits);
//	}
//out_nofds:
//	return ret;
//}
//
//int do_select(int n, fd_set_bits *fds, struct timespec *end_time)
//{
//	ktime_t expire, *to = NULL;
//	struct poll_wqueues table;
//	poll_table *wait;
//	int retval, i, timed_out = 0;
//	unsigned long slack = 0;
//
//	rcu_read_lock();
//	// ���fds��fd����Ч��, ����ȡ��ǰ����fd  
//	retval = max_select_fd(n, fds);
//	rcu_read_unlock();
//
//	if (retval < 0) {
//		return retval;
//	}
//	n = retval;
//
//	// ��ʼ�� poll_wqueues �ṹ, ���ú���ָ��_qproc    Ϊ__pollwait  
//	poll_initwait(&table);
//	wait = &table.pt;
//	if (end_time && !end_time->tv_sec && !end_time->tv_nsec) {
//		wait = NULL;
//		timed_out = 1;
//	}
//
//	if (end_time && !timed_out) {
//		// ������Ҫ�ȴ���ʱ��.  
//		slack = select_estimate_accuracy(end_time);
//	}
//
//	retval = 0;
//	for (;;) {
//		unsigned long *rinp, *routp, *rexp, *inp, *outp, *exp;
//
//		inp = fds->in;
//		outp = fds->out;
//		exp = fds->ex;
//		rinp = fds->res_in;
//		routp = fds->res_out;
//		rexp = fds->res_ex;
//		// �������е�������, i �ļ�������  
//		for (i = 0; i < n; ++rinp, ++routp, ++rexp) {
//			unsigned long in, out, ex, all_bits, bit = 1, mask, j;
//			unsigned long res_in = 0, res_out = 0, res_ex = 0;
//			const struct file_operations *f_op = NULL;
//			struct file *file = NULL;
//			// ��鵱ǰ�� slot �е�������  
//			in = *inp++;
//			out = *outp++;
//			ex = *exp++;
//			all_bits = in | out | ex;
//			if (all_bits == 0) { // û����Ҫ������������, ��һ��slot  
//				i += __NFDBITS;
//				continue;
//			}
//
//			for (j = 0; j < __NFDBITS; ++j, ++i, bit <<= 1) {
//				int fput_needed;
//				if (i >= n) {
//					break;
//				}
//				// ����Ҫ���������� i  
//				if (!(bit & all_bits)) {
//					continue;
//				}
//				// ȡ���ļ��ṹ  
//				file = fget_light(i, &fput_needed);
//				if (file) {
//					f_op = file->f_op;
//					// û�� f_op �Ļ�����Ϊһֱ���ھ���״̬  
//					mask = DEFAULT_POLLMASK;
//					if (f_op && f_op->poll) {
//						// ���õȴ��¼�������  
//						wait_key_set(wait, in, out, bit);
//						/*
//						static inline void wait_key_set(poll_table *wait, unsigned long in,
//						unsigned long out, unsigned long bit)
//						{
//						wait->_key = POLLEX_SET;// (POLLPRI)
//						if (in & bit)
//						wait->_key |= POLLIN_SET;//(POLLRDNORM | POLLRDBAND | POLLIN | POLLHUP | POLLERR)
//						if (out & bit)
//						wait->_key |= POLLOUT_SET;//POLLOUT_SET (POLLWRBAND | POLLWRNORM | POLLOUT | POLLERR)
//						}
//						*/
//						// ��ȡ��ǰ�ľ���״̬, ����ӵ��ļ��Ķ�Ӧ�ȴ�������  
//						mask = (*f_op->poll)(file, wait);
//						// ��poll��ȫһ��  
//					}
//					fput_light(file, fput_needed);
//					// �ͷ��ļ�  
//					// ����ļ� i �Ƿ������¼�������  
//					if ((mask & POLLIN_SET) && (in & bit)) {
//						res_in |= bit;
//						retval++;
//						// ������о����¼��Ͳ����������ļ���  
//						// �ȴ���������ӻص�����  
//						wait = NULL;
//					}
//					if ((mask & POLLOUT_SET) && (out & bit)) {
//						res_out |= bit;
//						retval++;
//						wait = NULL;
//					}
//					if ((mask & POLLEX_SET) && (ex & bit)) {
//						res_ex |= bit;
//						retval++;
//						wait = NULL;
//					}
//				}
//			}
//			if (res_in) {
//				*rinp = res_in;
//			}
//			if (res_out) {
//				*routp = res_out;
//			}
//			if (res_ex) {
//				*rexp = res_ex;
//			}
//			cond_resched();
//		}
//		wait = NULL; // ����ӻص������Ķ��Ѿ������  
//		if (retval || timed_out || signal_pending(current)) {
//			break;   // �źŷ����������¼�������ʱ  
//		}
//		if (table.error) {
//			retval = table.error; // ����������  
//			break;
//		}
//		// ת�����ں�ʱ��  
//		if (end_time && !to) {
//			expire = timespec_to_ktime(*end_time);
//			to = &expire;
//		}
//		// �ȴ�ֱ����ʱ, ���ɻص���������, ��ʱ����ٴα����ļ�������  
//		if (!poll_schedule_timeout(&table, TASK_INTERRUPTIBLE,
//			to, slack)) {
//			timed_out = 1;
//		}
//	}
//
//	poll_freewait(&table);
//
//	return retval;
//}

#endif // #if defined(_MSC_VER)