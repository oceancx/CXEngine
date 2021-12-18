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
 | get_uniform()                                                                |
 *------------------------------------------------------------------------------*/

#if 1 /* #ifndef GL_VERSION_4_5 */
#define GET_UNIFORM_FUNC(T, nt, t, checkt)  \
static int GetUniform_##nt(lua_State *L, GLuint program, GLint location, GLsizei size)  \
    {                                                                                   \
    GLsizei i;                                                                          \
    T params[16];                                                                       \
    glGetUniform##t##v(program, location, params);                                      \
    CheckError(L);                                                                      \
    for(i=0; i<size; i++)                                                               \
        lua_push##checkt(L, params[i]);                                                 \
    return size;                                                                        \
    }
#else
#define GET_UNIFORM_FUNC(T, nt, t, checkt)  \
static int GetUniform_##nt(lua_State *L, GLuint program, GLint location, GLsizei size)  \
    {                                                                                   \
    GLsizei i;                                                                          \
    T params[16];                                                                       \
    glGetnUniform##t##v(program, location, size, params);                               \
    CheckError(L);                                                                      \
    for(i=0; i<size; i++)                                                               \
        lua_push##checkt(L, params[i]);                                                 \
    return size;                                                                        \
    }
#endif

GET_UNIFORM_FUNC(GLint, b, i, boolean)
GET_UNIFORM_FUNC(GLint, i, i, integer)
GET_UNIFORM_FUNC(GLuint, ui, ui, integer)
GET_UNIFORM_FUNC(GLfloat, f, f, number)
GET_UNIFORM_FUNC(GLdouble, d, d, number)

static int GetUniform(lua_State *L)
/* get_uniform(program, location, type, size) */
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    GLenum type = checkuniformtype(L, 3);
    GLsizei size = luaL_checkinteger(L, 4); 
    if((size < 1) || (size>16))
        return luaL_argerror(L, 4, "out of range");
    switch(type)
        {
        case NONGL_BOOLEAN: return GetUniform_b(L, program, location, size);
        case GL_INT: return GetUniform_i(L, program, location, size);
        case GL_UNSIGNED_INT: return GetUniform_ui(L, program, location, size);
        case GL_FLOAT:  return GetUniform_f(L, program, location, size);
        case GL_DOUBLE: return GetUniform_d(L, program, location, size);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }

    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | get_uniform_location() etc                                                   |
 *------------------------------------------------------------------------------*/

static int GetUniformLocation(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    const GLchar *name = luaL_checkstring(L, 2);
    GLint loc = glGetUniformLocation(program, name);
    CheckError(L);
    if(loc==-1)
        return luaL_error(L, "invalid name '%s'", name);
    lua_pushinteger(L, loc);
    return 1;
    }

static GLsizei ActiveUniformMaxLength(lua_State *L, GLuint program)
    {
    GLsizei len;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &len);
    CheckError(L);
    return len;
    }

static int GetActiveUniformName(lua_State *L)
    {
    GLsizei maxlen, length;
    GLchar *name;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);

    maxlen = ActiveUniformMaxLength(L, program);
    name = (GLchar*)Malloc(L, maxlen*sizeof(GLchar));
    glGetActiveUniformName(program, index, maxlen, &length, name);
    CheckErrorFree(L, name);
    if(length > 0) 
        lua_pushstring(L, name);
    Free(L, name);
    return (length > 0) ? 1 : luaL_error(L, "invalid index %d", index);
    }

static int GetUniformIndices(lua_State *L)
    {
    GLsizei i, count;
    const GLchar **names;
    GLuint *indices;
    GLuint program = luaL_checkinteger(L, 1);
    i = 2;
    while(!lua_isnoneornil(L, i)) luaL_checkstring(L, i++);
    if(i == 2)
        return luaL_argerror(L, 2, "string expected");
    count = i - 2;
    names = (const GLchar**)Malloc2(L, count*sizeof(GLchar*), (void**)&indices, count*sizeof(GLuint));
    for(i = 0; i < count; i++)
        names[i] = lua_tostring(L, i + 2);
    glGetUniformIndices(program, count, names, indices);
    CheckErrorFree2(L, names, indices);
    for(i = 0; i < count; i++)
        {
        if(indices[i] == GL_INVALID_INDEX)
            {
            lua_pushfstring(L, "invalid name '%s'", names[i]);
            Free(L, names);
            Free(L, indices);
            return lua_error(L);
            }
        }
    for(i = 0; i < count; i++)
        lua_pushinteger(L, indices[i]);
    Free(L, names);
    Free(L, indices);
    return count;
    }

/*------------------------------------------------------------------------------*
 | get_active uniforms()                                                        |
 *------------------------------------------------------------------------------*/


static int GetType(lua_State *L, GLuint program, GLsizei count, GLuint *indices, GLenum pname, GLint *params)
    {
    int i;
    glGetActiveUniformsiv(program, count, indices, pname, params);
    CheckErrorFree2(L, indices, params);
    for(i = 0; i<count; i++)
        pushglsltype(L, params[i]); /* this may raise an error, but it would be symptom of a bug */
    Free(L, indices);
    Free(L, params);
    return count;
    }


static int GetInt(lua_State *L, GLuint program, GLsizei count, GLuint *indices, GLenum pname, GLint *params)
    {
    int i;
    glGetActiveUniformsiv(program, count, indices, pname, params);
    CheckErrorFree2(L, indices, params);
    for(i = 0; i<count; i++)
        lua_pushinteger(L, params[i]);
    Free(L, indices);
    Free(L, params);
    return count;
    }

static int GetBoolean(lua_State *L, GLuint program, GLsizei count, GLuint *indices, GLenum pname, GLint *params)
    {
    int i;
    glGetActiveUniformsiv(program, count, indices, pname, params);
    CheckErrorFree2(L, indices, params);
    for(i = 0; i<count; i++)
        lua_pushboolean(L, params[i]);
    Free(L, indices);
    Free(L, params);
    return count;
    }


static int GetActiveUniforms(lua_State *L)
/* get_active_uniforms(program, pname, uniform1, uniform2, ...) */
    {
    int arg;
    GLsizei count, i;
    GLuint *indices;
    GLint *params;
    GLuint program = luaL_checkinteger(L, 1);
    GLenum pname = checkuniformpname(L, 2);
    arg = 3;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    if(arg==3)
        return luaL_argerror(L, 3, "integer expected");
    count = arg - 3;
    indices = (GLuint*)Malloc2(L, count*sizeof(GLuint), (void**)&params, count*sizeof(GLint));
    for(i=0; i<count; i++)
        indices[i] = lua_tointeger(L, i+3);
    switch(pname)
        {
        case GL_UNIFORM_TYPE: return GetType(L,program,count,indices,pname,params);
        case GL_UNIFORM_SIZE: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_NAME_LENGTH: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_BLOCK_INDEX: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_OFFSET: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_ARRAY_STRIDE: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_MATRIX_STRIDE: return GetInt(L,program,count,indices,pname,params);
        case GL_UNIFORM_IS_ROW_MAJOR: return GetBoolean(L,program,count,indices,pname,params);
        case GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX: 
                            return GetInt(L,program,count,indices,pname,params);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }

static int GetActiveUniform(lua_State *L)
/* name, type, size = get_active_uniform(program, index) */
    {
    GLsizei maxlen, length;
    GLchar *name;
    GLsizei size;
    GLenum type;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    maxlen = ActiveUniformMaxLength(L, program);
    name = (GLchar*)Malloc(L, maxlen*sizeof(GLchar));
    glGetActiveUniform(program, index, maxlen, &length, &size, &type, name);
    CheckErrorFree(L, name);
    if(length == 0)
        {
        Free(L, name);
        return luaL_error(L, "invalid index %d", index);
        }
    lua_pushstring(L, name);
    pushglsltype(L, type);
    lua_pushinteger(L, size);
    Free(L, name);
    return 3;
    }


/*------------------------------------------------------------------------------*
 | get_active uniform_block()                                                   |
 *------------------------------------------------------------------------------*/

static GLint GetAUB_NameLength(lua_State *L, GLuint program, GLuint index)
    {
    GLint param;
    glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &param);
    CheckError(L);
    return param;
    }


static int GetAUB_Int(lua_State *L, GLuint program, GLuint index, GLenum pname)
    {
    GLint param;
    glGetActiveUniformBlockiv(program, index, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }

static int GetAUB_Boolean(lua_State *L, GLuint program, GLuint index, GLenum pname)
    {
    GLint param;
    glGetActiveUniformBlockiv(program, index, pname, &param);
    CheckError(L);
    lua_pushboolean(L, param);
    return 1;
    }

static int GetActiveUniformBlock(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLenum pname = checkblockpname(L, 3);
    switch(pname)
        {
        case GL_UNIFORM_BLOCK_BINDING:
        case GL_UNIFORM_BLOCK_DATA_SIZE:
        case GL_UNIFORM_BLOCK_NAME_LENGTH:
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES:
                        return GetAUB_Int(L, program, index, pname);
        case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER:
                        return GetAUB_Boolean(L, program, index, pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


static int GetUniformBlockIndex(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    const char *name = luaL_checkstring(L, 2);
    GLuint index = glGetUniformBlockIndex(program, name);
    CheckError(L);
    lua_pushinteger(L, index);
    return 1;
    }

static int GetActiveUniformBlockName(lua_State *L)
    {
    GLsizei length;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLsizei bufsz = GetAUB_NameLength(L, program, index);
    GLchar *name = (GLchar*)Malloc(L, bufsz*sizeof(GLchar));
    glGetActiveUniformBlockName(program, index, bufsz, &length, name);
    CheckErrorFree(L, name);
    lua_pushstring(L, name);
    Free(L, name);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | get_atomic_counter_buffer()                                                  |
 *------------------------------------------------------------------------------*/

static int GetACB_Int(lua_State *L, GLuint program, GLuint index, GLenum pname)
    {
    GLint param;
    glGetActiveAtomicCounterBufferiv(program, index, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }

static int GetACB_Boolean(lua_State *L, GLuint program, GLuint index, GLenum pname)
    {
    GLint param;
    glGetActiveAtomicCounterBufferiv(program, index, pname, &param);
    CheckError(L);
    lua_pushboolean(L, param);
    return 1;
    }

static int GetActiveAtomicCounterBuffer(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLenum pname = checkacbpname(L, 3);
    switch(pname)
        {
        case GL_ATOMIC_COUNTER_BUFFER_BINDING:
        case GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE:
        case GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS:
        case GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES:
                            return GetACB_Int(L, program, index, pname);
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER:
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER:
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER:
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER:
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER:
        case GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER:
                            return GetACB_Boolean(L, program, index, pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "get_uniform", GetUniform },
        { "get_uniform_location", GetUniformLocation },
        { "get_active_uniform_name", GetActiveUniformName },
        { "get_uniform_indices", GetUniformIndices },
        { "get_active_uniform", GetActiveUniform },
        { "get_active_uniforms", GetActiveUniforms },
        { "get_uniform_block_index", GetUniformBlockIndex },
        { "get_active_uniform_block_name", GetActiveUniformBlockName },
        { "get_active_uniform_block", GetActiveUniformBlock },
        { "get_active_atomic_counter_buffer", GetActiveAtomicCounterBuffer },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_getuniform(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

