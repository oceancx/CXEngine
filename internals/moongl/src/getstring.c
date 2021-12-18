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

static int GetString(lua_State *L)
    {
    GLsizei index;
    GLsizei num;
    const GLubyte *s;
    GLenum name = checkstringname(L, 1);
    switch(name)
        {
        case GL_RENDERER:
        case GL_VENDOR:
        case GL_VERSION:    
                    s = glGetString(name); break;
        case GL_SHADING_LANGUAGE_VERSION:
                    if(lua_isinteger(L, 2))
                        {
                        index = lua_tointeger(L, 2);
                        num = getSizei(L, GL_NUM_SHADING_LANGUAGE_VERSIONS);
                        if(index >= num)
                            return luaL_argerror(L, 2, "out of range");
                        s = glGetStringi(name, index);
                        }
                    else    
                        s = glGetString(name); 
                    break;
        case GL_EXTENSIONS:
                    index = luaL_checkinteger(L, 2);
                    num = getSizei(L, GL_NUM_EXTENSIONS);
                    if(index >= num)
                        return luaL_argerror(L, 2, "out of range");
                    s = glGetStringi(name, index);
                    break;
        case GL_SPIR_V_EXTENSIONS: //GL_VERSION_4_6
                    index = luaL_checkinteger(L, 2);
                    num = getSizei(L, GL_NUM_SPIR_V_EXTENSIONS);
                    if(index >= num)
                        return luaL_argerror(L, 2, "out of range");
                    s = glGetStringi(name, index);
                    break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    if(s == NULL)
        return luaL_error(L, UNEXPECTED_ERROR);
    lua_pushstring(L, (char*)s);
    return 1;
    }


static int GetExtensions(lua_State *L)
    {
    GLsizei i, count;
    const GLubyte *s;
    GLboolean byname = optboolean(L, 1, 0);

    count = getSizei(L, GL_NUM_EXTENSIONS);
    CheckError(L);

    lua_newtable(L);

    for(i=0; i<count; i++)
        {
        s = glGetStringi(GL_EXTENSIONS, i);
        CheckError(L);
        if(s == NULL)
            return luaL_error(L, UNEXPECTED_ERROR);
        if(byname)
            {
            lua_pushboolean(L, 1);
            lua_setfield(L, -2, (char*)s);
            }
        else
            {
            lua_pushstring(L, (char*)s);
            lua_rawseti(L, -2, i+1);
            }
        }

    return 1;
    }

static int GetSpirVExtensions(lua_State *L)
    {
    GLsizei i, count;
    const GLubyte *s;
    GLboolean byname = optboolean(L, 1, 0);

    count = getSizei(L, GL_NUM_SPIR_V_EXTENSIONS);
    CheckError(L);

    lua_newtable(L);

    for(i=0; i<count; i++)
        {
        s = glGetStringi(GL_SPIR_V_EXTENSIONS, i);
        CheckError(L);
        if(s == NULL)
            return luaL_error(L, UNEXPECTED_ERROR);
        if(byname)
            {
            lua_pushboolean(L, 1);
            lua_setfield(L, -2, (char*)s);
            }
        else
            {
            lua_pushstring(L, (char*)s);
            lua_rawseti(L, -2, i+1);
            }
        }

    return 1;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "get_string", GetString },
        { "get_extensions", GetExtensions },
        { "get_spir_v_extensions", GetSpirVExtensions }, //GL_VERSION_4_6
        { NULL, NULL } /* sentinel */
    };

void moongl_open_getstring(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

