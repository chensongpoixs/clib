/***********************************************************************************************
			created: 		2019-04-06

			author:			chensong

			purpose:		select_reactor
我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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
		//需要修改 m_maxfd 的值
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
		//需要修改 m_maxfd 的值
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

		//寻找最大socked
		m_maxfd = INVALID_SOCKET;
		int32 descriptor_index = -1;
		for (int32 i = 0; i < m_curfd_count; ++i)
		{
			//寻找fd
			if (m_paras[i].socket == descriptor)
			{
				descriptor_index = i;
			}
			else if (m_maxfd == INVALID_SOCKET || m_paras[i].socket > m_maxfd)
			{
				//寻找最大socket
				m_maxfd = m_paras[i].socket;
			}
		}
		m_curfd_count--;
		if (descriptor_index == -1)
		{
			ERROR_EX_LOG("socket not found");
			return;
		}
		//把最后一个socket移动到删除的那个socket位置
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
		// 后期优化 write 文件集合 使用 IO使用两个线程  select 使用一个线程 阻塞在select上和epoll_wait 还有一个线程专门读取和写入的操作
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




// select 实现

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
//		// 计算超时时间  
//		to = &end_time;
//		if (poll_select_set_timeout(to,
//			tv.tv_sec + (tv.tv_usec / USEC_PER_SEC),
//			(tv.tv_usec % USEC_PER_SEC) * NSEC_PER_USEC)) {
//			return -EINVAL;
//		}
//	}
//	ret = core_sys_select(n, inp, outp, exp, to);
//	// 复制剩余时间到用户空间  
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
//	//小对象使用栈上的空间,节约内存, 加快访问速度  
//	long stack_fds[SELECT_STACK_ALLOC / sizeof(long)];
//
//	ret = -EINVAL;
//	if (n < 0) {
//		goto out_nofds;
//	}
//
//	rcu_read_lock();
//	// 取得进程对应的 fdtable  
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
//		// 栈上的空间不够, 申请内存, 全部使用堆上的空间  
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
//	// 复制用户空间到内核  
//	if ((ret = get_fd_set(n, inp, fds.in)) ||
//		(ret = get_fd_set(n, outp, fds.out)) ||
//		(ret = get_fd_set(n, exp, fds.ex))) {
//		goto out;
//	}
//	// 初始化fd set  
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
//		// 该返回值会被系统捕获, 并以同样的参数重新调用sys_select()  
//		ret = -ERESTARTNOHAND;
//		if (signal_pending(current)) {
//			goto out;
//		}
//		ret = 0;
//	}
//
//	// 复制到用户空间  
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
//	// 检查fds中fd的有效性, 并获取当前最大的fd  
//	retval = max_select_fd(n, fds);
//	rcu_read_unlock();
//
//	if (retval < 0) {
//		return retval;
//	}
//	n = retval;
//
//	// 初始化 poll_wqueues 结构, 设置函数指针_qproc    为__pollwait  
//	poll_initwait(&table);
//	wait = &table.pt;
//	if (end_time && !end_time->tv_sec && !end_time->tv_nsec) {
//		wait = NULL;
//		timed_out = 1;
//	}
//
//	if (end_time && !timed_out) {
//		// 估计需要等待的时间.  
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
//		// 遍历所有的描述符, i 文件描述符  
//		for (i = 0; i < n; ++rinp, ++routp, ++rexp) {
//			unsigned long in, out, ex, all_bits, bit = 1, mask, j;
//			unsigned long res_in = 0, res_out = 0, res_ex = 0;
//			const struct file_operations *f_op = NULL;
//			struct file *file = NULL;
//			// 检查当前的 slot 中的描述符  
//			in = *inp++;
//			out = *outp++;
//			ex = *exp++;
//			all_bits = in | out | ex;
//			if (all_bits == 0) { // 没有需要监听的描述符, 下一个slot  
//				i += __NFDBITS;
//				continue;
//			}
//
//			for (j = 0; j < __NFDBITS; ++j, ++i, bit <<= 1) {
//				int fput_needed;
//				if (i >= n) {
//					break;
//				}
//				// 不需要监听描述符 i  
//				if (!(bit & all_bits)) {
//					continue;
//				}
//				// 取得文件结构  
//				file = fget_light(i, &fput_needed);
//				if (file) {
//					f_op = file->f_op;
//					// 没有 f_op 的话就认为一直处于就绪状态  
//					mask = DEFAULT_POLLMASK;
//					if (f_op && f_op->poll) {
//						// 设置等待事件的掩码  
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
//						// 获取当前的就绪状态, 并添加到文件的对应等待队列中  
//						mask = (*f_op->poll)(file, wait);
//						// 和poll完全一样  
//					}
//					fput_light(file, fput_needed);
//					// 释放文件  
//					// 检查文件 i 是否已有事件就绪，  
//					if ((mask & POLLIN_SET) && (in & bit)) {
//						res_in |= bit;
//						retval++;
//						// 如果已有就绪事件就不再向其他文件的  
//						// 等待队列中添加回调函数  
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
//		wait = NULL; // 该添加回调函数的都已经添加了  
//		if (retval || timed_out || signal_pending(current)) {
//			break;   // 信号发生，监听事件就绪或超时  
//		}
//		if (table.error) {
//			retval = table.error; // 产生错误了  
//			break;
//		}
//		// 转换到内核时间  
//		if (end_time && !to) {
//			expire = timespec_to_ktime(*end_time);
//			to = &expire;
//		}
//		// 等待直到超时, 或由回调函数唤醒, 超时后会再次遍历文件描述符  
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