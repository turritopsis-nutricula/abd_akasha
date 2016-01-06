#ifndef CONVERTMPL__HPP
#define CONVERTMPL__HPP

#ifdef btTransform_H
#    define USE_BULLET
#endif

#ifdef __IRRLICHT_H_INCLUDED__
#    define USE_IRR
#endif

#ifdef  _d3d9TYPES_H_
#    define USE_X3D
#endif

//boost
#include <boost/mpl/assert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/count.hpp>

namespace akasha
{
namespace convert
{

//�^�O
namespace tag
{
struct vector3;
struct matrix4;
}


namespace mpl = boost::mpl;

namespace library
{
struct undefined
{
	typedef mpl::map<> typeMap;
};
#ifdef __IRRLICHT_H_INCLUDED__
struct irrlicht
{

	typedef mpl::map<

		mpl::pair< tag::vector3, irr::core::vector3df>

		> typeMap;
};
#endif //__IRRLICHT_H_INCLUDED__

////////////////////////////////////////////////////////////////
#ifdef USE_BULLET
struct bullet
{

	typedef mpl::map<

		mpl::pair< tag::vector3, btVector3>,
		mpl::pair< tag::matrix4, btTransform>

			> typeMap;
};
#endif //USE_BULLET

////////////////////////////////////////////////////////////////
#ifdef USE_X3D
struct x3d
{
	typedef mpl::map<

		mpl::pair< tag::vector3, X3DAUDIO_VECTOR>
		> typeMap;
};
#endif //USE_X3D
}

//! ���C�u�����}�b�v���X�g
typedef mpl::vector<
library::undefined
#ifdef __IRRLICHT_H_INCLUDED__
,library::irrlicht
#endif //__IRRLICHT_H_INCLUDED__

#ifdef USE_BULLET
,library::bullet
#endif
#ifdef USE_X3D
,library::x3d
#endif
> liblarys;

namespace mplType
{

//! Liblrary���̌^����Key���擾
template<typename L,typename A>
	struct getKey_Library :
		mpl::first<
		typename mpl::deref<
		typename mpl::find_if<
		typename  L::typeMap::type, boost::is_same<mpl::second<mpl::_1>, A>
		>::type                         // find_if
		>::type                               // deref
		>                               // first
	{

	};

//! Liblrary���ɊY���^�����邩�ۂ�
template<typename L,typename T>
	struct findKey_Library :
		mpl::if_<
		mpl::less<
		mpl::long_<0>,
		typename mpl::count_if<
		typename L::typeMap::type,
		boost::is_same< mpl::second<mpl::_1>, T >>::type
		>,                          // mpl::count_if
		mpl::true_, mpl::false_>
	{
	};


//! �Y���^�������C�u������Ԃ�
template<typename T>
	struct getLibrary :
		mpl::deref<
		typename mpl::find_if<
		::akasha::convert::liblarys,
		findKey_Library<mpl::_1, T>>::type
		>
	{
	};

//! �����̌^����^�[�Q�b�g���C�u�����̌^�ɕϊ�
template<typename TargetLib, typename Arg>
	struct ConvertType :
		mpl::at<
		typename TargetLib::typeMap::type,
		typename getKey_Library<
		typename getLibrary<Arg>::type, Arg
		>::type >
	{
	};

}


}
}

#undef USE_X3D
#undef USE_IRR
#undef USE_BULLET

#endif  // CONVERTMPL__HPP
