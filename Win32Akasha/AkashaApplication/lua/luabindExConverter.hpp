#ifndef LUABIND_EX_CONVERTER__HPP
#define LUABIND_EX_CONVERTER__HPP

#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>

namespace luabindEx
{
////////////////////////////////////////////////////////////////
namespace converter
{
template<class T>
	//struct default_converter<boost::optional<std::string>>
	struct optional_string_converter_generator
	: ::luabind::native_converter_base<boost::optional<T>>
{
	static int compute_score(lua_State* L, int index)
	{
		switch (lua_type(L, index))
		{
		case LUA_TNIL:
		case LUA_TSTRING:
			return 0;
		default:
			return -1;
		}
	}

	boost::optional<T> from(lua_State* L, int index)
	{
		switch (lua_type(L, index))
		{
		case LUA_TNIL:
			return boost::optional<T>();
		case LUA_TSTRING:
		default:
			return T(lua_tostring(L, index));
		}
	}

	void to(lua_State* L, boost::optional<T> const& x)
	{
		if (x)
			lua_pushstring(L, x->c_str());
		else
			lua_pushnil(L);
	}
};

template<class T>
struct optional_number_converter_generator
: ::luabind::native_converter_base<boost::optional<T>>
{
	typedef boost::optional<T> optionT_t;
	static int compute_score(lua_State* L, int index)
	{
		return 0;
		/*
			switch (lua_type(L, index))
			{
			case LUA_TNIL:
			case LUA_TNUMBER:
			return 0;
			default:
			return -1;
			}*/
	}

	boost::optional<T> from(lua_State* L, int index)
	{
		switch (lua_type(L, index))
		{
		case LUA_TNIL:
			return boost::none; //optionT_t();
		case LUA_TNUMBER:
			return T(lua_tonumber(L, index));
		default:
			return boost::none;
		}
	}

	void to(lua_State* L, optionT_t const& x)
	{
		if (x)
			lua_pushnumber(L, *x);
		else
			lua_pushnil(L);
	}
};


}
}


////////////////////////////////////////////////////////////////
// “o˜^
namespace luabind
{
template<>
	struct default_converter<boost::optional<std::string>> :
	::luabindEx::converter::optional_string_converter_generator<std::string>{};

template <>
	struct default_converter<boost::optional<std::string> const&>
	: default_converter<boost::optional<std::string>>
	{};

template<>
	struct default_converter<boost::optional<int>> :
	::luabindEx::converter::optional_number_converter_generator<int>{};

template<>
	struct default_converter<boost::optional<int> const&>
	: default_converter<boost::optional<int>>
	{};

template<>
	struct default_converter<boost::optional<float>> :
	::luabindEx::converter::optional_number_converter_generator<float>{};

template<>
	struct default_converter<boost::optional<float> const&>
	: default_converter<boost::optional<float>>
	{};
}

#endif
