/*
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <lua.h>
#include <lauxlib.h>
#include "hexcodec.h"

#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define pnalloc(n,t)    (t*)malloc( (n) * sizeof(t) )
#define pdealloc(p)     free((void*)p)


static int encode_lua( lua_State *L )
{
    size_t len = 0;
    const char *str = luaL_checklstring( L, 1, &len );
    // dest length must be greater than len*2 + 1(null-term)
    size_t dlen = len * 2;
    unsigned char *dest = NULL;

    // integer overflow or no-mem error
    if( dlen == SIZE_MAX || dlen < len ||
        !( dest = pnalloc( dlen, unsigned char ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( ENOMEM ) );
        return 2;
    }

    hex_encode( dest, (unsigned char*)str, len );
    lua_pushlstring( L, (const char*)dest, dlen );
    pdealloc( dest );
    
    return 1;
}


static int decode_lua( lua_State *L )
{
    size_t len = 0;
    unsigned char *src = (unsigned char*)luaL_checklstring( L, 1, &len );
    size_t dlen = len / 2;
    char *dest = NULL;

    // check hex-encoded src length
    if( len % 2 ){
        errno = EINVAL;
    }
    else if( ( dest = pnalloc( dlen, char ) ) )
    {
        if( hex_decode( dest, src, len ) == 0 ){
            lua_pushlstring( L, dest, dlen );
            pdealloc( dest );
            return 1;
        }
        pdealloc( dest );
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_hex( lua_State *L )
{
    lua_createtable( L, 0, 2 );
    lstate_fn2tbl( L, "encode", encode_lua );
    lstate_fn2tbl( L, "decode", decode_lua );
    
    return 1;
}


