/*
 *  Copyright 2014-present Masatoshi Fukunaga. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *  hex.c
 *  lua-hex
 *
 *  Created by Masatoshi Teruya on 14/12/06.
 *
 */
// project header
#include "hexcodec.h"
// lua
#include <lauxlib.h>
#include <lua.h>
// system
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define lstate_fn2tbl(L, k, v)                                                 \
    do {                                                                       \
        lua_pushstring(L, k);                                                  \
        lua_pushcfunction(L, v);                                               \
        lua_rawset(L, -3);                                                     \
    } while (0)

#define pnalloc(n, t) (t *)malloc((n) * sizeof(t))
#define pdealloc(p)   free((void *)p)

static int encode_lua(lua_State *L)
{
    size_t len      = 0;
    const char *str = luaL_checklstring(L, 1, &len);
    // dest length must be greater than len*2 + 1(null-term)
    size_t dlen     = len * 2;
    char *dest      = NULL;
    luaL_Buffer b   = {0};

    // integer overflow or no-mem error
    if (dlen == SIZE_MAX || dlen < len) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(ENOMEM));
        lua_pushinteger(L, ENOMEM);
        return 3;
    }

    // allocate dest buffer and encode src to hex-encoded dest
    luaL_buffinit(L, &b);

#if LUA_VERSION_NUM >= 502
    dest = luaL_prepbuffsize(&b, dlen);
    hex_encode(dest, dlen, (char *)str, len);
    luaL_addsize(&b, dlen);

#else
    do {
        size_t used = 0;

        dest = luaL_prepbuffer(&b);
        used = hex_encode(dest, LUAL_BUFFERSIZE, (char *)str, len);
        if (used == (size_t)-1) {
            return luaL_error(L, "BUG: destination buffer is too small");
        }
        luaL_addsize(&b, used * 2);
        str += used;
        len -= used;
    } while (len > 0);
#endif

    luaL_pushresult(&b);
    return 1;
}

static int decode_lua(lua_State *L)
{
    size_t len      = 0;
    const char *src = luaL_checklstring(L, 1, &len);
    size_t dlen     = len / 2;
    char *dest      = NULL;
    luaL_Buffer b   = {0};

    // check hex-encoded src length
    if (len % 2) {
        errno = EINVAL;
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        lua_pushinteger(L, errno);
        return 3;
    }

    // allocate dest buffer and decode hex-encoded src to dest
    luaL_buffinit(L, &b);

#if LUA_VERSION_NUM >= 502
    dest = luaL_prepbuffsize(&b, dlen);
    if (hex_decode(dest, dlen, (char *)src, len) == (size_t)-1) {
        if (errno == EILSEQ) {
            lua_pushnil(L);
            lua_pushstring(L, strerror(errno));
            lua_pushinteger(L, errno);
            return 3;
        }
        // destination buffer too small should never happen here
        return luaL_error(L, "BUG: destination buffer is too small");
    }
    luaL_addsize(&b, dlen);

#else
    dlen = LUAL_BUFFERSIZE;
    do {
        size_t used = 0;

        dest = luaL_prepbuffer(&b);
        used = hex_decode(dest, dlen, (char *)src, len);
        if (used == (size_t)-1) {
            if (errno == EILSEQ) {
                lua_pushnil(L);
                lua_pushstring(L, strerror(errno));
                lua_pushinteger(L, errno);
                return 3;
            }
            return luaL_error(L, "BUG: destination buffer is too small");
        }
        luaL_addsize(&b, used / 2);
        src += used;
        len -= used;
    } while (len > 0);
#endif

    luaL_pushresult(&b);
    return 1;
}

LUALIB_API int luaopen_hex(lua_State *L)
{
    lua_createtable(L, 0, 2);
    lstate_fn2tbl(L, "encode", encode_lua);
    lstate_fn2tbl(L, "decode", decode_lua);

    return 1;
}
