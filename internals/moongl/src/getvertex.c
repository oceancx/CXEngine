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
 | GetVertexArray                                                               |
 *------------------------------------------------------------------------------*/

#if 0
//void glGetVertexArrayiv(GLuint array, GLenum pname, GLint *param);
//void glGetVertexArrayIndexediv(GLuint array, GLuint index, GLenum pname, GLint *param);
//void glGetVertexArrayIndexed64iv(GLuint array, GLuint index, GLenum pname, GLint64 *param);
#endif

static int ArrayGetNonIndexed(lua_State *L, GLuint array, GLenum pname)
    {
    GLint param;
    glGetVertexArrayiv(array, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }

static int ArrayGetInt_(lua_State *L, GLuint array, GLenum pname, int boolean)
#define ArrayGetInt(L, array, pname) ArrayGetInt_((L), (array), (pname), 0)
#define ArrayGetBoolean(L, array, pname) ArrayGetInt_((L), (array), (pname), 1)
    {
    GLint param;
    GLuint index = luaL_checkinteger(L, 3);
    glGetVertexArrayIndexediv(array, index, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param);
    else
        lua_pushinteger(L, param);
    return 1;
    }

static int ArrayGetInt64(lua_State *L, GLuint array, GLenum pname)
    {
    GLint64 param;
    GLuint index = luaL_checkinteger(L, 3);
    glGetVertexArrayIndexed64iv(array, index, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }


static int ArrayGetEnum(lua_State *L, GLuint array, GLenum pname)
    {
    GLint param;
    GLuint index = luaL_checkinteger(L, 3);
    glGetVertexArrayIndexediv(array, index, pname, &param);
    CheckError(L);
    return pushtype(L, param);
    }


static int GetVertexArray(lua_State *L)
/* get_vertex_array(array, pname [, index]) */
    {
    GLuint array = luaL_checkinteger(L, 1);
    GLenum pname = checkvertexpname(L, 2);

    switch(pname)
        {
        case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
        case GL_VERTEX_ATTRIB_ARRAY_LONG:
                return ArrayGetBoolean(L, array, pname);
        case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
        case GL_VERTEX_ATTRIB_ARRAY_DIVISOR:
        case GL_VERTEX_ATTRIB_RELATIVE_OFFSET:
                return ArrayGetInt(L, array, pname);
        case GL_VERTEX_ATTRIB_ARRAY_TYPE:
                return ArrayGetEnum(L, array, pname);
        case GL_VERTEX_BINDING_OFFSET:
                return ArrayGetInt64(L, array, pname);
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
                return ArrayGetNonIndexed(L, array, pname);
        default:
            return luaL_argerror(L, 3, "invalid pname");
        }

    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | GetVertexAttrib                                                              |
 *------------------------------------------------------------------------------*/

static int GetInt_(lua_State *L, GLuint index, GLenum pname, int boolean)
#define GetInt(L, index, pname) GetInt_((L), (index), (pname), 0)
#define GetBoolean(L, index, pname) GetInt_((L), (index), (pname), 1)
    {
    GLint param;
    glGetVertexAttribiv(index, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param);
    else
        lua_pushinteger(L, param);
    return 1;
    }

static int GetEnum(lua_State *L, GLuint index, GLenum pname)
    {
    GLint param;
    glGetVertexAttribiv(index, pname, &param);
    CheckError(L);
    return pushtype(L, param);
    }


static int GetVertexAttrib(lua_State *L)
    {
    GLuint index = luaL_checkinteger(L, 1);
    GLenum pname = checkvertexpname(L,2);
    switch(pname)
        {
        case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
        case GL_VERTEX_ATTRIB_ARRAY_LONG:
                        return GetBoolean(L, index, pname);
        case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
        case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
        case GL_VERTEX_ATTRIB_ARRAY_DIVISOR:
        case GL_VERTEX_ATTRIB_RELATIVE_OFFSET:
        case GL_VERTEX_ATTRIB_BINDING:
                        return GetInt(L, index, pname);
        case GL_VERTEX_ATTRIB_ARRAY_TYPE:
                        return GetEnum(L, index, pname);
//      case GL_VERTEX_BINDING_OFFSET: //@@
        default:
            return luaL_argerror(L, 2, "invalid pname");
        }
    CheckError(L);
    return 0;
    }


#define pname GL_CURRENT_VERTEX_ATTRIB

static int CurGetInt(lua_State *L, GLuint index)
    {
    int i;
    GLint params[4];
    glGetVertexAttribIiv(index, pname, params);
    CheckError(L);
    for(i=0 ; i<4; i++)
        lua_pushinteger(L, params[i]);
    return 4;
    }

static int CurGetUint(lua_State *L, GLuint index)
    {
    int i;
    GLuint params[4];
    glGetVertexAttribIuiv(index, pname, params);
    CheckError(L);
    for(i=0 ; i<4; i++)
        lua_pushinteger(L, params[i]);
    return 4;
    }

static int CurGetFloat(lua_State *L, GLuint index)
    {
    int i;
    GLfloat params[4];
    glGetVertexAttribfv(index, pname, params);
    CheckError(L);
    for(i=0 ; i<4; i++)
        lua_pushnumber(L, params[i]);
    return 4;
    }

static int CurGetDouble(lua_State *L, GLuint index)
    {
    int i;
    GLdouble params[4];
    glGetVertexAttribLdv(index, pname, params);
    CheckError(L);
    for(i=0 ; i<4; i++)
        lua_pushnumber(L, params[i]);
    return 4;
    }


static int GetCurrentVertexAttrib(lua_State *L)
/* val1, val2, val3, val4 = get_current_vertex_attrib(L, type) */
    {
    GLuint index = luaL_checkinteger(L, 1);
    GLenum type = checktype(L, 2);
    switch(type)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_FLOAT: return CurGetFloat(L, index);
        case GL_DOUBLE: return CurGetDouble(L, index);
        case GL_INT: return CurGetInt(L, index);
        case GL_UNSIGNED_INT: return CurGetUint(L, index);
        default:
            return luaL_error(L, "type not supported by this operation");
        }
    return 0;
    }
#undef pname


#define pname GL_VERTEX_ATTRIB_ARRAY_POINTER
static int GetVertexAttribPointer(lua_State *L)
    {
    GLuint index = luaL_checkinteger(L, 1);
    void *pointer;
    glGetVertexAttribPointerv(index, pname, &pointer);
    CheckError(L);
    lua_pushinteger(L, (GLsizeiptr)pointer);
    return 1;
    }
#undef pname

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/


static const struct luaL_Reg Functions[] = 
    {
        { "get_vertex_array", GetVertexArray },
        { "get_vertex_attrib", GetVertexAttrib },
        { "get_current_vertex_attrib", GetCurrentVertexAttrib },
        { "get_vertex_attrib_pointer", GetVertexAttribPointer },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_getvertex(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


