#ifndef INSTANSIATER__H
#define INSTANSIATER__H

#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>

#define BOOST_PP_LOCAL_MACRO(n) \
	template< BOOST_PP_ENUM_PARAMS( n, typename A) > \
T* operator()( BOOST_PP_ENUM_BINARY_PARAMS(n, A, a) ) \
{ return new T( BOOST_PP_ENUM_PARAMS(n, a) ); } \
template<class Self, BOOST_PP_ENUM_PARAMS(n, typename A) > \
struct result< Self( BOOST_PP_ENUM_PARAMS(n, A) ) > \
{ typedef T* type; }; \
/**/

namespace akasha
{
template<class T>
	struct Instansiater
{

	template <typename Sig>
		struct result;

	T* operator()()
	{
		return new T;
	}
	template <class Self>
		struct result< Self() >
		{
			typedef T* type;
		};

	/*
		template<typename A0>
		T* operator()(A0 a0)
		{
		return new T(a0);
		}
		template <class Self, typename A0>
		struct result< Self(A0) >
		{
		typedef T* type;
		}
		*/

#define BOOST_PP_LOCAL_LIMITS (1, 10)

#include BOOST_PP_LOCAL_ITERATE()

};

}
#endif // INSTANSIATER__H
