/***********************************************************************************************
created: 		2020-05-16

author:			chensong

purpose:		chandler_type
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


#ifndef _C_HANDLER_TYPE_
#define _C_HANDLER_TYPE_

namespace chen {
	
		/// Default handler type traits provided for all handlers.
		/**
		* The handler_type traits class is used for determining the concrete handler
		* type to be used for an asynchronous operation. It allows the handler type to
		* be determined at the point where the specific completion handler signature
		* is known.
		*
		* This template may be specialised for user-defined handler types.
		*/
		template <typename Handler, typename Signature>
		struct handler_type
		{
			/// The handler type for the specific signature.
			typedef Handler type;
		};

#if !defined(GENERATING_DOCUMENTATION)

		template <typename Handler, typename Signature>
		struct handler_type<const Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<volatile Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const volatile Handler, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<volatile Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<const volatile Handler&, Signature>
			: handler_type<Handler, Signature> {};

		template <typename Handler, typename Signature>
		struct handler_type<Handler&, Signature>
			: handler_type<Handler, Signature> {};

#if defined(CHEN_ASIO_HAS_MOVE)
		template <typename Handler, typename Signature>
		struct handler_type<Handler&&, Signature>
			: handler_type<Handler, Signature> {};
#endif // defined(CHEN_ASIO_HAS_MOVE)

		template <typename ReturnType, typename Signature>
		struct handler_type<ReturnType(), Signature>
			: handler_type<ReturnType(*)(), Signature> {};

		template <typename ReturnType, typename Arg1, typename Signature>
		struct handler_type<ReturnType(Arg1), Signature>
			: handler_type<ReturnType(*)(Arg1), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Signature>
		struct handler_type<ReturnType(Arg1, Arg2), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Arg4, typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3, Arg4), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3, Arg4), Signature> {};

		template <typename ReturnType, typename Arg1, typename Arg2, typename Arg3,
			typename Arg4, typename Arg5, typename Signature>
			struct handler_type<ReturnType(Arg1, Arg2, Arg3, Arg4, Arg5), Signature>
			: handler_type<ReturnType(*)(Arg1, Arg2, Arg3, Arg4, Arg5), Signature> {};

#endif // !defined(GENERATING_DOCUMENTATION)

	
} // namespace chen


#define CHNE_ASIO_HANDLER_TYPE(h, sig) \
  typename handler_type<h, sig>::type

#endif // _C_HANDLER_TYPE_
