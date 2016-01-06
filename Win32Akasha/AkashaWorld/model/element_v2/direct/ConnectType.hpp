#ifndef AKASHA_MODEL_DIRECT_CONNECT_TYPE_HPP_
#define AKASHA_MODEL_DIRECT_CONNECT_TYPE_HPP_


namespace akasha
{
namespace model
{
namespace element_v2
{

enum class ConnectType
{
	Undefine,
	North,
	South,
	West,
	East,
	Input,
	Output,
	Relative_North,
	Relative_South,
	Relative_West,
	Relative_East,

	Port_1,
	Port_2,
	Port_3,
	Port_4,

	Count
}; // enum class ConnectType


////////////////////////////////////////////////////////////////////
// Functer Fold ( not undef)
namespace direct
{
namespace detail
{

template<ConnectType I, template<ConnectType> class Polisy>
struct EnumFold
{
	static auto recursive(ConnectType e)
		-> decltype( Polisy<I>::get() )
	{
		return e==I ?
			Polisy<I>::get() : EnumFold<(ConnectType)((long)I + 1), Polisy>::recursive(e);
	}
};

template<template<ConnectType> class Polisy>
struct EnumFold<ConnectType::Count, Polisy>
{
	static auto recursive(ConnectType e)
		-> decltype(Polisy<ConnectType::Count>::get())
	{ return Polisy<ConnectType::Count>::get(); }
};
} // namespace detail

#define AKASHA_ENUM_FUNCTOR_DECL( FuncName ) \
namespace detail{ template<ConnectType I> \
struct FuncName ## Functor \
{ static auto get() -> decltype( FuncName<I>() ){ return FuncName<I>(); } }; } \
inline  auto FuncName(ConnectType i) \
-> decltype(detail::EnumFold< ConnectType::Undefine, detail::FuncName ## Functor>::recursive(i)) \
{ return detail::EnumFold< ConnectType::Undefine, detail::FuncName ## Functor>::recursive(i); }
} // namespace direct

} // namespace element_v2
} // namespace model
} // akasha
#endif /* end of include guard */
