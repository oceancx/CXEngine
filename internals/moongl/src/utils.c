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

/*------------------------------------------------------------------------------*
 | Misc utilities                                                               |
 *------------------------------------------------------------------------------*/

int noprintf(const char *fmt, ...) 
    { (void)fmt; return 0; }

int notavailable(lua_State *L, ...) 
    { 
    GLuint major = VERSION_MAJOR(moongl_version);
    GLuint minor = VERSION_MINOR(moongl_version);
    return luaL_error(L, "function not available in this OpenGL version (%d.%d)", major, minor);
    }


GLboolean checkboolean(lua_State *L, int arg)
    {
    if(!lua_isboolean(L, arg))
        return (GLboolean)luaL_argerror(L, arg, "boolean expected");
    return lua_toboolean(L, arg) ? GL_TRUE : GL_FALSE;
    }

GLboolean optboolean(lua_State *L, int arg, GLboolean d)
    {
    if(!lua_isboolean(L, arg))
        return d;
    return lua_toboolean(L, arg) ? GL_TRUE : GL_FALSE;
    }


int checkoption_hint(lua_State *L, int arg, const char *def, const char *const lst[])
/* Variant of luaL_checkoption(), with an added hint in the error message */
    {
    const char *hint = NULL;
    const char *name = (def) ? luaL_optstring(L, arg, def) : luaL_checkstring(L, arg);
    int i;
    for (i=0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)  return i;

    if(lua_checkstack(L, i*2))
        {
        for(i=0; lst[i]; i++)
            {
            lua_pushfstring(L, "'%s'", lst[i]);
            lua_pushstring(L, "|");
            }
        i = i*2;
        if(i>0)
            {
            lua_pop(L, 1); /* the last separator */
            lua_concat(L, i-1);
            hint = lua_tostring(L, -1); 
            }
        }
    if(hint)
        return luaL_argerror(L, arg, lua_pushfstring(L, 
                    "invalid option '%s', valid options are: %s", name, hint));
    return luaL_argerror(L, arg, lua_pushfstring(L, "invalid option '%s'", name));
    }

int isoption(lua_State *L, int arg, const char *const lst[])
/* Variant of luaL_checkoption(), checks only and returns a boolean */
    {
    int i;
    const char *name;

    if(!lua_isstring(L, arg)) return 0;
    name = lua_tostring(L, arg);
    for (i=0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)  return 1;
    return 0;
    }

int checkcolor(lua_State *L, int arg, GLfloat dst[4])
    {
    int i;
    if(lua_type(L, arg) == LUA_TTABLE)
        {
        for(i=0; i<4; i++)
            {
            lua_rawgeti(L, arg, i+1);
            dst[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
            }
        }
    else if(lua_type(L, arg) == LUA_TNUMBER)
        {
        dst[0] = luaL_checknumber(L, arg);
        dst[1] = luaL_checknumber(L, arg+1);
        dst[2] = luaL_checknumber(L, arg+2);
        dst[3] = luaL_checknumber(L, arg+3);
        }
    else
        return luaL_error(L, "color expected");
    return 0;
    }

void *checklightuserdata(lua_State *L, int arg)
    {
    if(lua_type(L, arg) != LUA_TLIGHTUSERDATA)
        { luaL_argerror(L, arg, "expected lightuserdata"); return NULL; }
    return lua_touserdata(L, arg);
    }

GLuint checktargetorname(lua_State *L, int arg, GLenum *dst, uint32_t domain)
/* The element at the index arg on the Lua stack may be an enum (string), or an integer.
 * In the first case the enum code is stored in 'code' and 0 is returned.
 * In the second case the integer value is returned.
 */
    {
    if(lua_isstring(L, arg))
        {
        *dst = enums_check(L, domain, arg);
        return 0;
        }
    return (GLuint)luaL_checkinteger(L, arg);
    }

const char *checkdataptr(lua_State *L, int arg, size_t *len)
    {
    *len = 0;
    char *data = NULL;
    if(!lua_istable(L, arg)) return NULL;
    lua_rawgeti(L, arg, 1);
    data = checklightuserdata(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, arg, 2);
    *len = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return data;
    }

size_t sizeoftype(lua_State *L, int type)
    {
    switch(type)
        {
        case GL_NONE: return 0;
        case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
        case GL_BYTE:  return sizeof(GLubyte);
        case GL_UNSIGNED_SHORT:  return sizeof(GLushort);
        case GL_SHORT:  return sizeof(GLshort);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_INT:  return sizeof(GLint);
        case GL_FIXED: return sizeof(GLfixed);
        case GL_HALF_FLOAT:  return sizeof(GLhalf);
        case GL_FLOAT:  return sizeof(GLfloat);
        case GL_DOUBLE: return sizeof(GLdouble);
        case GL_UNSIGNED_BYTE_3_3_2:
        case GL_UNSIGNED_BYTE_2_3_3_REV:  return sizeof(GLubyte);
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV: return sizeof(GLshort);
        case GL_UNSIGNED_INT_8_8_8_8: 
        case GL_UNSIGNED_INT_8_8_8_8_REV: 
        case GL_UNSIGNED_INT_10_10_10_2: 
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_24_8:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_5_9_9_9_REV: return sizeof(GLuint);
        case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: return 8; /* 2*32 bit, see 8.4.4.2 */
        case GL_INT_2_10_10_10_REV: return sizeof(GLint);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }




