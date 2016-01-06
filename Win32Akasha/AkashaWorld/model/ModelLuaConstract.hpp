#ifndef MODEL_LUA_CONSTRACT__HPP
#define MODEL_LUA_CONSTRACT__HPP

#include "../../AkashaSystem/AkashaSystem.hpp"

#include "../../akashaApplication/lua/luabindExConverter.hpp"
#include "../../AkashaApplication/lua/LuaEnvironment.hpp"
#include "element/ElementData.hpp"

#include <Riceball/singleton/SingletonHolder.hpp>

#include "ModelObserver.hpp"
#include "../convert/ConvertImpl_narowchar_widechar.hpp"

#include <luabind/luabind.hpp>
#include <luabind/raw_policy.hpp>
#include <luabind/tag_function.hpp>
#include <lua.hpp>

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

namespace akasha
{
namespace lua
{
namespace binded
{
typedef float lFloat;
}
}
}

namespace
{
template <class F>
	luabind::detail::tagged_function <
	typename luabind::detail::signature_from_function<float ( int )>::type
	, F
	>
	tag_bind( F f )
{
	return luabind::tag_function<float ( int )>( f );
}

}

namespace akasha
{
namespace lua
{
namespace binded
{

using  es_type = model::element_v2::ElementParameter;
using observer_ptr_type = boost::shared_ptr<model::ModelObserver>;

///////////////////////
//btVector3 helper
struct ExtractX
{
	inline static lFloat extruct(btVector3 const& v)
	{ return v.getX(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return q.getX(); }
};
struct ExtractX_N
{
	inline static lFloat extruct(btVector3 const& v)
	{ return -v.getX(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return -q.getX(); }
};
struct ExtractY
{
	inline static lFloat extruct(btVector3 const& v)
	{ return v.getY(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return q.getY(); }
};
struct ExtractY_N
{
	inline static lFloat extruct(btVector3 const& v)
	{ return -v.getY(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return -q.getY(); }
};
struct ExtractZ
{
	inline static lFloat extruct(btVector3 const& v)
	{ return v.getZ(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return q.getZ(); }
};
struct ExtractZ_N
{
	inline static lFloat extruct(btVector3 const& v)
	{ return -v.getZ(); }
	inline static lFloat extruct(btQuaternion const& q)
	{ return -q.getZ(); }
};
struct ExtractW
{
	inline static lFloat extruct(btQuaternion const& q)
	{ return q.getW(); }
};
struct ExtractLocalX
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return t.getBasis().tdotx( lv ); }
};
struct ExtractLocalX_N
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return -t.getBasis().tdotx( lv ); }
};

struct ExtractLocalY
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return t.getBasis().tdoty( lv ); }
};
struct ExtractLocalY_N
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return -t.getBasis().tdoty( lv ); }
};
struct ExtractLocalZ
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return t.getBasis().tdotz( lv ); }
};
struct ExtractLocalZ_N
{
	inline static lFloat extruct(btTransform const& t, btVector3 const& lv)
	{ return -t.getBasis().tdotz( lv ); }
};
/////////////////////
//outメッセージ
struct MessageDrawerBase
{

	boost::function<void ( unsigned int, const std::string )> m_functor;

	template<typename Sig>
		void
		setDrawFunctor( Sig && f )
		{
			m_functor = f;
		}

};
typedef akasha::singleton::SingletonHolder<MessageDrawerBase> MessageDrawer;

//////////////////
//out(lineNumber, str1,str2, ...)
inline int L_out( lua_State* l )
{
	const size_t count = lua_gettop( l );

	if ( count < 2 )  // 引数足りない
		return 0;

	int line = luaL_checkint( l, 1 ); // 表示行

	if ( line < 0 || line > 21 )
	{
		//luaL_error(l, "Error: out() #arg1 Number ignore.");
		return 0;
	}
	std::stringstream result;
	for ( size_t i = 2; i <= count; i++ )
	{
		if ( lua_type( l, i ) != LUA_TNIL )
			result << lua_tostring( l, i );
		else
			result << "nil";
	}

	MessageDrawer::Instance().m_functor( line,  result.str()  );

	return 0;
}



////////////////
// _KEY( keynum ) _KEYDOWN( keynum ), _KEYUP( keynum )
struct LS_KEYStat
{
	boost::function<bool ( unsigned int )> m_keyFunctor;
	lFloat operator()( int i ) const
	{
		return m_keyFunctor( ( unsigned int )i ) ? 1.f : 0.f;
	}
};


////////////////
// _FPS
inline int L_FPS()
{
	//TODO : 再実装待ち
	return 0;
}
////////////////
// _TICKS
struct LS_TICKS
{
	observer_ptr_type obs_;
	LS_TICKS() = delete;
	LS_TICKS(observer_ptr_type const& o) : obs_(o){ }

	int operator()() const
	{
		return obs_->getLifeTicks();
	}
};
////////////////
//  _WIDTH
inline const int L_WIDTH()
{
	//        	  TODO 再実装待ち
	return 0;
}
////////////////
// _HEIGHT
inline const int L_HEIGHT()
{
	//        	  TODO 再実装待ち
	return 0;
}

//////////////////////
//基本テンプレート
template<typename ModelType, template<class> class ExeType >
struct LS_ModelReadBase
{
	ModelType& m_;
	LS_ModelReadBase() = delete;
	LS_ModelReadBase( ModelType& m ) : m_( m ) { }

	auto
		operator()( int n ) const
		-> decltype( ExeType<ModelType>::excute( m_, ( unsigned int )n ) )
		{
			if ( n < 0 || n >= ( int )m_.getElementCount() ) n = 0;
			return ExeType<ModelType>::excute( m_, ( unsigned int )n );
		}

};
template<typename EntityType>
struct LS_ElementAccessBase
{
	observer_ptr_type statues_;
	LS_ElementAccessBase() = delete;
	LS_ElementAccessBase(observer_ptr_type const& s) :
		statues_(s){}

	auto operator()( std::size_t n ) const
		-> decltype( EntityType::excute(statues_->getElementStatus(n)))
		{
			n = n < statues_->getElementCount() ? n : statues_->getElementCount()-1;
			return EntityType::excute(statues_->getElementStatus(n));
		}
};
/////////////////////////////////////////
// _M
struct LSB_M
{
	inline static
		lFloat excute( es_type const& s)
		{
			auto inv =s.getInvMass();
			if (inv > SIMD_EPSILON)
				return btScalar(1.)/inv;
			else
				return btScalar(0.);
		}
};

/////////////////////////////////////////
// _E
struct LSB_E
{
	inline static
		lFloat
		excute( es_type const& s)
		{
			return s.energyLevel() * 1000.f;
		}
};
////////////////////////////////////////
// _EFFECT
struct LSB_EFFECT
{
	inline static lFloat
		excute(es_type const& s)
		{
			return s.getAttribute().getValue(
					boost::type< ::akasha::model::element::parameter::effect>());
		}
};


/////////////////////////////////////////
//_X, _Y, _Z
struct L_XYZ
{
	observer_ptr_type obs_;
	L_XYZ() = delete;
	L_XYZ(observer_ptr_type const& o ) : obs_(o){}
	void operator()( lua_State* L, int cn ) const
	{
		int size = lua_gettop( L );
		if ( size == 1 )
		{
			int n = luaL_checkint( L, 1 );
			if ( n < 0 || n >= ( int )obs_->getElementCount() ) n = 0;
			const auto& v =
				obs_->getElementStatus(n).getWorldTransform().getOrigin();

			//lua_settop(L,0);
			lua_pushnumber( L, v.getX() );
			lua_pushnumber( L, v.getY() );
			lua_pushnumber( L, v.getZ() );
		}
	}
};

template<typename ExtracterType>
struct LSB_XYZ
{
	inline static lFloat excute( es_type const& s)
	{
		return ExtracterType::extruct(s.getWorldTransform().getOrigin());
	}
};

////////////////
//_VX,_VY,_VZ
template<typename ExtracterType>
struct LSB_Vxyz
{
	inline static lFloat excute( es_type const& s )
	{
		return ExtracterType::extruct(s.getWorldTransform(), s.getVelocity());
	}
};
////////////////
// _XX - _ZZ
template<typename ExtracterType, int N>
struct LSB_XX_to_ZZ
{
	inline static lFloat excute( es_type const& s)
	{
		return ExtracterType::extruct( s.getWorldTransform().getBasis()[ N ]);
	}
};

////////////////
// _Q?
template<typename ExtracterType>
struct LSB_Qxyzw
{
	inline static lFloat excute( es_type const& s)
	{
		btQuaternion q;
		s.getWorldTransform().getBasis().getRotation(q);
		return ExtracterType::extruct(q);
	}
};

///////////////////////
// _G?
template<typename ExtracterType>
struct LS_Gxyz
{
	observer_ptr_type obs_;
	LS_Gxyz() = delete;
	LS_Gxyz( observer_ptr_type const& s) : obs_(s){}

	lFloat operator()( std::size_t id ) const
	{
		id = id < obs_->getElementCount() ? id : obs_->getElementCount()-1;
		return ExtracterType::extruct(obs_->getGravityCenter(id));
	}
	lFloat operator()() const
	{
		return ExtracterType::extruct(obs_->getGravityCenter( 0U ));
	}
};

//////////////////////////////
// _W?
template<typename ExtracterType>
struct LSB_Wxyz
{
	inline static lFloat excute( es_type const& s )
	{
		return ExtracterType::extruct(
				s.getWorldTransform(),
				s.getAngularVelocity());
	}
};
//////////////////////////////
// _PARENT, _TOP
struct LSB_PARENT
{
	inline static
		int
		excute( es_type const& s )
		{
			return s.parentId();
		}
};
struct LS_TOP
{
	observer_ptr_type mo_;
	LS_TOP() = delete;
	LS_TOP( observer_ptr_type const& s) :
		mo_(s){}
	int operator()( int id) const
	{
		return (int)(mo_->getElementTop((std::size_t)id));
	}
};

//////////////////////////////
// _H
template<class ModelType>
struct L_H
{
	ModelType& m_;
	boost::function<boost::optional<btScalar> ( const btVector3&, btScalar )> m_altFunctor;

	lFloat
		operator()() const
		{
			return 0.f; // (*this)(0);
		}
	lFloat
		operator()( boost::optional<int> n ) const
		{
			if ( n )
				return ( *this )( *n );
			else
				return 0.f;
		}
	lFloat
		operator()( int sn ) const
		{

			//unsigned int n = sn.is_initialized() ? (unsigned int)(*sn) : 0;
			unsigned int n = ( unsigned int )sn;

			if ( n < 0 || n >= m_.getElementCount() ) n = 0;

			btVector3 const& p = m_.getElementTransform( n ).getOrigin();
			if ( auto a = m_altFunctor( p, 10.f ) )
				return  p.getY() - ( *a );
			else
				return -10000;
		}
};

//////////////////////////////////////
// _BYE
template<class ModelType>
struct LSB_BYE
{
	inline static  bool
		excute( ModelType& m, unsigned int i )
		{
			//TODO : Lua専用のConneptを実装する
			return true;
		}
};

///////////////////////////////////////////
// _SETCOLOR ラインカラーの設定
template<class LineDrawer>
struct L_SETCOLOR
{
	LineDrawer& drawer_;
	void
		operator()( int r, int g, int b) const
		{
			drawer_.setLineColor( r, g, b );
		}

};
// _MOVE3D
template<class LineDrawer>
struct L_MOVE3D
{
	LineDrawer& drawer_;
	void operator()(float x, float y, float z) const
	{
		drawer_.move3DLine( x, y, z);
	}
};
// _LINE3D
template<class LineDrawer>
struct L_LINE3D
{
	LineDrawer& drawer_;
	void operator()(float x,float y, float z) const
	{
		drawer_.draw3DLine(x,y,z);
	}

};


// 			template<class F> using funcBind = luabind::tag_function<F>;
////////////////////////////////////////////////////////////////
// モデルLua環境構築
template <
typename LineDrawerRefType,
			typename OutMessageFunctor,
			typename LuaEnvironmentRefType
	>
inline void ConstractModelLuaEnvironment(
		lua_State* l,
		observer_ptr_type const& statusbuffer,
		LineDrawerRefType& lineDrawer,
		OutMessageFunctor && mesFunctor,
		LuaEnvironmentRefType& modelLuaEnv
		)
{
	using LineDrawerType =
		typename boost::unwrap_reference<LineDrawerRefType>::type;
	using LuaEnvironmentType =
		typename boost::unwrap_reference<LuaEnvironmentRefType>::type;


	//モデル変数を登録
	for( const auto & v : statusbuffer->getModelValueMap() )
		luabind::globals( l )[ v.first ] = v.second;

	//モデルNameを登録
	statusbuffer->status_for_each(
			[l](es_type const& e){
			auto const& n = e.getNameString();
			if (n.length() != 0)
			luabind::globals(l)[n.c_str()] = e.id();
			});


	//out実装
	MessageDrawer::Instance().setDrawFunctor( std::move( mesFunctor ) );
	lua_register( l, "out", &lua::binded::L_out );

	using luabindEx::defFunctor;
	using luabind::tag_function;
	using luabind::def;

	// Simulation
	luabind::module(l)
		[
		def( "_PAUSE", tag_function<void ()>(
					boost::bind(&LuaEnvironmentType::simulationContextHandle, modelLuaEnv, false)
					)),
		def( "_PLAY", tag_function<void ()>(
					boost::bind(&LuaEnvironmentType::simulationContextHandle, modelLuaEnv, true)
					))
		];
	luabind::module( l )
		[

		def( "_KEY",  tag_function<lFloat (int)>(
					LS_KEYStat{boost::bind(&LuaEnvironmentType::getInput, modelLuaEnv, _1)}) ),

		def( "_KEYDOWN", tag_function<lFloat ( int )>(
					LS_KEYStat {boost::bind( &LuaEnvironmentType::getInputDown, modelLuaEnv, _1 ) } ) ),
		def( "_KEYUP", tag_function<lFloat ( int )>(
					LS_KEYStat {boost::bind( &LuaEnvironmentType::getInputUp, modelLuaEnv, _1 ) } ) ),

		def( "_FPS",    &L_FPS ),
		def( "_TICKS",  tag_function<int ()>( LS_TICKS{ statusbuffer })),
		def( "_WIDTH",  &L_WIDTH ),
		def( "_HEIGHT", &L_HEIGHT ),


		def( "_SETCOLOR",
				tag_function<void ( int, int, int)>( L_SETCOLOR<LineDrawerType>{ lineDrawer } ) ),
		def( "_MOVE3D",
				tag_function<void ( lFloat, lFloat, lFloat)>( L_MOVE3D<LineDrawerType>{ lineDrawer }) ),
		def( "_LINE3D",
				tag_function<void ( lFloat, lFloat, lFloat)>( L_LINE3D<LineDrawerType>{ lineDrawer }) )

			];

	using L_GX = LS_Gxyz<ExtractX>;
	using L_GY = LS_Gxyz<ExtractY>;
	using L_GZ = LS_Gxyz<ExtractZ>;
	luabind::module(l)
		[
		def( "_GX", tag_function<lFloat ()>(    L_GX( statusbuffer )) ),
		def( "_GX", tag_function<lFloat (int)>( L_GX( statusbuffer )) ),
		def( "_GY", tag_function<lFloat ()>(    L_GY( statusbuffer )) ),
		def( "_GY", tag_function<lFloat (int)>( L_GY( statusbuffer )) ),
		def( "_GZ", tag_function<lFloat ()>(    L_GZ( statusbuffer )) ),
		def( "_GZ", tag_function<lFloat (int)>( L_GZ( statusbuffer )) )
		];

	using L_M = LS_ElementAccessBase<LSB_M>;
	using L_E = LS_ElementAccessBase< LSB_E >;
	using L_EFFECT = LS_ElementAccessBase< LSB_EFFECT >;
	luabind::module( l )
		[
		def( "_M", tag_bind( L_M( statusbuffer ) ) ),
		def( "_E", tag_bind( L_E( statusbuffer ) ) ),

		def( "_EFFECT", tag_bind( L_EFFECT( statusbuffer ) ) )
		];

	using L_X = LS_ElementAccessBase< LSB_XYZ<ExtractX> >;
	using L_Y = LS_ElementAccessBase< LSB_XYZ<ExtractY> >;
	using L_Z = LS_ElementAccessBase< LSB_XYZ<ExtractZ> >;
	luabind::module( l )
		[
		def( "_XYZ",
				tag_function<void ( lua_State*, int )>( L_XYZ( statusbuffer ) ),
				luabind::raw( _1 ) ),
		def( "_X", tag_bind( L_X( statusbuffer ) ) ),
		def( "_Y", tag_bind( L_Y( statusbuffer ) ) ),
		def( "_Z", tag_bind( L_Z( statusbuffer ) ) )
		];

	using L_VX = LS_ElementAccessBase< LSB_Vxyz<ExtractLocalX> >;
	using L_VY = LS_ElementAccessBase< LSB_Vxyz<ExtractLocalY> >;
	using L_VZ = LS_ElementAccessBase< LSB_Vxyz<ExtractLocalZ_N> >;
	luabind::module( l )
		[
		def( "_VX", tag_bind( L_VX( statusbuffer ) ) ),
		def( "_VY", tag_bind( L_VY( statusbuffer ) ) ),
		def( "_VZ", tag_bind( L_VZ( statusbuffer ) ) )
		];

	//TODO : 未確認
	using L_XX = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractX, 0> >;
	using L_YX = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractY, 0> >;
	using L_ZX = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractZ, 0> >;

	using L_YY = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractX, 1> >;
	using L_XY = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractY, 1> >;
	using L_ZY = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractZ, 1> >;

	using L_YZ = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractX, 2> >;
	using L_ZZ = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractY, 2> >;
	using L_XZ = LS_ElementAccessBase< LSB_XX_to_ZZ<ExtractZ, 2> >;

	luabind::module( l )
		[
		def( "_XX", tag_bind( L_XX( statusbuffer ) ) ),
		def( "_XY", tag_bind( L_XY( statusbuffer ) ) ),
		def( "_XZ", tag_bind( L_XZ( statusbuffer ) ) ),

		def( "_YX", tag_bind( L_YX( statusbuffer ) ) ),
		def( "_YY", tag_bind( L_YY( statusbuffer ) ) ),
		def( "_YZ", tag_bind( L_YZ( statusbuffer ) ) ),

		def( "_ZX", tag_bind( L_ZX( statusbuffer ) ) ),
		def( "_ZY", tag_bind( L_ZY( statusbuffer ) ) ),
		def( "_ZZ", tag_bind( L_ZZ( statusbuffer ) ) )
		];

	//TODO : 未確認
	using L_QX = LS_ElementAccessBase< LSB_Qxyzw<ExtractX> >;
	using L_QY = LS_ElementAccessBase< LSB_Qxyzw<ExtractY> >;
	using L_QZ = LS_ElementAccessBase< LSB_Qxyzw<ExtractZ> >;
	using L_QW = LS_ElementAccessBase< LSB_Qxyzw<ExtractW> >;

	luabind::module( l )
		[
		def( "_QX", tag_bind( L_QX( statusbuffer ) ) ),
		def( "_QY", tag_bind( L_QY( statusbuffer ) ) ),
		def( "_QZ", tag_bind( L_QZ( statusbuffer ) ) ),
		def( "_QW", tag_bind( L_QW( statusbuffer ) ) )
		];

	using L_WX = LS_ElementAccessBase< LSB_Wxyz<ExtractLocalX_N> >;
	using L_WY = LS_ElementAccessBase< LSB_Wxyz<ExtractLocalY> >;
	using L_WZ = LS_ElementAccessBase< LSB_Wxyz<ExtractLocalZ_N> >;

	luabind::module( l )
		[
		def( "_WX", tag_bind( L_WX( statusbuffer ) ) ),
		def( "_WY", tag_bind( L_WY( statusbuffer ) ) ),
		def( "_WZ", tag_bind( L_WZ( statusbuffer ) ) )
		];

	using L_PARENT = LS_ElementAccessBase< LSB_PARENT>;
	luabind::module( l )
		[
		def( "_PARENT", tag_function<int (int)>( L_PARENT( statusbuffer ) ) ),
		def( "_TOP", tag_function<int (int)>( LS_TOP( statusbuffer )) )
			// 									def( "_BYE", tag_bind( L_BYE( m ) ) ) TODO: re impl
		];

	// 								luabind::module( l )
	// 									[
	// 									def( "_H",
	// 											tag_bind(
	// 												L_H<ModelType> {m, boost::bind( &LuaEnvironmentType::getLandAlt, modelLuaEnv, _1, _2 )}
	// 												) )
	// 									];
}
}

}
}
#endif
