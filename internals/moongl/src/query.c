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

GEN_FUNC(Querie, OTYPE_QUERY)
DELETE_FUNC(Querie, OTYPE_QUERY)
IS_FUNC(Query)

static int CreateQueries(lua_State *L)
    {
    GLsizei i;
    GLuint* names;
    GLenum target = checkquerytarget(L, 1);
    GLsizei n = luaL_optinteger(L, 2, 1);
    check_init_called(L);
    luaL_checkstack(L, n, NULL);
    names = (GLuint*)Malloc(L, n*sizeof(GLuint));
    glCreateQueries(target, n, names);
    CheckErrorFree(L, names);
    for(i = 0; i < n; i++)
        {
        object_new(L, OTYPE_QUERY, names[i]);
        lua_pushinteger(L, names[i]);
        }
    Free(L, names);
    return n;
    }

static int BeginQuery(lua_State *L)
    {
    GLenum index;
    GLenum target = checkquerytarget(L, 1);
    GLuint name = luaL_checkinteger(L, 2);
    if(lua_isnoneornil(L, 3))
        glBeginQuery(target, name);
    else
        {
        index = luaL_checkinteger(L, 3);
        glBeginQueryIndexed(target, index, name);
        }
    CheckError(L);
    return 0;
    }

static int EndQuery(lua_State *L)
    {
    GLenum index;
    GLenum target = checkquerytarget(L, 1);
    if(lua_isnoneornil(L, 2))
        glEndQuery(target);
    else
        {
        index = luaL_checkinteger(L, 2);
        glEndQueryIndexed(target, index);
        }
    CheckError(L);
    return 0;
    }

static int QueryCounter(lua_State *L)
    {
    GLuint name = luaL_checkinteger(L, 1);
    GLenum target = GL_TIMESTAMP; /* checkquerytarget(L, 2); only TIMESTAMP */
    glQueryCounter(name, target);
    CheckError(L);
    return 0;
    }

static int GetTimestamp(lua_State *L)
    {
    GLint64 data;
    glGetInteger64v(GL_TIMESTAMP, &data);
    CheckError(L);
    lua_pushinteger(L, data);
    return 1;
    }


static int GetInt(lua_State *L, GLenum target, GLenum pname, GLenum *index)
/* get_query(target, pname [, index]) */
    {
    GLint param;
    if(index!=NULL)
        glGetQueryIndexediv(target, *index, pname, &param);
    else
        glGetQueryiv(target, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }

static int GetQuery(lua_State *L)
    {
    int indexed = 0;
    GLenum index;
    GLenum target = checkquerytarget(L, 1);
    GLenum pname = checkquerypname(L, 2);
    if(!lua_isnoneornil(L, 3))
        {
        indexed = 1;
        index = luaL_checkinteger(L, 3);
        }
    switch(pname)
        {
        case GL_CURRENT_QUERY:
        case GL_QUERY_COUNTER_BITS:
            return GetInt(L, target, pname, indexed == 1 ? &index : NULL);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


//void glGetQueryObjectiv(GLuint name, GLenum pname, GLint *params); //@@
//void glGetQueryObjectuiv(GLuint name, GLenum pname, GLuint *params); //@@
//void glGetQueryObjecti64v(GLuint name, GLenum pname, GLint64 *params); //@@
//void glGetQueryObjectui64v(GLuint name, GLenum pname, GLuint64 *params); //@@
static int GetQueryObject(lua_State *L) //@@TODO
    {
    NOT_AVAILABLE;
    CheckError(L);
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "gen_queries", GenQueries },
        { "create_queries", CreateQueries },
        { "delete_queries", DeleteQueries },
        { "is_query",  IsQuery },
        { "begin_query", BeginQuery },
        { "end_query", EndQuery },
        { "query_counter", QueryCounter },
        { "get_timestamp", GetTimestamp },
        { "get_query", GetQuery },
        { "get_query_object", GetQueryObject },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_query(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

