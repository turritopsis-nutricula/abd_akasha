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

    //! Lua�̃O���[�o����Ԃɕϐ���o�^
    static void RegistVarNumber(lua_State* L, const LuaVarName& n,  LuaNumber v) {
      lua_pushnumber(L, v);
      lua_setglobal(L, n.c_str());
    };

    //! std::map<string,string>���e�[�u���ɂ��ēo�^
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

    //! int�l��Lua��table������炤�B�����������ꍇ�̏����l���B
    template <typename T>
    static T getInteger(lua_State* L, const int tableIndex, const char* field, const T def = 0){
      T result;
      lua_getfield(L, tableIndex, field); // �t�B�[���h�ɒl��ς�
      // �^�[�Q�b�g�����l�A���l�ɒl���镶����ł����
#ifdef _DEBUG
      //if (!lua_isnumber(L, -1))
        //OutDebug( _T("getInterger:�t�B�[���h�s��:%s:�����l = %i\n"), field, def);
#endif //_DEBUG
      result = lua_isnumber(L, -1) ? static_cast<T>(lua_tointeger(L,-1)) : def;

      lua_pop(L,1);             // �ς񂾒l��pop
      return result;
    };

    //! �����l��Lua�̃e�[�u������Ⴄ�B
    template <typename T>
    static T getNumber(lua_State* L, const int tableIndex, const char* field, const T def = 0.0){
      T result;
      lua_getfield(L, tableIndex,field);
#ifdef _DEBUG
      //if (!lua_isnumber(L, -1))
        //OutDebug( _T("getFloat:�t�B�[���h�s��:%s:�����l = %f\n"), field, def);
#endif //_DEBUG
      result = lua_isnumber(L,-1) ? static_cast<T>(lua_tonumber(L, -1)) : def;
      lua_pop(L,1);
      return result;
    };

    //! Lua�X�^�b�N�����_���v
    //TODO:printf���ǂ��ɂ�����
    static void dumpStack(lua_State* L)  {
        int i;
        //�X�^�b�N�ɐς܂�Ă��鐔���擾����
        int stackSize = lua_gettop(L);
        for( i = stackSize; i >= 1; i-- ) {
                int type = lua_type(L, i);
                printf("Stack[%2d-%10s] : ", i, lua_typename(L,type) );

                switch( type ) {
                case LUA_TNUMBER:
                        //number�^
                        printf("%f", lua_tonumber(L, i) );
                        break;
                case LUA_TBOOLEAN:
                        //boolean�^
                        if( lua_toboolean(L, i) ) {
                                printf("true");
                        }else{
                                printf("false");
                        }
                        break;
                case LUA_TSTRING:
                        //string�^
                        printf("%s", lua_tostring(L, i) );
                        break;
                case LUA_TNIL:
                        //nil
                        break;
                default:
                        //���̑��̌^
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
