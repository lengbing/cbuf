#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef _WIN32
#define LUA_MOD_EXPORT __declspec(dllexport)
#else
#define LUA_MOD_EXPORT extern
#endif


#define CBUFNAME "CBUF*"


typedef struct cbuf
{
    size_t size;
    int nuv;
    char* buf;
} cbuf;


static int api_new(lua_State* L)
{
    size_t size = luaL_checkinteger(L, 1);
    int nuv = 0;
    if (lua_gettop(L) >= 2) {
        nuv = (int)luaL_checkinteger(L, 2);
    }
    cbuf* cb = lua_newuserdatauv(L, sizeof(cbuf), nuv);
    if (!cb) {
        return luaL_error(L, "create userdata failed");
    }
    char* buf = malloc(size + 1);
    if (!buf) {
        return luaL_error(L, "create buffer failed");
    }
    buf[size] = '\0';
    cb->size = size;
    cb->nuv = nuv;
    cb->buf = buf;
    luaL_setmetatable(L, CBUFNAME);
    return 1;
}


static int api_delete(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    free(cb->buf);
    return 0;
}


static int api_uvcount(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    lua_pushinteger(L, cb->nuv);
    return 1;
}


static int api_getuv(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    int index = (int)luaL_checkinteger(L, 2);
    lua_getiuservalue(L, 1, index);
    return 1;
}


static int api_setuv(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    int index = (int)luaL_checkinteger(L, 2);
    lua_settop(L, 3);
    lua_setiuservalue(L, 1, index);
    return 0;
}


static int api_raw(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    lua_pushlightuserdata(L, cb->buf);
    return 1;
}


static int api_shift(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = luaL_checkinteger(L, 2);
    size_t length = luaL_checkinteger(L, 3);
    ptrdiff_t shift = luaL_checkinteger(L, 4);
    if (index >= cb->size) {
        return luaL_error(L, "index out of range");
    }
    if (index + length > cb->size) {
        return luaL_error(L, "length out of range");
    }
    if (shift < 0 && (size_t)(-shift) > index) {
        return luaL_error(L, "shift out of range");
    }
    if (shift > 0 && index + length + (size_t)shift > cb->size) {
        return luaL_error(L, "shift out of range");
    }
    memmove(cb->buf + index + shift, cb->buf + index, length);
    return 0;
}


static int api_zero(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = 0;
    int top = lua_gettop(L);
    if (top >= 2) {
        index = luaL_checkinteger(L, 2);
        if (index >= cb->size) {
            return luaL_error(L, "index out of range");
        }
    }
    size_t length = cb->size - index;
    if (top >= 3) {
        length = luaL_checkinteger(L, 3);
        if (index + length > cb->size) {
            return luaL_error(L, "length out of range");
        }
    }
    memset(cb->buf + index, 0, length);
    return 0;
}


static int api_copy(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = luaL_checkinteger(L, 2);
    index = luaL_checkinteger(L, 2);
    if (index >= cb->size) {
        return luaL_error(L, "index out of range");
    }
    luaL_argexpected(L, lua_islightuserdata(L, 3), 3, lua_typename(L, LUA_TLIGHTUSERDATA));
    const char* src = lua_touserdata(L, 3);
    size_t length = luaL_checkinteger(L, 4);
    if (index + length > cb->size) {
        return luaL_error(L, "length out of range");
    }
    memcpy(cb->buf + index, src, length);
    return 0;
}


static int api_copystring(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = luaL_checkinteger(L, 2);
    index = luaL_checkinteger(L, 2);
    if (index >= cb->size) {
        return luaL_error(L, "index out of range");
    }
    size_t length;
    const char* src = luaL_checklstring(L, 3, &length);
    if (lua_gettop(L) >= 4) {
        length = luaL_checkinteger(L, 4);
    }
    if (index + length > cb->size) {
        return luaL_error(L, "length out of range");
    }
    memcpy(cb->buf + index, src, length);
    return 0;
}


static int api_substring(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = 0;
    if (lua_gettop(L) >= 2) {
        index = luaL_checkinteger(L, 2);
        if (index >= cb->size) {
            return luaL_error(L, "index out of range");
        }
    }
    lua_pushstring(L, cb->buf + index);
    return 1;
}


static int api_sublstring(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    size_t index = 0;
    size_t length = cb->size;
    int top = lua_gettop(L);
    if (top >= 2) {
        index = luaL_checkinteger(L, 2);
        if (index >= cb->size) {
            return luaL_error(L, "index out of range");
        }
    }
    if (top >= 3) {
        length = luaL_checkinteger(L, 3);
        if (index + length > cb->size) {
            return luaL_error(L, "length out of range");
        }
    }
    lua_pushlstring(L, cb->buf + index, length);
    return 1;
}


static int api_magic_index(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    if (lua_isinteger(L, 2)) {
        size_t index = lua_tointeger(L, 2);
        if (index >= cb->size) {
            return luaL_error(L, "index out of range");
        }
        lua_pushlstring(L, cb->buf + index, 1);
        return 1;
    }
    if (luaL_getmetatable(L, CBUFNAME) == LUA_TNIL) {
        return luaL_error(L, "no cbuf metatable");
    }
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);
    return 1;
}


static int api_magic_newindex(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    if (!lua_isinteger(L, 2)) {
        return luaL_error(L, "invalid key");
    }
    size_t index = lua_tointeger(L, 2);
    if (index >= cb->size) {
        return luaL_error(L, "index out of range");
    }
    size_t length;
    const char* src = luaL_checklstring(L, 3, &length);
    if (index + length > cb->size) {
        return luaL_error(L, "length out of range");
    }
    memcpy(cb->buf + index, src, length);
    return 0;
}


static int api_magic_len(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    lua_pushinteger(L, cb->size);
    return 1;
}


static int api_magic_tostring(lua_State* L)
{
    cbuf* cb = luaL_checkudata(L, 1, CBUFNAME);
    lua_pushstring(L, cb->buf);
    return 1;
}


LUA_MOD_EXPORT int luaopen_cbuf(lua_State* L)
{
    luaL_newmetatable(L, CBUFNAME);
    luaL_Reg methods[] = {
        {"delete", api_delete},
        {"uvcount", api_uvcount},
        {"getuv", api_getuv},
        {"setuv", api_setuv},
        {"raw", api_raw},
        {"shift", api_shift},
        {"zero", api_zero},
        {"copy", api_copy},
        {"copystring", api_copystring},
        {"substring", api_substring},
        {"sublstring", api_sublstring},
        {"__index", api_magic_index},
        {"__newindex", api_magic_newindex},
        {"__len", api_magic_len},
        {"__tostring", api_magic_tostring},
        {NULL, NULL}
    };
    luaL_setfuncs(L, methods, 0);

    luaL_Reg lib[] = {
        {"new", api_new},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}
