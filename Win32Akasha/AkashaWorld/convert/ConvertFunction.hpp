#ifndef CONVERT_FUNCTION_HPP
#define CONVERT_FUNCTION_HPP

//#include "convert/ConvertMPL.hpp"
#include <memory>

namespace akasha
{
template<typename To, typename From>
	To convertType(const From& from)
{
	return To();
}

namespace convert
{

/*
//
template<typename T, typename Arg>
typename mplType::ConvertType<T, Arg>::type&& convert(const Arg& arg)
{
//return detail::convertImpl<typename mplType::ConvertType<T, Arg>::type, Arg>(arg);
return convertImpl<typename mplType::ConvertType<T, Arg>::type, Arg>(arg);
}

template<typename T, typename Arg>
typename mplType::ConvertType<T, Arg>::type&& transform(const Arg&& arg)
{
return transformImpl<typename mplType::ConvertType<T, Arg>::type, Arg>( std::move(arg) );
}
*/
	template<typename To, typename From>
To&& convertImpl(const From& from)
{
	return std::move(To());
}
	template<typename To, typename From>
To&& transformImpl(const From&& from)
{
	return std::move(convertImpl(from));
}
/*
	namespace detail
	{

	template<typename To, typename From>
	To convertImpl(const From& from){
	return To;
	}

	template<>
	irr::core::vector3df convertImpl(const btVector3& from)
	{
	return irr::core::vector3df(from.getX(),from.getY(),from.getZ());
	}
	}
	*/
}
}
#endif //CONVERT_FUNCTION_HPP
