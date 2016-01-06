#ifndef LUA_ENVIONMENT__HPP
#define LUA_ENVIONMENT__HPP

#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>

#include <lua.hpp>

#include <Riceball/singleton/SingletonHolder.hpp>

#include <sstream>

namespace {
////////////////////////////////////////////////////////////////
//エラーハンドラ

int lua_error_handler(lua_State* L)
{
	lua_Debug d = {};
	std::stringstream msg;

	// スタックからエラーメッセージを取得する
	std::string err = lua_tostring(L, -1);

	msg << "ERROR: " << err << "\n\nBacktrace:\n";

	for (int stack_depth = 1; lua_getstack(L, stack_depth, &d); ++stack_depth)
	{
		lua_getinfo(L, "Sln", &d);
		msg << "#" << stack_depth << " ";
		if (d.name)
			msg << "<" << d.namewhat << "> \"" << d.name << "\"";
		else
			msg << "--";
		msg << " (called";
		if (d.currentline > 0)
			msg << " at line " << d.currentline;
		msg << " in ";
		if (d.linedefined > 0)
			msg << "function block between line " << d.linedefined << ".." << d.lastlinedefined << " of ";
		msg << d.short_src;
		msg << ")\n";
	}

	std::cout << msg.str().c_str() << std::endl;

	// スタックに積まれているエラーメッセージを、新しい文字列に置換する。
	lua_pop(L, 1);
	lua_pushstring(L, msg.str().c_str());

	//   std::cout << msg.str() << std::endl;

	return 1;
}

}

////////////////////////////////////////////////////////////////
//luabind 拡張
namespace luabindEx
{
////////////////////////////////////////////////////////////////
// Functor登録
namespace detail
{
template <class F, class Sig, class Policies>
	struct functor_registration : ::luabind::detail::registration
{
	functor_registration(char const* name, F f, Policies const& policies)
		: name(name)
		  , f(f)
		  , policies(policies)
	{}

	void register_(lua_State* L) const
	{
		/*::luabind::object fn  =
		  ::luabind::make_function(L,f,Sig() , ::luabind::detail::null_type());*/
		::luabind::object fn  =
			::luabind::make_function(L,f,Sig() , policies);

		::luabind::detail::add_overload(
				::luabind::object(::luabind::from_stack(L, -1))
				, name
				, fn
				);
	}

	char const* name;
	F f;
	Policies policies;
};
}

	template<class F, class Sig, class Policies>
::luabind::scope defFunctor(char const* name,F f,const Sig&, Policies const& policies)
{

	return ::luabind::scope(
			std::auto_ptr< ::luabind::detail::registration>(
				new detail::functor_registration<F, Sig,Policies>(name, f, policies)));
}
	template<class F, class Sig>
::luabind::scope defFunctor(char const* name, F f,const Sig& s)
{
	return defFunctor(name, f, Sig(), ::luabind::detail::null_type());
}


}


namespace akasha
{
namespace lua
{
////////////////////////////////////////////////////////////////
//Lua環境
class LuaEnvironment : boost::noncopyable
	{
		lua_State* m_luaState;
	public:
		LuaEnvironment() : m_luaState(0)
		{}
		~LuaEnvironment()
		{
			if (m_luaState)
				lua_close(m_luaState);
		}

		lua_State* open()
		{
			m_luaState = lua_open();
			luabind::open(m_luaState);
			return m_luaState;
		}

		lua_State* getState() const
		{
			return m_luaState;
		}
	};


//簡易的なluabindユーティリティ
// ファンクタをグローバル空間に登録
template<typename StringType,class F, class Sig>
	void bindGrobal(lua_State* L, const StringType* n, F&& f, Sig)
	{
		::luabind::globals(L)[n] =
			::luabind::make_function(L,f,Sig(), ::luabind::detail::null_type());
	}


}
}


#endif
