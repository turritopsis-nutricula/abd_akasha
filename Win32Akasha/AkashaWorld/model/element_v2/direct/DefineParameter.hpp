#ifndef AKASHA_MODEL_ELEMENT_DIRECT_TRANSFORMDEFINE_HPP_
#define AKASHA_MODEL_ELEMENT_DIRECT_TRANSFORMDEFINE_HPP_

#include "ConnectType.hpp"


#define SET_CONNECT_PARAM( TYPE, STR, ID, IS_REL, IS_ENERGY)           \
template<> constexpr char const* GetTypeString< ConnectType:: TYPE >() \
{ return STR ; }                                                       \
template<> constexpr int GetIDNumber< ConnectType:: TYPE >()           \
{ return ID ; }                                                        \
template<> constexpr ConnectType GetConnectType< ID >()                \
{ return  ConnectType::  TYPE  ;}                                      \
template<> constexpr bool IsRelativeConnect< ConnectType:: TYPE >()    \
{ return IS_REL ;}                                                     \
template<> constexpr bool IsEnergyConnection< ConnectType:: TYPE >()   \
{ return IS_ENERGY; }

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace direct
{

// Type String
template<ConnectType T>
	constexpr char const* GetTypeString();
// Type ID
template<ConnectType T>
	constexpr int GetIDNumber();
// ID -> Type
template<int ID>
	constexpr ConnectType GetConnectType();
// Is RelativeConnection
template<ConnectType T>
	constexpr bool IsRelativeConnect();
// Is EnergyConnection
template<ConnectType T>
	constexpr bool IsEnergyConnection();

//               ( ConnectType,   String, ID, IsRelative, IsEnergy)
SET_CONNECT_PARAM( Undefine,       "",   -1, false, false );
SET_CONNECT_PARAM( Count,          "",   -2, false, false );

SET_CONNECT_PARAM( North,          "N",  2,  false, false );
SET_CONNECT_PARAM( South,          "S",  0,  false, false );
SET_CONNECT_PARAM( West,           "W",  1,  false, false );
SET_CONNECT_PARAM( East,           "E",  3,  false, false );

SET_CONNECT_PARAM( Input,          "I",  4,  false, true  );
SET_CONNECT_PARAM( Output,         "O",  5,  false, true  );


SET_CONNECT_PARAM( Relative_North, "_N", 12, true,  false );
SET_CONNECT_PARAM( Relative_South, "_S", 10, true,  false );
SET_CONNECT_PARAM( Relative_West,  "_W", 11, true,  false );
SET_CONNECT_PARAM( Relative_East,  "_E", 13, true,  false );

SET_CONNECT_PARAM( Port_1,         "_1", 20, true,  true  );
SET_CONNECT_PARAM( Port_2,         "_2", 21, true,  true  );
SET_CONNECT_PARAM( Port_3,         "_3", 22, true,  true  );
SET_CONNECT_PARAM( Port_4,         "_4", 23, true,  true  );

// From: .DefineParameter.hpp
AKASHA_ENUM_FUNCTOR_DECL( GetTypeString );
AKASHA_ENUM_FUNCTOR_DECL( GetIDNumber );
AKASHA_ENUM_FUNCTOR_DECL( IsEnergyConnection );
AKASHA_ENUM_FUNCTOR_DECL( IsRelativeConnect );

} // namespace direct
} // namespace element_v2
} // namespace model
} // namespace akasha

#undef SET_CONNECT_PARAM

#endif /* end of include guard */
