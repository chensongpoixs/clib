/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		chandler_type_requirements
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
#ifndef _C_HANDLER_TYPE_REQUIREMENTS_H_
#define _C_HANDLER_TYPE_REQUIREMENTS_H_
#if 0

namespace chen {


#if defined(_HANDLER_TYPE_REQUIREMENTS)

# if defined(_HANDLER_TYPE_REQUIREMENTS_ASSERT)

	template <typename Handler>
	auto zero_arg_handler_test(Handler h, void*)
		-> decltype(
			sizeof(Handler(static_cast<const Handler&>(h))),
			((h)()),
			char(0));

	template <typename Handler>
	char(&zero_arg_handler_test(Handler, ...))[2];

	template <typename Handler, typename Arg1>
	auto one_arg_handler_test(Handler h, Arg1* a1)
		-> decltype(
			sizeof(Handler(static_cast<const Handler&>(h))),
			((h)(*a1)),
			char(0));

	template <typename Handler>
	char(&one_arg_handler_test(Handler h, ...))[2];

	template <typename Handler, typename Arg1, typename Arg2>
	auto two_arg_handler_test(Handler h, Arg1* a1, Arg2* a2)
		-> decltype(
			sizeof(Handler(static_cast<const Handler&>(h))),
			((h)(*a1, *a2)),
			char(0));

	template <typename Handler>
	char(&two_arg_handler_test(Handler, ...))[2];

#  define _HANDLER_TYPE_REQUIREMENTS_ASSERT(expr, msg) \
     static_assert(expr, msg);

# else // defined(BOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)

#  define _HANDLER_TYPE_REQUIREMENTS_ASSERT(expr, msg)

# endif // defined(BOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS_ASSERT)

	template <typename T> T& lvref();
	template <typename T> T& lvref(T);
	template <typename T> const T& clvref();
	template <typename T> const T& clvref(T);
	template <typename T> char argbyv(T);

	template <int>
	struct handler_type_requirements
	{
	};

#define _COMPLETION_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef _HANDLER_TYPE(handler_type, \
      void()) asio_true_handler_type; \
  \
  _HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::zero_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), 0)) == 1, \
      "CompletionHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()(), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_READ_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "ReadHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>(), \
            boost::asio::detail::lvref<const std::size_t>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF


#define BOOST_ASIO_WRITE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "WriteHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>(), \
            boost::asio::detail::lvref<const std::size_t>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_ACCEPT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::one_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0))) == 1, \
      "AcceptHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_CONNECT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::one_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0))) == 1, \
      "ConnectHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_COMPOSED_CONNECT_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, iter_type)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const iter_type*>(0))) == 1, \
      "ComposedConnectHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>(), \
            boost::asio::detail::lvref<const iter_type>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_RESOLVE_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, iter_type)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const iter_type*>(0))) == 1, \
      "ResolveHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>(), \
            boost::asio::detail::lvref<const iter_type>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_WAIT_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::one_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0))) == 1, \
      "WaitHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_SIGNAL_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, int)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const int*>(0))) == 1, \
      "SignalHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>(), \
            boost::asio::detail::lvref<const int>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::one_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0))) == 1, \
      "HandshakeHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_BUFFERED_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code, std::size_t)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::two_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0), \
          static_cast<const std::size_t*>(0))) == 1, \
      "BufferedHandshakeHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
          boost::asio::detail::lvref<const boost::system::error_code>(), \
          boost::asio::detail::lvref<const std::size_t>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_SHUTDOWN_HANDLER_CHECK( \
    handler_type, handler) \
  \
  typedef BOOST_ASIO_HANDLER_TYPE(handler_type, \
      void(boost::system::error_code)) \
    asio_true_handler_type; \
  \
  BOOST_ASIO_HANDLER_TYPE_REQUIREMENTS_ASSERT( \
      sizeof(boost::asio::detail::one_arg_handler_test( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>(), \
          static_cast<const boost::system::error_code*>(0))) == 1, \
      "ShutdownHandler type requirements not met") \
  \
  typedef boost::asio::detail::handler_type_requirements< \
      sizeof( \
        boost::asio::detail::argbyv( \
          boost::asio::detail::clvref< \
            asio_true_handler_type>())) + \
      sizeof( \
        boost::asio::detail::lvref< \
          asio_true_handler_type>()( \
            boost::asio::detail::lvref<const boost::system::error_code>()), \
        char(0))> BOOST_ASIO_UNUSED_TYPEDEF

#else // !defined(BOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)

#define BOOST_ASIO_COMPLETION_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_READ_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_WRITE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_ACCEPT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_CONNECT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_COMPOSED_CONNECT_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_RESOLVE_HANDLER_CHECK( \
    handler_type, handler, iter_type) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_WAIT_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_SIGNAL_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_BUFFERED_HANDSHAKE_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#define BOOST_ASIO_SHUTDOWN_HANDLER_CHECK( \
    handler_type, handler) \
  typedef int BOOST_ASIO_UNUSED_TYPEDEF

#endif // !defined(BOOST_ASIO_ENABLE_HANDLER_TYPE_REQUIREMENTS)


} // namespace chen

#endif
#endif // _C_HANDLER_TYPE_REQUIREMENTS_H_

