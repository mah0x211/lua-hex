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


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define pnalloc(n,t)    (t*)malloc( (n) * sizeof(t) )
#define pdealloc(p)     free((void*)p)


static int encode_lua( lua_State *L )
{
    static const char dec2hex[16] = "0123456789abcdef";
    size_t len = 0;
    const char *src = luaL_checklstring( L, 1, &len );
    // dest length must be greater than len*2 + 1(null-term)
    size_t dlen = len * 2;
    const char *dest = NULL;
    unsigned char *ptr = NULL;
    size_t i = 0;
    
    // integer overflow or no-mem error
    if( dlen < len || !( dest = pnalloc( dlen + 1, const char ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( ENOMEM ) );
        return 2;
    }
    
    ptr = (unsigned char*)dest;
    for(; i < len; i++ ){
        *ptr++ = dec2hex[src[i] >> 4];
        *ptr++ = dec2hex[src[i] & 0xf];
    }
    *ptr = 0;
    lua_pushlstring( L, dest, dlen );
    pdealloc( dest );
    
    return 1;
}


static int decode_lua( lua_State *L )
{
    static const char hex2dec[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    //  0  1  2  3  4  5  6  7  8  9
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 
    //  A   B   C   D   E   F
        10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    //  a   b   c   d   e   f
        10, 11, 12, 13, 14, 15,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1
    };
    size_t len = 0;
    unsigned char *src = (unsigned char*)luaL_checklstring( L, 1, &len );
    size_t dlen = 0;
    char *dest = NULL;
    char *ptr = NULL;
	size_t i = 0;
    
    // src length must be multiple of two
    if( len % 2 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    
    // dest length must be greater than len/2 + 1(null-term)
    dlen = len / 2;
    if( !( dest = pnalloc( dlen + 1, char ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    
    ptr = dest;
    for(; i < len; i += 2 )
    {
        if( hex2dec[src[i]] == -1 || hex2dec[src[i+1]] == -1 ){
            errno = EINVAL;
            return -1;
        }
        *ptr++ = hex2dec[src[i]] << 4 | hex2dec[src[i+1]];
	}
	*ptr = 0;
    lua_pushlstring( L, dest, dlen );
    pdealloc( dest );
    
    return 1;
}


LUALIB_API int luaopen_hex( lua_State *L )
{
    lua_createtable( L, 0, 2 );
    lstate_fn2tbl( L, "encode", encode_lua );
    lstate_fn2tbl( L, "decode", decode_lua );
    
    return 1;
}


