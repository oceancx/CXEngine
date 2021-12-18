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


#define VERTEX_ATTRIB_FUNC(T, t, checkt)                                \
static int VertexAttrib_##T(lua_State *L, GLuint index, int n, int arg) \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    while(i < n)                                                        \
        values[i++] = luaL_check##checkt(L, arg++);                     \
    switch(n)                                                           \
        {                                                               \
        case 1: glVertexAttrib1##t##v(index, values); break;            \
        case 2: glVertexAttrib2##t##v(index, values); break;            \
        case 3: glVertexAttrib3##t##v(index, values); break;            \
        case 4: glVertexAttrib4##t##v(index, values); break;            \
        default:                                                        \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

#define VERTEX_ATTRIB_FUNC_4ONLY(T, t, checkt)  \
static int VertexAttrib_##T(lua_State *L, GLuint index, int n, int arg) \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    while(i < n)                                                        \
        values[i++] = luaL_check##checkt(L, arg++);                     \
    switch(n)                                                           \
        {                                                               \
        case 4: glVertexAttrib4##t##v(index, values); break;            \
        default:                                                        \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

#define VERTEX_ATTRIB_FUNC_I(T, t, checkt)  \
static int VertexAttrib_##T(lua_State *L, GLuint index, int n, int arg) \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    while(i < n)                                                        \
        values[i++] = luaL_check##checkt(L, arg++);                     \
    switch(n)                                                           \
        {                                                               \
        case 1: glVertexAttribI1##t##v(index, values); break;           \
        case 2: glVertexAttribI2##t##v(index, values); break;           \
        case 3: glVertexAttribI3##t##v(index, values); break;           \
        case 4: glVertexAttribI4##t##v(index, values); break;           \
        default:                                                        \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

#define VERTEX_ATTRIB_FUNC_L(T, t, checkt)  \
static int VertexAttrib_##T(lua_State *L, GLuint index, int n, int arg) \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    while(i < n)                                                        \
        values[i++] = luaL_check##checkt(L, arg++);                     \
    switch(n)                                                           \
        {                                                               \
        case 1: glVertexAttribL1##t##v(index, values); break;           \
        case 2: glVertexAttribL2##t##v(index, values); break;           \
        case 3: glVertexAttribL3##t##v(index, values); break;           \
        case 4: glVertexAttribL4##t##v(index, values); break;           \
        default:                                                        \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

VERTEX_ATTRIB_FUNC_4ONLY(GLbyte, b, integer)
VERTEX_ATTRIB_FUNC_4ONLY(GLubyte, ub, integer)
VERTEX_ATTRIB_FUNC(GLshort, s, integer)
VERTEX_ATTRIB_FUNC_4ONLY(GLushort, us, integer)
VERTEX_ATTRIB_FUNC_I(GLint, i, integer)
VERTEX_ATTRIB_FUNC_I(GLuint, ui, integer)
VERTEX_ATTRIB_FUNC(GLfloat, f, number)
VERTEX_ATTRIB_FUNC_L(GLdouble, d, number)

static int VertexAttrib(lua_State *L)
/* gl.vertex_attrib(index, type, val1 [, val2, val3, val4]) */
    {
    int n;
    GLuint index = luaL_checkinteger(L, 1);
    GLenum type = checktype(L, 2);
    luaL_checknumber(L, 3); n=1;
    if(lua_isnumber(L, 4)) n++;
    if(lua_isnumber(L, 5)) n++;
    if(lua_isnumber(L, 6)) n++;

    switch(type)
        {
#define arg_first 3
        case GL_BYTE: return VertexAttrib_GLbyte(L, index, n, arg_first);
        case GL_UNSIGNED_BYTE: return VertexAttrib_GLubyte(L, index, n, arg_first);
        case GL_SHORT: return VertexAttrib_GLshort(L, index, n, arg_first);
        case GL_UNSIGNED_SHORT: return VertexAttrib_GLushort(L, index, n, arg_first);
        case GL_FLOAT: return VertexAttrib_GLfloat(L, index, n, arg_first);
        case GL_DOUBLE: return VertexAttrib_GLdouble(L, index, n, arg_first);
        case GL_INT: return VertexAttrib_GLint(L, index, n, arg_first);
        case GL_UNSIGNED_INT: return VertexAttrib_GLuint(L, index, n, arg_first);
        default:
            return luaL_error(L, "type not supported by this operation");
#undef arg_first
        }
    return 0;
    }

#define VERTEX_ATTRIB_4N_FUNC(T, t)                                     \
static int VertexAttribN_##T(lua_State *L, GLuint index, int arg)   \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    while(i < 4)                                                        \
        values[i++] = luaL_checkinteger(L, arg++);                      \
    glVertexAttrib4N##t##v(index, values);                              \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

VERTEX_ATTRIB_4N_FUNC(GLbyte, b)
VERTEX_ATTRIB_4N_FUNC(GLubyte, ub)
VERTEX_ATTRIB_4N_FUNC(GLshort, s)
VERTEX_ATTRIB_4N_FUNC(GLushort, us)
VERTEX_ATTRIB_4N_FUNC(GLint, i)
VERTEX_ATTRIB_4N_FUNC(GLuint, ui)

static int VertexAttribN(lua_State *L)
/* gl.vertex_attrib_n(index, type, val1, val2, val3, val4) */
    {
    GLuint index = luaL_checkinteger(L, 1);
    GLenum type = checktype(L, 2);
    switch(type)
        {
#define arg_first 3
        case GL_BYTE: return VertexAttribN_GLbyte(L, index, arg_first);
        case GL_UNSIGNED_BYTE: return VertexAttribN_GLubyte(L, index, arg_first);
        case GL_SHORT: return VertexAttribN_GLshort(L, index, arg_first);
        case GL_UNSIGNED_SHORT: return VertexAttribN_GLushort(L, index, arg_first);
        case GL_INT: return VertexAttribN_GLint(L, index, arg_first);
        case GL_UNSIGNED_INT: return VertexAttribN_GLuint(L, index, arg_first);
        case GL_FLOAT:
        case GL_DOUBLE:
        case GL_HALF_FLOAT:
        case GL_FIXED:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
            return luaL_error(L, "type not supported by this operation");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
#undef arg_first
        }
    return 0;
    }

static int VertexAttribP(lua_State *L)
/* gl.vertex_attrib_p(index, type, normalized, val1 , val2, val3, val4) */
    {
    int i, n, arg;
    GLuint values[4];
    GLuint index = luaL_checkinteger(L, 1);
    GLenum type = checktype(L, 2);
    GLboolean normalized = checkboolean(L, 3);
    luaL_checkinteger(L, 4); n=1;
    if(lua_isinteger(L, 5)) n++;
    if(lua_isinteger(L, 6)) n++;
    if(lua_isinteger(L, 7)) n++;
    switch(type)
        {
        case GL_INT_2_10_10_10_REV: break;
        case GL_UNSIGNED_INT_2_10_10_10_REV: break;
        case GL_UNSIGNED_INT_10F_11F_11F_REV: break;
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
        case GL_DOUBLE:
        case GL_HALF_FLOAT:
        case GL_FIXED:
            return luaL_error(L, "type not supported by this operation");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    i = 0; arg = 4;
    while(i < n)
        values[i++] = lua_tointeger(L, arg++);
    switch(n)
        {
        case 1: glVertexAttribP1uiv(index, type, normalized, values); break;
        case 2: glVertexAttribP2uiv(index, type, normalized, values); break;
        case 3: glVertexAttribP3uiv(index, type, normalized, values); break;
        case 4: glVertexAttribP4uiv(index, type, normalized, values); break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

static int VertexAttribPointer(lua_State *L)
/* vertex_attrib_pointer(index, size, type, normalized, stride, pointer) */
    {
    GLuint index = luaL_checkinteger(L, 1);
    GLint size = luaL_checkinteger(L, 2); /* no. of components */
    GLenum type = checktype(L, 3);
    GLboolean normalized = checkboolean(L, 4);
    GLsizei stride = luaL_checkinteger(L, 5);
    intptr_t pointer = luaL_checkinteger(L, 6);

    if(size == 0)
        size = GL_BGRA;

    switch(type)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
        case GL_HALF_FLOAT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_FIXED:
                glVertexAttribPointer(index, size, type, normalized, stride, (void*)pointer);
                break;
        case GL_DOUBLE:
                glVertexAttribLPointer(index, size, GL_DOUBLE, stride, (void*)pointer);
                break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }


static int VertexAttribPointerI(lua_State *L)
/* vertex_attrib_i_pointer(index, size, type, stride, pointer) */
    {
    GLuint index = luaL_checkinteger(L, 1);
    GLint size = luaL_checkinteger(L, 2); /* no. of components */
    GLenum type = checktype(L, 3);
    GLsizei stride = luaL_checkinteger(L, 4);
    intptr_t pointer = luaL_checkinteger(L, 5);

    switch(type)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
                glVertexAttribIPointer(index, size, type, stride, (void*)pointer);
                break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }


static int VertexAttribFormat(lua_State *L)
/* vertex_attrib_format(index, size, type, normalized, offset)
 * vertex_attrib_format(array, index, size, type, normalized, offset) 
 */
    {
    int arg, named = 0;
    GLuint index, offset, array = 0;
    GLint size;
    GLenum type;
    GLboolean normalized;
    arg = 1;
    if(!lua_isstring(L, 3)) /* 'type' */
        {
        named = 1;
        array = luaL_checkinteger(L, arg++);
        }
    index = luaL_checkinteger(L, arg++);
    size = luaL_checkinteger(L, arg++); /* no. of components */
    type = checktype(L, arg++);
    normalized = checkboolean(L, arg++);
    offset = luaL_checkinteger(L, arg++);
    switch(type)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_FIXED:
        case GL_FLOAT:
        case GL_HALF_FLOAT:
                if(named)
                    glVertexArrayAttribFormat(array, index, size, type, normalized, offset);
                    //glVertexArrayAttribIFormat(index, size, type, offset);
                else
                    glVertexAttribFormat(index, size, type, normalized, offset);
                    //glVertexAttribIFormat(index, size, type, offset);
                break;
        case GL_DOUBLE:
                if(named)
                    glVertexArrayAttribLFormat(array, index, size, GL_DOUBLE, offset);
                else
                    glVertexAttribLFormat(index, size, GL_DOUBLE, offset);
                break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }


static int BindVertexBuffer(lua_State *L)
/* bind_vertex_buffer(index, buffer, offset, stride)
 * bind_vertex_buffer(array, index, buffer, offset, stride)
 */
    {
    int arg, named=0;
    GLuint array, index, buffer;
    GLintptr offset;
    GLsizei stride;
    arg = 1;
    if(lua_isinteger(L, 5))
        {
        named = 1;
        array = luaL_checkinteger(L, arg++);
        }
    index = luaL_checkinteger(L, arg++);
    buffer = luaL_checkinteger(L, arg++);
    offset = luaL_checkinteger(L, arg++);
    stride = luaL_checkinteger(L, arg++);
    if(named)
        glVertexArrayVertexBuffer(array, index, buffer, offset, stride);
    else    
        glBindVertexBuffer(index, buffer, offset, stride);
    CheckError(L);
    return 0;
    }

static int BindVertexBuffers(lua_State *L)
/*
 * bind_vertex_buffers(first, buffer1, offset1, stride1 [, buffer2, offset2, stride2, ...])
 * bind_vertex_buffers(array, first, buffer1, offset1, stride1 [, buffer2, offset2, stride2, ...])
 */
    {
    GLsizei named, count, i, arg;
    GLuint array, first;
    GLuint *buffers;
    GLintptr *offsets;
    GLsizei *strides;
    
    arg = 1;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    count = arg - 1;
    if(count < 4)
        return luaL_checkinteger(L, arg); /* raises an error */
    arg = 1;
    if(((count-2)%3) == 0)
        {
        named = 1;
        count = (count - 2)/3;
        array = luaL_checkinteger(L, arg++);
        }
    else if(((count-1)%3) == 0)
        {
        named = 0;
        count = (count - 1)/3;
        }
    else
        return luaL_error(L, NARGS_ERROR);
    first = lua_tointeger(L, arg++);
    buffers = (GLuint*)Malloc3(L, count*sizeof(GLuint),
                        (void**)&offsets, count*sizeof(GLintptr),
                        (void**)&strides, count*sizeof(GLsizei));
    for(i=0; i<count; i++)
        {
        buffers[i] = lua_tointeger(L, arg++);
        offsets[i] = lua_tointeger(L, arg++);
        strides[i] = lua_tointeger(L, arg++);
        }
    if(named)
        glVertexArrayVertexBuffers(array, first, count, buffers, offsets, strides);
    else
        glBindVertexBuffers(first, count, buffers, offsets, strides);
    Free(L, buffers);
    Free(L, offsets);
    Free(L, strides);
    CheckError(L);
    return 0;
    }

static int VertexAttribBinding(lua_State *L)
    {
    int arg, named = 0;
    GLuint array, attribindex, bindingindex;
    arg = 1;
    if(lua_isinteger(L, 3))
        {
        named = 1;
        array = luaL_checkinteger(L, arg++);
        }
    attribindex = luaL_checkinteger(L, arg++);
    bindingindex = luaL_checkinteger(L, arg++);
    if(named)
        glVertexArrayAttribBinding(array, attribindex, bindingindex);
    else
        glVertexAttribBinding(attribindex, bindingindex);
    CheckError(L);
    return 0;
    }


static int PatchParameter(lua_State *L)
/* patch_parameter(pname, val1, ...) */
    {
    int arg, i, count;
    GLint value;
    GLfloat *values;
    GLenum pname = checkpatchpname(L, 1);
    switch(pname)
        {
        case GL_PATCH_VERTICES:
                value = luaL_checkinteger(L, 2);
                glPatchParameteri(pname, value);
                CheckError(L);
                return 0;
        case GL_PATCH_DEFAULT_OUTER_LEVEL:
        case GL_PATCH_DEFAULT_INNER_LEVEL:
                arg = 2;
                while(!lua_isnoneornil(L, arg))
                    luaL_checknumber(L, arg++);
                if(arg==2)
                    luaL_checknumber(L, 2); /* raises an error */
                count = arg - 2;
                values = (GLfloat*)Malloc(L, count*sizeof(GLfloat));
                arg = 2;
                for(i=0; i<count; i++)
                    values[i] = lua_tonumber(L, arg++);
                glPatchParameterfv(pname, values);
                Free(L, values);
                CheckError(L);
                return 0;
        default:
            return luaL_error(L,UNEXPECTED_ERROR);
        }
    return 0;
    }

UINT2_FUNC(VertexAttribDivisor)

static int EnableVertexAttribArray(lua_State *L)
/* enable_vertex_attrib_array(index)
 * enable_vertex_attrib_array(array, index)
 */
    {
    GLuint index, array;    
    if(lua_isinteger(L, 2))
        {
        array = luaL_checkinteger(L, 1);
        index = luaL_checkinteger(L, 2);
        glEnableVertexArrayAttrib(array, index);
        }
    else
        {
        index = luaL_checkinteger(L, 1);
        glEnableVertexAttribArray(index);
        }
    CheckError(L);
    return 0;
    }

static int DisableVertexAttribArray(lua_State *L)
/* disable_vertex_attrib_array(index)
 * disable_vertex_attrib_array(array, index)
 */
    {
    GLuint index, array;    
    if(lua_isinteger(L, 2))
        {
        array = luaL_checkinteger(L, 1);
        index = luaL_checkinteger(L, 2);
        glDisableVertexArrayAttrib(array, index);
        }
    else
        {
        index = luaL_checkinteger(L, 1);
        glDisableVertexAttribArray(index);
        }
    CheckError(L);
    return 0;
    }

static int VertexBindingDivisor(lua_State *L)
/* vertex_binding_divisor(index, divisor)
 * vertex_binding_divisor(array, index, divisor)
 */
    {
    GLuint index, divisor, array;   
    if(lua_isinteger(L, 3))
        {
        array = luaL_checkinteger(L, 1);
        index = luaL_checkinteger(L, 2);
        divisor = luaL_checkinteger(L, 3);
        glVertexArrayBindingDivisor(array, index, divisor);
        }
    else
        {
        index = luaL_checkinteger(L, 1);
        divisor = luaL_checkinteger(L, 2);
        glVertexBindingDivisor(index, divisor);
        }
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Vertex Attributes                                                            |
 *------------------------------------------------------------------------------*/

static int BindAttribLocation(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    const GLchar *name = luaL_checkstring(L, 3);
    glBindAttribLocation(program, index, name);
    CheckError(L);
    return 0;
    }

static int GetAttribLocation(lua_State *L)
    {
    GLuint program = luaL_checkinteger(L, 1);
    const GLchar *name = luaL_checkstring(L, 2);
    GLint index = glGetAttribLocation(program, name);
    CheckError(L);
    lua_pushinteger(L, index);
    return 1;
    }


static int GetActiveAttrib(lua_State *L)
/* name, type, size = get_active_attribute(program, index) */
    {
    GLint size;
    GLenum type;
    GLsizei length;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLsizei bufsz = programGet(L, program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
    GLchar *name = (GLchar*)Malloc(L, bufsz*sizeof(GLchar));
    glGetActiveAttrib(program, index, bufsz, &length, &size, &type, name);
    CheckErrorFree(L, name);
    if(length == 0)
        { Free(L, name); return 0; }
    lua_pushstring(L, name);
    Free(L, name);
    pushglsltype(L, type);
    lua_pushinteger(L, size);
    return 3;
    }


/*------------------------------------------------------------------------------*
 | Vertex Array Objects                                                         |
 *------------------------------------------------------------------------------*/

NEW_FUNC(VertexArray, OTYPE_VERTEX_ARRAY)
GEN_FUNC(VertexArray, OTYPE_VERTEX_ARRAY)
BIND_FUNC(VertexArray)
DELETE_FUNC(VertexArray, OTYPE_VERTEX_ARRAY)
IS_FUNC(VertexArray)
CREATE_FUNC(VertexArray, OTYPE_VERTEX_ARRAY)

static int VertexArrayElementBuffer(lua_State *L)
    {
    GLuint array = luaL_checkinteger(L, 1);
    GLuint buffer = luaL_checkinteger(L, 2);
    glVertexArrayElementBuffer(array, buffer);
    CheckError(L);
    return 0;
    }



/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "patch_parameter", PatchParameter },
        { "vertex_attrib", VertexAttrib },
        { "vertex_attrib_n", VertexAttribN },
        { "vertex_attrib_p", VertexAttribP },
        { "enable_vertex_attrib_array", EnableVertexAttribArray },
        { "disable_vertex_attrib_array", DisableVertexAttribArray },
        { "vertex_attrib_pointer", VertexAttribPointer },
        { "vertex_attrib_i_pointer", VertexAttribPointerI },
        { "vertex_attrib_format", VertexAttribFormat },
        { "bind_vertex_buffer", BindVertexBuffer },
        { "bind_vertex_buffers", BindVertexBuffers },
        { "vertex_attrib_binding", VertexAttribBinding },
        { "vertex_binding_divisor", VertexBindingDivisor },
        { "vertex_attrib_divisor", VertexAttribDivisor },
        { "new_vertex_array", NewVertexArray },
        { "gen_vertex_arrays", GenVertexArrays },
        { "bind_vertex_array", BindVertexArray },
        { "create_vertex_arrays", CreateVertexArrays },
        { "is_vertex_array", IsVertexArray },
        { "delete_vertex_arrays", DeleteVertexArrays },
        { "vertex_array_element_buffer", VertexArrayElementBuffer },
        { "bind_attrib_location", BindAttribLocation },
        { "get_active_attrib", GetActiveAttrib },
        { "get_attrib_location", GetAttribLocation },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_vertex_array(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


