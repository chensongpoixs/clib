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
		//锟斤拷要锟睫革拷 m_maxfd 锟斤拷值
		if (descriptor == INVALID_SOCKET)
		{
			ERROR_EX_LOG("descriptor is invalid");
			return -1;
		}

		if (m_curfd_count > m_maxfd_count)
		{
			ERROR_EX_LOG("reach max fd count %lu", m_maxfd_count);
			return -1;
		}
		if (( st& E_READFDS) != 0)
		{//if(descriptor, m_fd_set_adapter[read_op]))
			if (m_fd_set_adapter[read_op].is_set(descriptor))
			{
				ERROR_EX_LOG("socket has been set");
				return -1;
			}
			//ERROR_EX_LOG("add read !!!!!!!!!!!!!!!!!!!!!!!!!!");
			//FD_SET(descriptor, m_fd_set_adapter[read_op]);
			m_fd_set_adapter[read_op].set(descriptor);
		}
		// cur_
		// write 
		if ((st & E_WRITFDS) != 0)
		{
			//ERROR_EX_LOG("add write !!!!!!!!!!!!!!!!!!!!!!!!!!");
			if (m_fd_set_adapter[write_op].is_set(descriptor)/*FD_ISSET(descriptor, m_fd_set_adapter[write_op])*/)
			{
				ERROR_EX_LOG("add write !!!!!!!!!!!!!!!!!!!!!!!!!!");
				m_fd_set_adapter[write_op].set(descriptor);
				//FD_SET(descriptor, m_fd_set_adapter[write_op]);
			}
		}
		else
		{
			if (/*FD_ISSET(descriptor, m_fd_set_adapter[write_op])*/m_fd_set_adapter[write_op].is_set(descriptor))
			{
				ERROR_EX_LOG("delete write !!!!!!!!!!!!!!!!!!!!!!!!!!");
				//m_fd_set_adapter[write_op].remove(descriptor);
				//FD_SET(descriptor, m_fd_set_adapter[write_op]);
			}
		//	ERROR_EX_LOG("delate write ok !!!!!!!!!!!!!!!!!!!!!!!!!!");
			/*if (m_fd_set_adapter[write_op].is_set(descriptor))
			{
				m_fd_set_adapter[write_op].remove(descriptor);
			}*/
		}
		//if (/*!FD_ISSET(descriptor, m_fd_set_adapter[except_op])*/ m_fd_set_adapter[except_op].is_set(descriptor))
		{
			//FD_SET(descriptor, m_fd_set_adapter[except_op]);
			m_fd_set_adapter[except_op].set(descriptor);
		}
		
		m_maxfd = (m_maxfd == INVALID_SOCKET ) ? descriptor : max(descriptor, m_maxfd);
	   // 锟斤拷锟斤拷 curfd_count 
		m_paras[m_curfd_count].socket = descriptor;
		m_paras[m_curfd_count].ptr = session;
		++m_curfd_count;
		return 0;
	}
	void cselect_reactor::register_write_descriptor(socket_type & descriptor, void * para)
	{
		if (descriptor == -1)
		{
			ERROR_EX_LOG("ERROR register_write_descriptor fd (socket_type=%d)", descriptor);
			return;
		}

		if (/*FD_ISSET(descriptor, m_fd_set_adapter[write_op])*/ m_fd_set_adapter[except_op].is_set(descriptor))
		{
			m_fd_set_adapter[write_op].set(descriptor);
			//FD_SET(descriptor, m_fd_set_adapter[write_op]);
		}
		else
		{
			WARNING_EX_LOG("register_write_descriptor fd (socket_type=%d), error = %s", descriptor, strerror(errno));
		}
	}
	void cselect_reactor::deregiter_write_descriptor(socket_type & descriptor, void * para)
	{
		if (descriptor == -1)
		{
			ERROR_EX_LOG("ERROR deregiter_write_descriptor fd (socket_type=%d)", descriptor);
			return;
		}
		if (/*!FD_ISSET(descriptor, m_fd_set_adapter[write_op])*/ m_fd_set_adapter[write_op].is_set(descriptor))
		{
			m_fd_set_adapter[write_op].remove(descriptor);
			//FD_CLR(descriptor, m_fd_set_adapter[write_op]);
		}
		else
		{
			WARNING_EX_LOG("deregiter_write_descriptor fd (socket_type=%d), error = %s", descriptor, strerror(errno));
		}
	//	FD_CLR(descriptor, m_fd_set_adapter[write_op]);//struct kevent ev;
		//EV_SET(&ev, descriptor, EVFILT_WRITE, EV_DELETE, 0, 0, para);
	}
	void cselect_reactor::deregister_descriptor(socket_type& descriptor)
	{
		//锟斤拷要锟睫革拷 m_maxfd 锟斤拷值
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
		// delete fd
		/*if (m_fd_set_adapter[read_op].is_set(descriptor))
		{
			ERROR_EX_LOG("ERROR NetMgr::deregister_descriptor(socket_type=%d)", descriptor);
			return;
		}*/
		m_fd_set_adapter[read_op].remove(descriptor);
		m_fd_set_adapter[write_op].remove(descriptor);
		m_fd_set_adapter[except_op].remove(descriptor);

		/*	FD_CLR(descriptor, m_fd_set_adapter[read_op]);
			FD_CLR(descriptor, m_fd_set_adapter[write_op]);
			FD_CLR(descriptor, m_fd_set_adapter[except_op]);*/

		//寻锟斤拷锟斤拷锟絪ocked
		m_maxfd = INVALID_SOCKET;
		int32 descriptor_index = -1;
		for (int32 i = 0; i < m_curfd_count; ++i)
		{
			//寻锟斤拷fd
			if (m_paras[i].socket == descriptor)
			{
				descriptor_index = i;
			}
			else if (m_maxfd == INVALID_SOCKET || m_paras[i].socket > m_maxfd)
			{
				//寻锟斤拷锟斤拷锟絪ocket
				m_maxfd = m_paras[i].socket;
			}
		}
		m_curfd_count--;
		if (descriptor_index == -1)
		{
			ERROR_EX_LOG("socket not found");
			return;
		}
		//锟斤拷锟斤拷锟揭伙拷锟絪ocket锟狡讹拷锟斤拷删锟斤拷锟斤拷锟角革拷socket位锟斤拷
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
		/*if (m_curfd_count == 0)
		{
			return 0;
		}*/
		struct timeval time_out;
		time_out.tv_sec = 0;
		time_out.tv_usec = ms * 10000;
		
		
		m_active_fd_set_adapter[read_op].reset();
		m_active_fd_set_adapter[write_op].reset();
		m_active_fd_set_adapter[except_op].reset();
		for (uint32 j = 0; j < m_curfd_count; ++j)
		{
			for (int32 i = read_op; i <= except_op; ++i)
			{
				if (m_fd_set_adapter[i].is_set(m_paras[j].socket))
				{
					m_active_fd_set_adapter[i].set(m_paras[j].socket);
				}
			}
			
		}
		int32	num_event = 0;
		//SYSTEM_LOG("m_maxfd = %d", m_maxfd);
		// 锟斤拷锟斤拷锟脚伙拷 write 锟侥硷拷锟斤拷锟斤拷 使锟斤拷 IO使锟斤拷锟斤拷锟斤拷锟竭筹拷  select 使锟斤拷一锟斤拷锟竭筹拷 锟斤拷锟斤拷锟斤拷select锟较猴拷epoll_wait 锟斤拷锟斤拷一锟斤拷锟竭筹拷专锟脚讹拷取锟斤拷写锟斤拷牟锟斤拷锟?
		//ERROR_EX_LOG("select start curfd_count = %u", m_curfd_count);
		int32 numevent = ::select(m_maxfd + 1, m_active_fd_set_adapter[read_op], m_active_fd_set_adapter[write_op], m_active_fd_set_adapter[except_op] /*&readfds, &writefds, &excefds*/,  (const struct timeval *)&time_out);
	//	ERROR_EX_LOG("select end");
		if (numevent > 0)
		{
			/*using namespace std::chrono;

			uint32_t elapse = 0;
			steady_clock::time_point cur_time;
			steady_clock::time_point active_time;
			steady_clock::time_point pre_time = steady_clock::now();
			steady_clock::duration dur;
			milliseconds ms;
			uint32_t	 net_elapse = 0;
			
			cur_time = steady_clock::now();
			dur = cur_time - pre_time;
			ms = duration_cast<milliseconds>(dur);
			net_elapse = static_cast<uint32_t>(ms.count());
			pre_time = cur_time;*/
			int32 events;
			for (int32 i = 0; i < m_curfd_count; ++i)
			{
				//SYSTEM_LOG("numevent  = %d - end, m_curfd_count = %d, num_event = %d , i = %d, socket = %d", numevent, m_curfd_count, num_event, i, m_paras[i].socket);
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
				//	ERROR_EX_LOG("count = %d, write m_curfd_count = %ld,  sockfd =%lu", i, m_curfd_count, m_paras[i].socket);
					events |= E_WRITFDS;  //TODO@chensong 2020-05-14 后期将要写入数据时才设置文件描述符可写入
				}

				if (m_active_fd_set_adapter[except_op].is_set(m_paras[i].socket)/*m_active_paras[i].events & E_EXCEFDS &&*//*0 != FD_ISSET(m_paras[i].socket, &excefds)*/ /*exec_fd_ptr->fd_array[i] == m_paras[i].socket*/)
				{
				//	ERROR_EX_LOG("count = %d, exce m_curfd_count = %ld,  sockfd =%lu", i, m_curfd_count, m_paras[i].socket);
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
			/*cur_time = steady_clock::now();

			dur = cur_time - pre_time;
			ms = duration_cast<milliseconds>(dur);
			elapse = static_cast<uint32_t>(ms.count());
			SYSTEM_LOG("numevent  = %d - end, m_curfd_count = %d, num_event = %d, net_elapse =%u, elapse = %u", numevent, m_curfd_count, num_event, net_elapse, elapse);*/
		}
		
		return num_event;
	}

} // namespace chen 




// select 实锟斤拷

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
//		// 锟斤拷锟姐超时时锟斤拷  
//		to = &end_time;
//		if (poll_select_set_timeout(to,
//			tv.tv_sec + (tv.tv_usec / USEC_PER_SEC),
//			(tv.tv_usec % USEC_PER_SEC) * NSEC_PER_USEC)) {
//			return -EINVAL;
//		}
//	}
//	ret = core_sys_select(n, inp, outp, exp, to);
//	// 锟斤拷锟斤拷剩锟斤拷时锟戒到锟矫伙拷锟秸硷拷  
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
//	//小锟斤拷锟斤拷使锟斤拷栈锟较的空硷拷,锟斤拷约锟节达拷, 锟接匡拷锟斤拷锟斤拷俣锟? 
//	long stack_fds[SELECT_STACK_ALLOC / sizeof(long)];
//
//	ret = -EINVAL;
//	if (n < 0) {
//		goto out_nofds;
//	}
//
//	rcu_read_lock();
//	// 取锟矫斤拷锟教讹拷应锟斤拷 fdtable  
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
//		// 栈锟较的空间不锟斤拷, 锟斤拷锟斤拷锟节达拷, 全锟斤拷使锟矫讹拷锟较的空硷拷  
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
//	// 锟斤拷锟斤拷锟矫伙拷锟秸间到锟节猴拷  
//	if ((ret = get_fd_set(n, inp, fds.in)) ||
//		(ret = get_fd_set(n, outp, fds.out)) ||
//		(ret = get_fd_set(n, exp, fds.ex))) {
//		goto out;
//	}
//	// 锟斤拷始锟斤拷fd set  
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
//		// 锟矫凤拷锟斤拷值锟结被系统锟斤拷锟斤拷, 锟斤拷锟斤拷同锟斤拷锟侥诧拷锟斤拷锟斤拷锟铰碉拷锟斤拷sys_select()  
//		ret = -ERESTARTNOHAND;
//		if (signal_pending(current)) {
//			goto out;
//		}
//		ret = 0;
//	}
//
//	// 锟斤拷锟狡碉拷锟矫伙拷锟秸硷拷  
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
//	// 锟斤拷锟絝ds锟斤拷fd锟斤拷锟斤拷效锟斤拷, 锟斤拷锟斤拷取锟斤拷前锟斤拷锟斤拷fd  
//	retval = max_select_fd(n, fds);
//	rcu_read_unlock();
//
//	if (retval < 0) {
//		return retval;
//	}
//	n = retval;
//
//	// 锟斤拷始锟斤拷 poll_wqueues 锟结构, 锟斤拷锟矫猴拷锟斤拷指锟斤拷_qproc    为__pollwait  
//	poll_initwait(&table);
//	wait = &table.pt;
//	if (end_time && !end_time->tv_sec && !end_time->tv_nsec) {
//		wait = NULL;
//		timed_out = 1;
//	}
//
//	if (end_time && !timed_out) {
//		// 锟斤拷锟斤拷锟斤拷要锟饺达拷锟斤拷时锟斤拷.  
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
//		// 锟斤拷锟斤拷锟斤拷锟叫碉拷锟斤拷锟斤拷锟斤拷, i 锟侥硷拷锟斤拷锟斤拷锟斤拷  
//		for (i = 0; i < n; ++rinp, ++routp, ++rexp) {
//			unsigned long in, out, ex, all_bits, bit = 1, mask, j;
//			unsigned long res_in = 0, res_out = 0, res_ex = 0;
//			const struct file_operations *f_op = NULL;
//			struct file *file = NULL;
//			// 锟斤拷榈鼻帮拷锟?slot 锟叫碉拷锟斤拷锟斤拷锟斤拷  
//			in = *inp++;
//			out = *outp++;
//			ex = *exp++;
//			all_bits = in | out | ex;
//			if (all_bits == 0) { // 没锟斤拷锟斤拷要锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷, 锟斤拷一锟斤拷slot  
//				i += __NFDBITS;
//				continue;
//			}
//
//			for (j = 0; j < __NFDBITS; ++j, ++i, bit <<= 1) {
//				int fput_needed;
//				if (i >= n) {
//					break;
//				}
//				// 锟斤拷锟斤拷要锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 i  
//				if (!(bit & all_bits)) {
//					continue;
//				}
//				// 取锟斤拷锟侥硷拷锟结构  
//				file = fget_light(i, &fput_needed);
//				if (file) {
//					f_op = file->f_op;
//					// 没锟斤拷 f_op 锟侥伙拷锟斤拷锟斤拷为一直锟斤拷锟节撅拷锟斤拷状态  
//					mask = DEFAULT_POLLMASK;
//					if (f_op && f_op->poll) {
//						// 锟斤拷锟矫等达拷锟铰硷拷锟斤拷锟斤拷锟斤拷  
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
//						// 锟斤拷取锟斤拷前锟侥撅拷锟斤拷状态, 锟斤拷锟斤拷拥锟斤拷募锟斤拷亩锟接︼拷却锟斤拷锟斤拷锟斤拷锟? 
//						mask = (*f_op->poll)(file, wait);
//						// 锟斤拷poll锟斤拷全一锟斤拷  
//					}
//					fput_light(file, fput_needed);
//					// 锟酵凤拷锟侥硷拷  
//					// 锟斤拷锟斤拷募锟?i 锟角凤拷锟斤拷锟斤拷锟铰硷拷锟斤拷锟斤拷锟斤拷  
//					if ((mask & POLLIN_SET) && (in & bit)) {
//						res_in |= bit;
//						retval++;
//						// 锟斤拷锟斤拷锟斤拷芯锟斤拷锟斤拷录锟斤拷筒锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷募锟斤拷锟? 
//						// 锟饺达拷锟斤拷锟斤拷锟斤拷锟斤拷踊氐锟斤拷锟斤拷锟? 
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
//		wait = NULL; // 锟斤拷锟斤拷踊氐锟斤拷锟斤拷锟斤拷亩锟斤拷丫锟斤拷锟斤拷锟斤拷  
//		if (retval || timed_out || signal_pending(current)) {
//			break;   // 锟脚号凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟铰硷拷锟斤拷锟斤拷锟斤拷时  
//		}
//		if (table.error) {
//			retval = table.error; // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷  
//			break;
//		}
//		// 转锟斤拷锟斤拷锟节猴拷时锟斤拷  
//		if (end_time && !to) {
//			expire = timespec_to_ktime(*end_time);
//			to = &expire;
//		}
//		// 锟饺达拷直锟斤拷锟斤拷时, 锟斤拷锟缴回碉拷锟斤拷锟斤拷锟斤拷锟斤拷, 锟斤拷时锟斤拷锟斤拷俅伪锟斤拷锟斤拷募锟斤拷锟斤拷锟斤拷锟? 
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