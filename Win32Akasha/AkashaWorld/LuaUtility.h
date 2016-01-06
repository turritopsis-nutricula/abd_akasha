#ifndef LUA_UTILITY__H
#define LUA_UTILITY__H

#pragma unmanaged
#include <string>
#include <map>

//Lua
#include "lua.hpp"

//Akasha
#include "akashaCommon.h"

#pragma comment(lib,"Lua51.lib")

namespace akasha {
  namespace lua{
    typedef std::string LuaVarName;
    typedef float LuaNumber;

    //! Luaのグローバル空間に変数を登録
    static void RegistVarNumber(lua_State* L, const LuaVarName& n,  LuaNumber v) {
      lua_pushnumber(L, v);
      lua_setglobal(L, n.c_str());
    };

    //! std::map<string,string>をテーブルにして登録
    static void RegistMap(lua_State* L, std::map<std::string,std::string>& m) {
      lua_newtable(L);
      std::map<std::string,std::string>::iterator it = m.begin();
      while (it != m.end() ) {
        lua_pushstring(L, (*it).first.c_str());
        lua_pushstring(L, (*it).second.c_str());
        lua_settable(L, -3);
        it++;
      }
    };

    //! int値をLuaのtableからもらう。無効だった場合の初期値も。
    template <typename T>
    static T getInteger(lua_State* L, const int tableIndex, const char* field, const T def = 0){
      T result;
      lua_getfield(L, tableIndex, field); // フィールドに値を積む
      // ターゲットが数値、数値に値する文字列であれば
#ifdef _DEBUG
      //if (!lua_isnumber(L, -1))
        //OutDebug( _T("getInterger:フィールド不良:%s:初期値 = %i\n"), field, def);
#endif //_DEBUG
      result = lua_isnumber(L, -1) ? static_cast<T>(lua_tointeger(L,-1)) : def;

      lua_pop(L,1);             // 積んだ値をpop
      return result;
    };

    //! 実数値をLuaのテーブルから貰う。
    template <typename T>
    static T getNumber(lua_State* L, const int tableIndex, const char* field, const T def = 0.0){
      T result;
      lua_getfield(L, tableIndex,field);
#ifdef _DEBUG
      //if (!lua_isnumber(L, -1))
        //OutDebug( _T("getFloat:フィールド不良:%s:初期値 = %f\n"), field, def);
#endif //_DEBUG
      result = lua_isnumber(L,-1) ? static_cast<T>(lua_tonumber(L, -1)) : def;
      lua_pop(L,1);
      return result;
    };

    //! Luaスタック内をダンプ
    //TODO:printfをどうにかする
    static void dumpStack(lua_State* L)  {
        int i;
        //スタックに積まれている数を取得する
        int stackSize = lua_gettop(L);
        for( i = stackSize; i >= 1; i-- ) {
                int type = lua_type(L, i);
                printf("Stack[%2d-%10s] : ", i, lua_typename(L,type) );

                switch( type ) {
                case LUA_TNUMBER:
                        //number型
                        printf("%f", lua_tonumber(L, i) );
                        break;
                case LUA_TBOOLEAN:
                        //boolean型
                        if( lua_toboolean(L, i) ) {
                                printf("true");
                        }else{
                                printf("false");
                        }
                        break;
                case LUA_TSTRING:
                        //string型
                        printf("%s", lua_tostring(L, i) );
                        break;
                case LUA_TNIL:
                        //nil
                        break;
                default:
                        //その他の型
                        printf("%s", lua_typename(L, type));
                        break;
                }
                printf("\n");
        }
        printf("\n");
      };



  }
}

#endif //LUA_UTILITY__H
