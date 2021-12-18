/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Stefano Trettel
 *
 * Software repository: MoonGL, https://github.com/stetre/moongl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"


static int Enable(lua_State *L)
    {
    GLenum cap = checkcapability(L, 1);
    int indexed = lua_isinteger(L, 2);
    if(indexed)
        glEnablei(cap, lua_tointeger(L, 2));
    else
        glEnable(cap);
    CheckError(L);
    return 0;
    }

static int Disable(lua_State *L)
    {
    GLenum cap = checkcapability(L, 1);
    int indexed = lua_isinteger(L, 2);
    if(indexed)
        glDisablei(cap, lua_tointeger(L, 2));
    else
        glDisable(cap);
    CheckError(L);
    return 0;
    }

static int IsEnabled(lua_State *L)
    {
    GLboolean res;
    GLenum cap = checkcapability(L, 1);
    int indexed = lua_isinteger(L, 2);
    if(indexed)
        res = glIsEnabledi(cap, lua_tointeger(L, 2));
    else
        res = glIsEnabled(cap);
    CheckError(L);
    lua_pushboolean(L, res);
    return 1;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "enable", Enable },
        { "disable", Disable },
        { "is_enabled", IsEnabled },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_capabilities(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

