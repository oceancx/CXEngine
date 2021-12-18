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

static int CreateShader(lua_State *L)
    {
    GLenum type = checkshadertype(L, 1);
    GLuint ref;
    check_init_called(L);
    ref = glCreateShader(type);
    CheckError(L);
    object_new(L, OTYPE_SHADER, ref);
    lua_pushinteger(L, ref);
    return 1;
    }

static int ShaderSource(lua_State *L)
/* shader_source(shader, string) */
    {
    size_t len;
    GLint length;
    GLuint shader = luaL_checkinteger(L, 1);
    const GLchar *string = luaL_checklstring(L, 2, &len);
    length = (GLint)len;
    glShaderSource(shader, 1, &string, &length);
    CheckError(L);
    return 0;
    }

static int CompileShader(lua_State *L)
    {
    char *buf;
    GLint ok, bufsz;
    GLuint name = luaL_checkinteger(L, 1);
    int check = lua_toboolean(L, 2);

    glCompileShader(name);
    CheckError(L);

    if(check)
        {
        glGetShaderiv(name, GL_COMPILE_STATUS, &ok);
        if(!ok)
            {
            glGetShaderiv(name, GL_INFO_LOG_LENGTH, &bufsz);
            CheckError(L);
            buf = (char*)Malloc(L, bufsz*sizeof(char));
            glGetShaderInfoLog(name, bufsz, NULL, buf);
            CheckErrorFree(L, buf);
            lua_pushfstring(L, "shader compile error: %s",buf);
            Free(L, buf);
            return lua_error(L);
            }
        }   
    return 0;
    }

VOID_FUNC(ReleaseShaderCompiler)
DELETE_FUNC(Shader, OTYPE_SHADER)
IS_FUNC(Shader)

static int ShaderBinary(lua_State *L)
    {
#define max_shaders 16  /* at most one per shader type */
    int count, i;
    GLuint shaders[max_shaders];
    GLenum binaryformat;
    const char *binary;
    size_t length;
    if(lua_type(L, 1) != LUA_TTABLE)
        return luaL_argerror(L, 1, "table expected");
    count = luaL_len(L, 1);
    if(count > max_shaders)
        return luaL_argerror(L, 1, "too many shaders");
    for(i=0; i <count; i++)
        {
        lua_geti(L, 1, i+1);
        shaders[i] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        }
    binaryformat = checkshaderbinaryformat(L, 2);
    binary = luaL_checklstring(L, 3, &length);
    glShaderBinary((GLsizei)count, shaders, binaryformat, binary, (GLsizei)length);
    CheckError(L);
    return 0;
#undef max_shaders
    }

static int SpecializeShader(lua_State *L) //GL_VERSION_4_6
    {
#define max_constants 256
    GLuint i, n = 0;
    GLuint *indices = NULL;
    GLuint *values = NULL;
    GLuint index[max_constants];
    GLuint value[max_constants];
    GLuint name = luaL_checkinteger(L, 1);
    const char *entrypoint = luaL_checkstring(L, 2);
    if(!lua_isnoneornil(L, 3))
        {
        if(lua_type(L, 3) != LUA_TTABLE)
            return luaL_argerror(L, 1, "table expected");
        if(lua_type(L, 4) != LUA_TTABLE)
            return luaL_argerror(L, 1, "table expected");
        n = luaL_len(L, 3);
        if(n > max_constants)
            return luaL_argerror(L, 1, "too many constants");
        if(n != luaL_len(L, 4))
            return luaL_argerror(L, 1, "mismatch between number of indices and of values");
        if(n>0)
            {
            for(i=0; i <n; i++)
                {
                lua_geti(L, 3, i+1);
                index[i] = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
                lua_geti(L, 4, i+1);
                value[i] = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
                }
            indices = index;
            values = value;
            }
        }
    glSpecializeShader(name, (GLchar*)entrypoint, n, indices, values);
    CheckError(L);
    return 0;
#undef max_constants
    }

static int GetShaderInfoLog(lua_State *L)
    {
    char *buf;
    GLint bufsz;
    GLuint name = luaL_checkinteger(L, 1);

    glGetShaderiv(name, GL_INFO_LOG_LENGTH, &bufsz);
    CheckError(L);
    if(bufsz==0) return 0;
    buf = (char*)Malloc(L, bufsz*sizeof(char));
    glGetShaderInfoLog(name, bufsz, NULL, buf);
    CheckErrorFree(L, buf);
    lua_pushstring(L, buf);
    Free(L, buf);
    return 1;
    }

static int GetShaderSource(lua_State *L)
    {
    char *buf;
    GLint bufsz;
    GLuint name = luaL_checkinteger(L, 1);

    glGetShaderiv(name, GL_SHADER_SOURCE_LENGTH, &bufsz);
    CheckError(L);
    if(bufsz==0) return 0;
    buf = (char*)Malloc(L, bufsz*sizeof(char));
    glGetShaderSource(name, bufsz, NULL, buf);
    CheckErrorFree(L, buf);
    lua_pushstring(L, buf);
    Free(L, buf);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | get_shader()                                                                 |
 *------------------------------------------------------------------------------*/

static int GetBoolean(lua_State *L, GLuint shader, GLenum pname)
    {
    GLint params; 
    glGetShaderiv(shader, pname, &params);
    CheckError(L);
    lua_pushboolean(L, params == GL_TRUE); 
    return 1;
    }

static int GetInteger(lua_State *L, GLuint shader, GLenum pname)
    {
    GLint params; 
    glGetShaderiv(shader, pname, &params);
    CheckError(L);
    lua_pushinteger(L, params); 
    return 1;
    }

static int GetEnum(lua_State *L, GLuint shader, GLenum pname, uint32_t domain)
    {
    GLint params;
    glGetShaderiv(shader, pname, &params);
    CheckError(L);
    return enums_push(L, domain, params);
    }

static int GetShader(lua_State *L)
    {
#define BOOLEAN return GetBoolean(L, shader, pname);
#define INTEGER return GetInteger(L, shader, pname);
    GLuint shader = luaL_checkinteger(L, 1);
    GLenum pname = checkshaderpname(L, 2);
    switch(pname)
        {
        case GL_SHADER_TYPE: return GetEnum(L, shader, pname, DOMAIN_SHADER_TYPE);
        case GL_DELETE_STATUS: BOOLEAN
        case GL_COMPILE_STATUS: BOOLEAN
        case GL_INFO_LOG_LENGTH: INTEGER
        case GL_SHADER_SOURCE_LENGTH: INTEGER
        case GL_SPIR_V_BINARY: BOOLEAN //GL_VERSION_4_6
        default: return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef BOOLEAN
#undef INTEGER
    }

static int GetShaderPrecisionFormat(lua_State *L)
/* get_shader_precision_format(shadertype, precisiontype)
 * -> range0, range1, precision
 */
    {
    GLint range[2];
    GLint precision;
    GLenum type = checkshadertype(L, 1);
    GLenum precisiontype = checkprecisionformat(L, 2);
    glGetShaderPrecisionFormat(type, precisiontype, range, &precision);
    CheckError(L);
    lua_pushinteger(L, range[0]);
    lua_pushinteger(L, range[1]);
    lua_pushinteger(L, precision);
    return 3;
    }

/*------------------------------------------------------------------------------*
 | Fragment Shaders                                                             |
 *------------------------------------------------------------------------------*/

static int BindFragDataLocation(lua_State *L)
    {
    int indexed = 0;
    GLuint index;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint color = luaL_checkinteger(L, 2);
    const GLchar *name = luaL_checkstring(L, 3);
    if(lua_isinteger(L, 4))
        {
        indexed = 1;
        index = luaL_checkinteger(L, 4);
        }
    if(indexed)
        glBindFragDataLocationIndexed(program, color, index, name);
    else
        glBindFragDataLocation(program, color, name);
    CheckError(L);
    return 0;
    }

static int GetFragDataLocation(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    const GLchar *name = luaL_checkstring(L, 2);
    GLint loc = glGetFragDataLocation(program, name);
    CheckError(L);
    lua_pushinteger(L, loc);
    return 1;
    }

static int GetFragDataIndex(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    const GLchar *name = luaL_checkstring(L, 2);
    GLint index = glGetFragDataIndex(program, name);
    CheckError(L);
    lua_pushinteger(L, index);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Compute Shaders                                                              |
 *------------------------------------------------------------------------------*/

UINT3_FUNC(DispatchCompute)

static int DispatchComputeIndirect(lua_State *L)
    {
    GLintptr indirect= luaL_checkinteger(L, 1);
    glDispatchComputeIndirect(indirect);
    CheckError(L);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "create_shader", CreateShader },
        { "shader_source", ShaderSource },
        { "compile_shader", CompileShader },
        { "release_shader_compiler", ReleaseShaderCompiler },
        { "delete_shader", DeleteShaders },
        { "delete_shaders", DeleteShaders },
        { "is_shader", IsShader  },
        { "shader_binary", ShaderBinary },
        { "specialize_shader", SpecializeShader },
        { "get_shader_info_log", GetShaderInfoLog },
        { "get_shader_source", GetShaderSource },
        { "get_shader", GetShader },
        { "get_shader_precision_format", GetShaderPrecisionFormat },
        { "bind_frag_data_location", BindFragDataLocation },
        { "get_frag_data_location", GetFragDataLocation },
        { "get_frag_data_index", GetFragDataIndex },
        { "dispatch_compute", DispatchCompute },
        { "dispatch_compute_indirect", DispatchComputeIndirect },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_shader(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


