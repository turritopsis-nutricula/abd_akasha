#ifndef AKASHA_MODEL_ELEMENT_DIRECT_DIRECTDIFINE_HPP_
#define AKASHA_MODEL_ELEMENT_DIRECT_DIRECTDIFINE_HPP_

#include "DefineParameter.hpp"
#include "ConnectType.hpp"

#include <Riceball/mpl/EasyTuple.hpp>
#include <LinearMath/btTransform.h>




// Unit Macros
#define S_1 btScalar(1.)
#define S_H btScalar(0.5)
#define S_0 btScalar(0.)

#define X_VECTOR btVector3( S_1,  S_0, S_0)
#define Y_VECTOR btVector3( S_0,  S_1, S_0)
#define Z_VECTOR btVector3( S_0,  S_0, S_1)

#define X_HALF_VECTOR btVector3( S_H,  S_0, S_0)
#define Y_HALF_VECTOR btVector3( S_0,  S_H, S_0)
#define Z_HALF_VECTOR btVector3( S_0,  S_0, S_H)

#define IDENT_TRANS( VEC ) btTransform( btMatrix3x3::getIdentity(), VEC)

namespace akasha
{
namespace model
{
namespace element_v2
{

namespace direct
{

// Joint Transform
template<ConnectType T>
	inline btTransform GetJointTransform();
// Child Transform
template<ConnectType T>
	inline btTransform GetChildTransform();
// Relative Bias Basis
template<ConnectType T>
	inline btQuaternion GetRelativeRotationBasis();

} // namespace direct

#define WITH_ENUM( TYPE ) ConnectType::  TYPE


#define SET_CONNECT_TRANS( TYPE, JOINT, CHILD, REL_BASIS) \
template<> inline btTransform GetJointTransform< WITH_ENUM(TYPE) >() \
{ return JOINT ;} \
template<> inline btTransform GetChildTransform< WITH_ENUM(TYPE) >() \
{ return CHILD ;} \
template<> inline btQuaternion GetRelativeRotationBasis< WITH_ENUM(TYPE) >() \
{ return REL_BASIS ; }

#define JUNCTION_CONNECT_TRANS( TYPE, REL_TYPE) \
template<> inline btTransform GetJointTransform< WITH_ENUM(TYPE) >() \
{ return GetJointTransform< WITH_ENUM(REL_TYPE) >() ;} \
template<> inline btTransform GetChildTransform< WITH_ENUM(TYPE) >() \
{ return GetChildTransform< WITH_ENUM(REL_TYPE) >() ;} \
template<> inline btQuaternion GetRelativeRotationBasis< WITH_ENUM(TYPE) >() \
{ return GetRelativeRotationBasis< WITH_ENUM(REL_TYPE) >(); }

using DirectType = ConnectType;

namespace direct
{


//ConnectType::Undefine
SET_CONNECT_TRANS(
		Undefine,
		btTransform::getIdentity(),
		btTransform::getIdentity(),
		btQuaternion::getIdentity()
		)

// ConnectType::Count
JUNCTION_CONNECT_TRANS( Count, Undefine)

// ConnectType::North
SET_CONNECT_TRANS(
		North,
		btTransform( btQuaternion( Y_VECTOR, SIMD_PI), Z_HALF_VECTOR),
		IDENT_TRANS( Z_VECTOR ),
		btQuaternion(Y_VECTOR, SIMD_PI)
		)

// ConnectType::South
SET_CONNECT_TRANS(
		South,
		btTransform( btQuaternion::getIdentity(),     -Z_HALF_VECTOR),
		IDENT_TRANS( -Z_VECTOR ),
		btQuaternion(Y_VECTOR, 0)
		)

// ConnectType::West
SET_CONNECT_TRANS(
		West,
		btTransform( btQuaternion(Y_VECTOR, SIMD_HALF_PI), -X_HALF_VECTOR),
		IDENT_TRANS( -X_VECTOR ),
		btQuaternion(Y_VECTOR, SIMD_HALF_PI)
		)

// ConnectType::East
SET_CONNECT_TRANS(
		East,
		btTransform( btQuaternion(Y_VECTOR, -SIMD_HALF_PI), X_HALF_VECTOR),
		IDENT_TRANS( X_VECTOR ),
		btQuaternion(Y_VECTOR, -SIMD_HALF_PI)
		)

// ConnectType::Input
SET_CONNECT_TRANS(
		Input,
		btTransform( btQuaternion( Y_VECTOR, SIMD_PI), Z_HALF_VECTOR),
		IDENT_TRANS( Z_VECTOR ),
		btQuaternion(Y_VECTOR, SIMD_PI)
		)
// ConnectType::Output
SET_CONNECT_TRANS(
		Output,
		btTransform( btQuaternion::getIdentity(),     -Z_HALF_VECTOR),
		IDENT_TRANS( -Z_VECTOR ),
		btQuaternion(Y_VECTOR, 0)
		)


// ConnectType::Relative_North
JUNCTION_CONNECT_TRANS( Relative_North, North)

// ConnectType::Relative_South
JUNCTION_CONNECT_TRANS( Relative_South, South)

// ConnectType::Relative_West
JUNCTION_CONNECT_TRANS( Relative_West, West )

// ConnectType::Relative_East
JUNCTION_CONNECT_TRANS( Relative_East, East)

JUNCTION_CONNECT_TRANS( Port_1, West )
JUNCTION_CONNECT_TRANS( Port_2, East )
JUNCTION_CONNECT_TRANS( Port_3, West )
JUNCTION_CONNECT_TRANS( Port_4, East )





// Function Decl
AKASHA_ENUM_FUNCTOR_DECL( GetChildTransform );
AKASHA_ENUM_FUNCTOR_DECL( GetJointTransform );
AKASHA_ENUM_FUNCTOR_DECL( GetRelativeRotationBasis );


} // namespace direct

#undef JUNCTION_CONNECT_TRANS
#undef SET_CONNECT_TRANS
#undef WITH_ENUM



} // namespace element_v2

} // namespace model
} // akasha



#undef IDENT_TRANS

#undef Z_HALF_VECTOR
#undef Y_HALF_VECTOR
#undef X_HALF_VECTOR

#undef Z_VECTOR
#undef Y_VECTOR
#undef X_VECTOR

#undef S_0
#undef S_1

#endif /* end of include guard */
