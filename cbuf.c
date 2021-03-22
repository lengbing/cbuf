#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef _MSC_VER
#define LOS_EXPORT __declspec(dllexport)
#else
#define LOS_EXPORT extern
#endif


typedef struct cbuf
{
    size_t size;
    char buf[];
} cbuf;


#define cbuf_header(b) (cbuf*)((b) - sizeof(cbuf))


#define to_cbuf(cb, idx) \
    luaL_argexpected(L, lua_islightuserdata(L, idx), idx, lua_typename(L, LUA_TLIGHTUSERDATA)); \
    char* cb##buf = lua_touserdata(L, idx);                                                     \
    if (!cb##buf) {                                                                             \
        luaL_argerror(L, idx, "null pointer");                                                  \
        return 0;                                                                               \
    }                                                                                           \
    cbuf* cb = cbuf_header(cb##buf)


static int cbuf_new(lua_State* L)
{
    luaL_argexpected(L, lua_isinteger(L, 1), 1, "integer");
    size_t size = lua_tointeger(L, 1);
    cbuf* cb = malloc(sizeof(cbuf) + size + 1);
    if (!cb) {
        return 0;
    }
    cb->size = size;
    cb->buf[size] = '\0';
    lua_pushlightuserdata(L, cb->buf);
    return 1;
}


static int cbuf_delete(lua_State* L)
{
    to_cbuf(cb, 1);
    free(cb);
    return 0;
}


static int cbuf_size(lua_State* L)
{
    to_cbuf(cb, 1);
    lua_pushinteger(L, cb->size);
    return 1;
}


static int cbuf_slice(lua_State* L)
{
    to_cbuf(cb, 1);
    luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
    size_t index = lua_tointeger(L, 2);
    if (index >= cb->size) {
        luaL_argerror(L, 2, "index out of range");
        return 0;
    }
    lua_pushlightuserdata(L, cb->buf + index);
    return 1;
}


static int cbuf_shift(lua_State* L)
{
    to_cbuf(cb, 1);
    luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
    luaL_argexpected(L, lua_isinteger(L, 3), 3, "integer");
    luaL_argexpected(L, lua_isinteger(L, 4), 4, "integer");
    size_t index = lua_tointeger(L, 2);
    if (index >= cb->size) {
        luaL_argerror(L, 2, "index out of range");
        return 0;
    }
    size_t length = lua_tointeger(L, 3);
    if (index + length > cb->size) {
        luaL_argerror(L, 3, "length out of range");
        return 0;
    }
    ptrdiff_t shift = lua_tointeger(L, 4);
    if (index + shift < 0 || index + length + shift > cb->size) {
        luaL_argerror(L, 4, "shift out of range");
        return 0;
    }
    memmove(cb->buf + index + shift, cb->buf + index, length);
    return 0;
}


static int cbuf_zero(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t index = 0;
    int top = lua_gettop(L);
    if (top >= 2) {
        luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
        index = lua_tointeger(L, 2);
        if (index >= cb->size) {
            luaL_argerror(L, 2, "index out of range");
            return 0;
        }
    }
    size_t length = cb->size - index;
    if (top >= 3) {
        luaL_argexpected(L, lua_isinteger(L, 3), 3, "integer");
        length = lua_tointeger(L, 3);
        if (index + length > cb->size) {
            luaL_argerror(L, 3, "length out of range");
            return 0;
        }
    }
    memset(cb->buf + index, 0, length);
    return 0;
}


static int cbuf_copy(lua_State* L)
{
    to_cbuf(destcb, 1);
    luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
    size_t destidx = lua_tointeger(L, 2);
    if (destidx >= destcb->size) {
        luaL_argerror(L, 2, "index out of range");
        return 0;
    }
    to_cbuf(srccb, 3);
    luaL_argexpected(L, lua_isinteger(L, 4), 4, "integer");
    size_t srcidx = lua_tointeger(L, 4);
    if (srcidx >= srccb->size) {
        luaL_argerror(L, 4, "index out of range");
        return 0;
    }
    luaL_argexpected(L, lua_isinteger(L, 5), 5, "integer");
    size_t length = lua_tointeger(L, 5);
    if (destidx + length > destcb->size || srcidx + length > srccb->size) {
        luaL_argerror(L, 5, "length out of range");
        return 0;
    }
    memcpy(destcb->buf + destidx, srccb->buf + srcidx, length);
    return 0;
}


static int cbuf_copystring(lua_State* L)
{
    to_cbuf(destcb, 1);
    luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
    size_t destidx = lua_tointeger(L, 2);
    if (destidx >= destcb->size) {
        luaL_argerror(L, 2, "index out of range");
        return 0;
    }
    luaL_argexpected(L, lua_isstring(L, 3), 3, lua_typename(L, LUA_TSTRING));
    size_t srcsize;
    const char* srcstr = lua_tolstring(L, 3, &srcsize);
    luaL_argexpected(L, lua_isinteger(L, 4), 4, "integer");
    size_t srcidx = lua_tointeger(L, 4);
    luaL_argexpected(L, lua_isinteger(L, 5), 5, "integer");
    size_t length = lua_tointeger(L, 5);
    if (destidx + length > destcb->size || srcidx + length > srcsize) {
        luaL_argerror(L, 5, "length out of range");
        return 0;
    }
    memcpy(destcb->buf + destidx, srcstr + srcidx, length);
    return 0;
}


static int cbuf_tostring(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t index = 0;
    int top = lua_gettop(L);
    if (top >= 2) {
        luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
        index = lua_tointeger(L, 2);
        if (index >= cb->size) {
            luaL_argerror(L, 2, "index out of range");
            return 0;
        }
    }
    lua_pushstring(L, cb->buf + index);
    return 1;
}


static int cbuf_tolstring(lua_State* L)
{
    to_cbuf(cb, 1);
    luaL_argexpected(L, lua_isinteger(L, 2), 2, "integer");
    luaL_argexpected(L, lua_isinteger(L, 3), 3, "integer");
    size_t index = lua_tointeger(L, 2);
    if (index >= cb->size) {
        luaL_argerror(L, 2, "index out of range");
        return 0;
    }
    size_t length = lua_tointeger(L, 3);
    if (index + length > cb->size) {
        luaL_argerror(L, 3, "length out of range");
        return 0;
    }
    lua_pushlstring(L, cb->buf + index, length);
    return 1;
}


LOS_EXPORT int luaopen_cbuf(lua_State* L)
{
    luaL_Reg lib[] = {
        {"new", cbuf_new},
        {"delete", cbuf_delete},
        {"size", cbuf_size},
        {"slice", cbuf_slice},
        {"shift", cbuf_shift},
        {"zero", cbuf_zero},
        {"copy", cbuf_copy},
        {"copystring", cbuf_copystring},
        {"tostring", cbuf_tostring},
        {"tolstring", cbuf_tolstring},
        {NULL, NULL}
    };
    luaL_newlib(L, lib);
    return 1;
}