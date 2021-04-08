#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef _WIN32
#define LUA_MOD_EXPORT __declspec(dllexport)
#else
#define LUA_MOD_EXPORT extern
#endif


typedef struct cbuf
{
    size_t size;
    lua_Integer uvs[];
} cbuf;


#define buf_nuvs(b) *((lua_Integer*)((b) - sizeof(lua_Integer)))
#define buf_header(b) ((cbuf*)((b) - (sizeof(cbuf) + sizeof(lua_Integer) * (buf_nuvs(b) + 2))))
#define cb_size(cb) (cb)->size
#define cb_nuvs(cb) (cb)->uvs[0]
#define cb_uv(cb, i) (cb)->uvs[(i)]
#define cb_nuvs2(cb) cb_uv((cb), cb_nuvs(cb) + 1)
#define cb_buf(cb) (char*)&cb_uv((cb), cb_nuvs(cb) + 2)

#define to_buf(buf, idx) \
    luaL_argexpected(L, lua_islightuserdata(L, idx), idx, lua_typename(L, LUA_TLIGHTUSERDATA)); \
    char* buf = lua_touserdata(L, idx);                                                         \
    if (!buf) {                                                                                 \
        luaL_argerror(L, idx, "null pointer");                                                  \
        return 0;                                                                               \
    }                                          

#define to_cbuf(cb, idx) \
    to_buf(cb##buf, idx) \
    cbuf* cb = buf_header(cb##buf)


static int cbuf_new(lua_State* L)
{
    size_t size = luaL_checkinteger(L, 1);
    size_t nuvs = 0;
    if (lua_gettop(L) >= 2) {
        nuvs = luaL_checkinteger(L, 2);
        if (nuvs > UINT8_MAX) {
            luaL_argerror(L, 2, "too many user values");
            return 0;
        }
    }
    cbuf* cb = malloc(sizeof(cbuf) + sizeof(lua_Integer) * nuvs + sizeof(char) * (size + 2));
    if (!cb) {
        return 0;
    }
    cb_size(cb) = size;
    cb_nuvs(cb) = nuvs;
    for (size_t i = 1; i <= nuvs; ++i) {
        cb_uv(cb, i) = 0;
    }
    cb_nuvs2(cb) = nuvs;
    char* buf = cb_buf(cb);
    buf[size] = '\0';
    lua_pushlightuserdata(L, buf);
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
    lua_pushinteger(L, cb_size(cb));
    return 1;
}


static int cbuf_uvcount(lua_State* L)
{
    to_buf(buf, 1);
    lua_pushinteger(L, buf_nuvs(buf));
    return 1;
}


static int cbuf_getuv(lua_State* L)
{
    to_cbuf(cb, 1);
    lua_Integer i = luaL_checkinteger(L, 2);
    if (i < 1 || i > cb_nuvs(cb)) {
        luaL_argerror(L, 2, "uv index out of range");
        return 0;
    }
    lua_pushinteger(L, cb_uv(cb, i));
    return 1;
}


static int cbuf_setuv(lua_State* L)
{
    to_cbuf(cb, 1);
    lua_Integer i = luaL_checkinteger(L, 2);
    lua_Integer v = luaL_checkinteger(L, 3);
    if (i < 1 || i > cb_nuvs(cb)) {
        luaL_argerror(L, 2, "uv index out of range");
        return 0;
    }
    cb_uv(cb, i) = v;
    return 0;
}


static int cbuf_slice(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t offset = luaL_checkinteger(L, 2);
    if (offset >= cb_size(cb)) {
        luaL_argerror(L, 2, "offset out of range");
        return 0;
    }
    lua_pushlightuserdata(L, cbbuf + offset);
    return 1;
}


static int cbuf_shift(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t offset = luaL_checkinteger(L, 2);
    if (offset >= cb->size) {
        luaL_argerror(L, 2, "offset out of range");
        return 0;
    }
    size_t length = luaL_checkinteger(L, 3);
    if (offset + length > cb_size(cb)) {
        luaL_argerror(L, 3, "length out of range");
        return 0;
    }
    ptrdiff_t shift = luaL_checkinteger(L, 4);
    if (offset + shift < 0 || offset + length + shift > cb_size(cb)) {
        luaL_argerror(L, 4, "shift out of range");
        return 0;
    }
    memmove(cbbuf + offset + shift, cbbuf + offset, length);
    return 0;
}


static int cbuf_zero(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t offset = 0;
    int top = lua_gettop(L);
    if (top >= 2) {
        offset = luaL_checkinteger(L, 2);
        if (offset >= cb_size(cb)) {
            luaL_argerror(L, 2, "offset out of range");
            return 0;
        }
    }
    size_t length = cb_size(cb) - offset;
    if (top >= 3) {
        length = luaL_checkinteger(L, 3);
        if (offset + length > cb_size(cb)) {
            luaL_argerror(L, 3, "length out of range");
            return 0;
        }
    }
    memset(cbbuf + offset, 0, length);
    return 0;
}


static int cbuf_copy(lua_State* L)
{
    to_cbuf(destcb, 1);
    size_t destoffset = luaL_checkinteger(L, 2);
    if (destoffset >= cb_size(destcb)) {
        luaL_argerror(L, 2, "offset out of range");
        return 0;
    }
    to_cbuf(srccb, 3);
    size_t srcoffset = luaL_checkinteger(L, 4);
    if (srcoffset >= cb_size(srccb)) {
        luaL_argerror(L, 4, "offset out of range");
        return 0;
    }
    size_t length = luaL_checkinteger(L, 5);
    if (destoffset + length > cb_size(destcb) || srcoffset + length > cb_size(srccb)) {
        luaL_argerror(L, 5, "length out of range");
        return 0;
    }
    memcpy(destcbbuf + destoffset, srccbbuf + srcoffset, length);
    return 0;
}


static int cbuf_copystring(lua_State* L)
{
    to_cbuf(destcb, 1);
    size_t destoffset = luaL_checkinteger(L, 2);
    if (destoffset >= cb_size(destcb)) {
        luaL_argerror(L, 2, "offset out of range");
        return 0;
    }
    size_t srcsize;
    const char* srcstr = luaL_checklstring(L, 3, &srcsize);
    size_t srcoffset = luaL_checkinteger(L, 4) - 1;
    size_t length = luaL_checkinteger(L, 5);
    if (destoffset + length > cb_size(destcb) || srcoffset + length > srcsize) {
        luaL_argerror(L, 5, "length out of range");
        return 0;
    }
    memcpy(destcbbuf + destoffset, srcstr + srcoffset, length);
    return 0;
}


static int cbuf_tostring(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t offset = 0;
    int top = lua_gettop(L);
    if (top >= 2) {
        offset = luaL_checkinteger(L, 2);
        if (offset >= cb->size) {
            luaL_argerror(L, 2, "offset out of range");
            return 0;
        }
    }
    lua_pushstring(L, cbbuf + offset);
    return 1;
}


static int cbuf_tolstring(lua_State* L)
{
    to_cbuf(cb, 1);
    size_t offset = luaL_checkinteger(L, 2);
    if (offset >= cb_size(cb)) {
        luaL_argerror(L, 2, "offset out of range");
        return 0;
    }
    size_t length = luaL_checkinteger(L, 3);
    if (offset + length > cb_size(cb)) {
        luaL_argerror(L, 3, "length out of range");
        return 0;
    }
    lua_pushlstring(L, cbbuf + offset, length);
    return 1;
}


LUA_MOD_EXPORT int luaopen_cbuf(lua_State* L)
{
    luaL_Reg lib[] = {
        {"new", cbuf_new},
        {"delete", cbuf_delete},
        {"size", cbuf_size},
        {"uvcount", cbuf_uvcount},
        {"getuv", cbuf_getuv},
        {"setuv", cbuf_setuv},
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