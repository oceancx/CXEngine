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

static GLint GetProgramStage_(lua_State *L, GLuint program, GLenum shadertype, GLenum pname)
    {
    GLint value;
    glGetProgramStageiv(program, shadertype, pname, &value);
    CheckError(L);
    return value;
    }

static int UniformBlockBinding(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint binding = luaL_checkinteger(L, 3);
    glUniformBlockBinding(program, index, binding);
    CheckError(L);
    return 0;
    }

static int ShaderStorageBlockBinding(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint binding = luaL_checkinteger(L, 3);
    glShaderStorageBlockBinding(program, index, binding);
    CheckError(L);
    return 0;
    }

static int UniformSubroutines(lua_State *L)
    {
    GLsizei count, arg, i;
    GLuint *indices;
    GLenum shadertype = checkshadertype(L, 1);
    arg = 2;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    if(arg == 2)
        return luaL_argerror(L, 2, "integer expected");
    count = arg - 2;
    indices = (GLuint*)Malloc(L, count*sizeof(GLint));
    arg = 2;
    for(i=0; i< count; i++)
        indices[i] = lua_tointeger(L, arg++);
    glUniformSubroutinesuiv(shadertype, count, indices);
    Free(L, indices);
    CheckError(L);
    return 0;
    }

static int GetInt(lua_State *L, GLuint program, GLenum shadertype, GLuint index, GLenum pname)
    {
    GLint value;
    glGetActiveSubroutineUniformiv(program, shadertype, index, pname, &value);
    CheckError(L);
    lua_pushinteger(L, value);
    return 1;
    }

static int GetCompatibleSubroutines(lua_State *L, GLuint program, GLenum shadertype, GLuint index)
    {
    GLint count, i;
    GLint *values;
    glGetActiveSubroutineUniformiv
        (program, shadertype, index, GL_NUM_COMPATIBLE_SUBROUTINES, &count);
    CheckError(L);
    values = (GLint*)Malloc(L, count*sizeof(GLint));
    glGetActiveSubroutineUniformiv
        (program, shadertype, index, GL_COMPATIBLE_SUBROUTINES, values);
    CheckErrorFree(L, values);
    for(i=0; i<count; i++)
        lua_pushinteger(L, values[i]);
    Free(L, values);
    return count;
    }

static int GetActiveSubroutineUniform(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    GLuint index = luaL_checkinteger(L, 3);
    GLenum pname = checksubroutinepname(L, 4);
    
    switch(pname)
        {
        case GL_NUM_COMPATIBLE_SUBROUTINES:
            return GetInt(L, program, shadertype, index, pname);
        case GL_COMPATIBLE_SUBROUTINES:
            return GetCompatibleSubroutines(L, program, shadertype, index);
        case GL_UNIFORM_SIZE:
            return GetInt(L, program, shadertype, index, pname);
        case GL_UNIFORM_NAME_LENGTH:
            return GetInt(L, program, shadertype, index, pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }

static int GetUniformSubroutine(lua_State *L)
    {
    GLuint value;
    GLenum shadertype = checkshadertype(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    glGetUniformSubroutineuiv(shadertype, location, &value);
    CheckError(L);
    lua_pushinteger(L, value);
    return 1;
    }

static int GetSubroutineUniformLocation(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    const GLchar *name = luaL_checkstring(L, 3);
    GLint loc = glGetSubroutineUniformLocation(program, shadertype, name);
    CheckError(L);
    lua_pushinteger(L, loc);
    return 1;
    }

static int GetSubroutineIndex(lua_State *L) 
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    const GLchar *name = luaL_checkstring(L, 3);
    GLuint index = glGetSubroutineIndex(program, shadertype, name);
    CheckError(L);
    lua_pushinteger(L, index);
    return 1;
    }


static int GetActiveSubroutineName(lua_State *L)
    {
    GLsizei length;
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    GLuint index = luaL_checkinteger(L, 3);
    GLsizei bufsz = GetProgramStage_(L, program, shadertype, 
                            GL_ACTIVE_SUBROUTINE_MAX_LENGTH);
    GLchar *name = (GLchar*)Malloc(L, bufsz*sizeof(GLchar));
    glGetActiveSubroutineName(program, shadertype, index, bufsz, &length, name);
    CheckErrorFree(L, name);
    lua_pushstring(L, name);    
    Free(L, name);  
    return 1;
    }

static int GetActiveSubroutineUniformName(lua_State *L)
    {
    GLsizei length;
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    GLuint index = luaL_checkinteger(L, 3);
    GLsizei bufsz = GetProgramStage_(L, program, shadertype, 
                            GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH);
    GLchar *name = (GLchar*)Malloc(L, bufsz*sizeof(GLchar));
    glGetActiveSubroutineUniformName(program, shadertype, index, bufsz, &length, name);
    CheckErrorFree(L, name);
    lua_pushstring(L, name);    
    Free(L, name);  
    return 1;
    }


static int GetStageInt(lua_State *L, GLuint program, GLenum shadertype, GLenum pname)
    {
    GLint value;
    glGetProgramStageiv(program, shadertype, pname, &value);
    CheckError(L);
    lua_pushinteger(L, value);
    return 1;
    }

static int GetProgramStage(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLenum shadertype = checkshadertype(L, 2);
    GLenum pname = checkstagepname(L, 3);
    switch(pname)
        {
        case GL_ACTIVE_SUBROUTINES: 
                return GetStageInt(L,program,shadertype,pname);
        case GL_ACTIVE_SUBROUTINE_UNIFORMS: 
                return GetStageInt(L,program,shadertype,pname);
        case GL_ACTIVE_SUBROUTINE_MAX_LENGTH: 
                return GetStageInt(L,program,shadertype,pname);
        case GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS: 
                return GetStageInt(L,program,shadertype,pname);
        case GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH: 
                return GetStageInt(L,program,shadertype,pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Memory barriers                                                              |
 *------------------------------------------------------------------------------*/

BITFIELD_STRINGS(BarrierBitStrings) = {
    "all", 
    "vertex attrib array", 
    "element array", 
    "uniform", 
    "texture fetch", 
    "shader image access", 
    "command", 
    "pixel buffer", 
    "texture update", 
    "buffer update", 
    "client mapped buffer", 
    "query buffer", 
    "framebuffer", 
    "transform feedback", 
    "atomic counter", 
    "shader storage",
    NULL
};
BITFIELD_CODES(BarrierBitCodes) = {
    GL_ALL_BARRIER_BITS,
    GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT, 
    GL_ELEMENT_ARRAY_BARRIER_BIT, 
    GL_UNIFORM_BARRIER_BIT, 
    GL_TEXTURE_FETCH_BARRIER_BIT, 
    GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, 
    GL_COMMAND_BARRIER_BIT, 
    GL_PIXEL_BUFFER_BARRIER_BIT, 
    GL_TEXTURE_UPDATE_BARRIER_BIT, 
    GL_BUFFER_UPDATE_BARRIER_BIT, 
    GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT, 
    GL_QUERY_BUFFER_BARRIER_BIT, 
    GL_FRAMEBUFFER_BARRIER_BIT, 
    GL_TRANSFORM_FEEDBACK_BARRIER_BIT, 
    GL_ATOMIC_COUNTER_BARRIER_BIT, 
    GL_SHADER_STORAGE_BARRIER_BIT,
};
BITFIELD_T(BarrierBitBitfield, BarrierBitStrings, BarrierBitCodes)
#define CheckBarrierBit(L, arg, mand) bitfieldCheck((L), (arg), (mand), &BarrierBitBitfield)
#define PushBarrierBit(L, code) bitfieldPush((L), (code), &BarrierBitBitfield)


static int MemoryBarrier(lua_State *L)
    {
    GLbitfield barriers = CheckBarrierBit(L, 1, 1);
    glMemoryBarrier(barriers);
    CheckError(L);
    return 0;
    }


static int MemoryBarrierByRegion(lua_State *L)
    {
    GLbitfield barriers = CheckBarrierBit(L, 1, 1);
    glMemoryBarrierByRegion(barriers);
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "uniform_block_binding", UniformBlockBinding },
        { "shader_storage_block_binding", ShaderStorageBlockBinding },
        { "uniform_subroutines", UniformSubroutines },
        { "get_active_subroutine_uniform", GetActiveSubroutineUniform },
        { "get_uniform_subroutine", GetUniformSubroutine },
        { "get_subroutine_uniform_location", GetSubroutineUniformLocation },
        { "get_subroutine_index", GetSubroutineIndex },
        { "get_active_subroutine_name", GetActiveSubroutineName },
        { "get_active_subroutine_uniform_name", GetActiveSubroutineUniformName },
        { "get_program_stage", GetProgramStage },
        { "memory_barrier", MemoryBarrier },
        { "memory_barrier_by_region", MemoryBarrierByRegion },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_subroutine(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

