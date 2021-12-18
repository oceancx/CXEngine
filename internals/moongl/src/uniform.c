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

#define Dimension(dim) (((dim) & 0x00ff) * (((dim) & 0xff00)>>8))

/* #define checkboolean */
#define checkinteger luaL_checkinteger
#define checknumber luaL_checknumber

static int CheckN(lua_State *L, int first)
    {
    int arg = first;
    while(!lua_isnoneornil(L, arg)) arg++;
    if(arg == first) return luaL_error(L, NARGS_ERROR);
    return arg - first;
    }


/*------------------------------------------------------------------------------*
 | uniform()                                                                    |
 *------------------------------------------------------------------------------*/

#define UNIFORM_FUNC(T, nt, t, checkt)                                  \
static int Uniform_##nt(lua_State *L, GLint location, int arg)          \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    int n = CheckN(L, arg);                                             \
    if(n > 4) return luaL_error(L, NARGS_ERROR);                        \
    while(i < n)                                                        \
        values[i++] = check##checkt(L, arg++);                          \
    switch(n)                                                           \
        {                                                               \
        case 1: glUniform1##t##v(location, 1, values); break;           \
        case 2: glUniform2##t##v(location, 1, values); break;           \
        case 3: glUniform3##t##v(location, 1, values); break;           \
        case 4: glUniform4##t##v(location, 1, values); break;           \
        default: return luaL_error(L, UNEXPECTED_ERROR);                \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

UNIFORM_FUNC(GLint, b, i, boolean)
UNIFORM_FUNC(GLint, i, i, integer)
UNIFORM_FUNC(GLuint, ui, ui, integer)
UNIFORM_FUNC(GLfloat, f, f, number)
UNIFORM_FUNC(GLdouble, d, d, number)

static int Uniform(lua_State *L)
    {
    GLint location = luaL_checkinteger(L, 1);
    GLenum type = checkuniformtype(L, 2);
#define arg_first 3
    switch(type)
        {
        case NONGL_BOOLEAN: return Uniform_b(L, location, arg_first);
        case GL_INT: return Uniform_i(L, location, arg_first);
        case GL_UNSIGNED_INT: return Uniform_ui(L, location, arg_first);
        case GL_FLOAT: return Uniform_f(L, location, arg_first);
        case GL_DOUBLE: return Uniform_d(L, location, arg_first);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef arg_first
    }

/*------------------------------------------------------------------------------*
 | uniformv()                                                                   |
 *------------------------------------------------------------------------------*/

#define UNIFORMV_FUNC(T, nt, t, checkt)                                 \
static int Uniformv_##nt(lua_State *L, GLint location, GLsizei count, int arg)  \
    {                                                                   \
    T *values;                                                          \
    int i = 0;                                                          \
    int n, m = CheckN(L, arg);                                          \
    if((m % count) != 0)                                                \
        return luaL_error(L, NARGS_ERROR);                              \
    values = (T*)Malloc(L, m*sizeof(T));                                \
    while(i < m)                                                        \
        {                                                               \
        if(!lua_is##checkt(L, arg))                                     \
            { Free(L, values); check##checkt(L, arg); return 0; }       \
        values[i++] = lua_to##checkt(L, arg++);                         \
        }                                                               \
    n = m / count;                                                      \
    switch(n)                                                           \
        {                                                               \
        case 1: glUniform1##t##v(location, count, values); break;       \
        case 2: glUniform2##t##v(location, count, values); break;       \
        case 3: glUniform3##t##v(location, count, values); break;       \
        case 4: glUniform4##t##v(location, count, values); break;       \
        default:                                                        \
            Free(L, values);                                            \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    Free(L, values);                                                    \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

UNIFORMV_FUNC(GLint, b, i, boolean)
UNIFORMV_FUNC(GLint, i, i, integer)
UNIFORMV_FUNC(GLuint, ui, ui, integer)
UNIFORMV_FUNC(GLfloat, f, f, number)
UNIFORMV_FUNC(GLdouble, d, d, number)

static int Uniformv(lua_State *L)
/* uniformv(location, count, type, val1 [, val2 ... val4]) */
    {
    GLint location = luaL_checkinteger(L, 1);
    GLsizei count = luaL_checkinteger(L, 2);
    GLenum type = checkuniformtype(L, 3);
    if(count<=0)
        return luaL_error(L, "count should be positive");
#define arg_first 4
    switch(type)
        {
        case NONGL_BOOLEAN: return Uniformv_b(L, location, count, arg_first);
        case GL_INT: return Uniformv_i(L, location, count, arg_first);
        case GL_UNSIGNED_INT: return Uniformv_ui(L, location, count, arg_first);
        case GL_FLOAT: return Uniformv_f(L, location, count, arg_first);
        case GL_DOUBLE: return Uniformv_d(L, location, count, arg_first);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef arg_first
    }



/*------------------------------------------------------------------------------*
 | program_uniform()                                                            |
 *------------------------------------------------------------------------------*/


#define PROGRAM_UNIFORM_FUNC(T, nt, t, checkt)                          \
static int ProgramUniform_##nt(lua_State *L, GLuint program, GLint location, int arg)   \
    {                                                                   \
    T values[4];                                                        \
    int i = 0;                                                          \
    int n = CheckN(L, arg);                                             \
    if(n > 4) return luaL_error(L, NARGS_ERROR);                        \
    while(i < n)                                                        \
        values[i++] = check##checkt(L, arg++);                          \
    switch(n)                                                           \
        {                                                               \
        case 1: glProgramUniform1##t##v(program, location, 1, values); break;   \
        case 2: glProgramUniform2##t##v(program, location, 1, values); break;   \
        case 3: glProgramUniform3##t##v(program, location, 1, values); break;   \
        case 4: glProgramUniform4##t##v(program, location, 1, values); break;   \
        default: return luaL_error(L, UNEXPECTED_ERROR);                \
        }                                                               \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

PROGRAM_UNIFORM_FUNC(GLint, b, i, boolean)
PROGRAM_UNIFORM_FUNC(GLint, i, i, integer)
PROGRAM_UNIFORM_FUNC(GLuint, ui, ui, integer)
PROGRAM_UNIFORM_FUNC(GLfloat, f, f, number)
PROGRAM_UNIFORM_FUNC(GLdouble, d, d, number)

static int ProgramUniform(lua_State *L)
/* program_uniform(program, location, type, val1 [, val2 ... val4]) */
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    GLenum type = checkuniformtype(L, 3);
#define arg_first 4
    switch(type)
        {
        case NONGL_BOOLEAN: return ProgramUniform_b(L, program, location, arg_first);
        case GL_INT: return ProgramUniform_i(L, program, location, arg_first);
        case GL_UNSIGNED_INT: return ProgramUniform_ui(L, program, location, arg_first);
        case GL_FLOAT: return ProgramUniform_f(L, program, location, arg_first);
        case GL_DOUBLE: return ProgramUniform_d(L, program, location, arg_first);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef arg_first
    }


/*------------------------------------------------------------------------------*
 | program_uniformv()                                                           |
 *------------------------------------------------------------------------------*/

#define PROGRAM_UNIFORMV_FUNC(T, nt, t, checkt)                         \
static int ProgramUniformv_##nt                                         \
            (lua_State *L, GLuint program, GLint location, GLsizei count, int arg)  \
    {                                                                   \
    T *values;                                                          \
    int i = 0;                                                          \
    int n, m = CheckN(L, arg);                                          \
    if((m % count) != 0)                                                \
        return luaL_error(L, NARGS_ERROR);                              \
    values = (T*)Malloc(L, m*sizeof(T));                                \
    while(i < m)                                                        \
        {                                                               \
        if(!lua_is##checkt(L, arg))                                     \
            { Free(L, values); check##checkt(L, arg); return 0; }       \
        values[i++] = lua_to##checkt(L, arg++);                         \
        }                                                               \
    n = m / count;                                                      \
    switch(n)                                                           \
        {                                                               \
        case 1: glProgramUniform1##t##v(program, location, count, values); break;       \
        case 2: glProgramUniform2##t##v(program, location, count, values); break;       \
        case 3: glProgramUniform3##t##v(program, location, count, values); break;       \
        case 4: glProgramUniform4##t##v(program, location, count, values); break;       \
        default:                                                        \
            Free(L, values);                                            \
            return luaL_error(L, NARGS_ERROR);                          \
        }                                                               \
    Free(L, values);                                                    \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

PROGRAM_UNIFORMV_FUNC(GLint, b, i, boolean)
PROGRAM_UNIFORMV_FUNC(GLint, i, i, integer)
PROGRAM_UNIFORMV_FUNC(GLuint, ui, ui, integer)
PROGRAM_UNIFORMV_FUNC(GLfloat, f, f, number)
PROGRAM_UNIFORMV_FUNC(GLdouble, d, d, number)

static int ProgramUniformv(lua_State *L)
/* program_uniformv(program, location, count, type, val1 [, val2 ... val4]) */
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    GLsizei count = luaL_checkinteger(L, 3);
    GLenum type = checkuniformtype(L, 4);
    if(count<=0)
        return luaL_error(L, "count should be positive");
#define arg_first 5
    switch(type)
        {
        case NONGL_BOOLEAN: return ProgramUniformv_b(L, program, location, count, arg_first);
        case GL_INT: return ProgramUniformv_i(L, program, location, count, arg_first);
        case GL_UNSIGNED_INT: return ProgramUniformv_ui(L, program, location, count, arg_first);
        case GL_FLOAT: return ProgramUniformv_f(L, program, location, count, arg_first);
        case GL_DOUBLE: return ProgramUniformv_d(L, program, location, count, arg_first);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef arg_first
    }



/*------------------------------------------------------------------------------*
 | uniform_matrix() and uniform_matrixv()                                       |
 *------------------------------------------------------------------------------*/

#define UNIFORM_MATRIXV_FUNC(T, nt, t, checkt)                          \
static int UniformMatrixv_##nt                                          \
        (lua_State *L, GLint location, GLsizei count, GLenum dim, GLboolean transpose, int arg)\
    {                                                                   \
    T *values;                                                          \
    int i, n = Dimension(dim)*count;                                    \
    for(i=0; i < n; i++)                                                \
        check##checkt(L, arg+i);                                        \
    values = (T*)Malloc(L, n*sizeof(T));                                \
    for(i=0; i < n; i++)                                                \
        {                                                               \
        values[i] = check##checkt(L, arg+i);                            \
        }                                                               \
    switch(dim)                                                         \
        {                                                               \
        case NONGL_MATRIX_2x2: glUniformMatrix2##t##v(location, count, transpose, values); break; \
        case NONGL_MATRIX_3x3: glUniformMatrix3##t##v(location, count, transpose, values); break; \
        case NONGL_MATRIX_4x4: glUniformMatrix4##t##v(location, count, transpose, values); break; \
        case NONGL_MATRIX_2x3: glUniformMatrix2x3##t##v(location, count, transpose, values); break;\
        case NONGL_MATRIX_3x2: glUniformMatrix3x2##t##v(location, count, transpose, values); break;\
        case NONGL_MATRIX_2x4: glUniformMatrix2x4##t##v(location, count, transpose, values); break;\
        case NONGL_MATRIX_4x2: glUniformMatrix4x2##t##v(location, count, transpose, values); break;\
        case NONGL_MATRIX_3x4: glUniformMatrix3x4##t##v(location, count, transpose, values); break;\
        case NONGL_MATRIX_4x3: glUniformMatrix4x3##t##v(location, count, transpose, values); break;\
        default: Free(L, values); return luaL_error(L, UNEXPECTED_ERROR);               \
        }                                                               \
    Free(L, values);                                                    \
    CheckError(L);                                                      \
    return 0;                                                           \
    }

UNIFORM_MATRIXV_FUNC(GLfloat, f, f, number)
UNIFORM_MATRIXV_FUNC(GLdouble, d, d, number)

static int UniformMatrixv(lua_State *L)
/* uniform_matrixv(location, count, type, dim, transpose, dim values) */
    {
    GLint location = luaL_checkinteger(L, 1);
    GLsizei count = luaL_checkinteger(L, 2);
    GLenum type = checkuniformtype(L, 3);
    GLenum dim = checkmatrixdimensions(L, 4);
    GLboolean transpose = checkboolean(L, 5);
    if(count<=0)
        return luaL_error(L, "count should be positive");
    switch(type)
        {
#define arg_first 6
        case GL_FLOAT: return UniformMatrixv_f(L, location, count, dim, transpose, arg_first);
        case GL_DOUBLE: return UniformMatrixv_d(L, location, count, dim, transpose, arg_first);
        case NONGL_BOOLEAN:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return luaL_error(L, "operation not supported for this type");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
#undef arg_first
        }
    return 0;
    }

static int UniformMatrix(lua_State *L)
/* uniform_matrix(location, type, dim, transpose, dim values) */
    {
    GLint location = luaL_checkinteger(L, 1);
    GLenum type = checkuniformtype(L, 2);
    GLenum dim = checkmatrixdimensions(L, 3);
    GLboolean transpose = checkboolean(L, 4);
    switch(type)
        {
#define arg_first 5
        case GL_FLOAT: return UniformMatrixv_f(L, location, 1, dim, transpose, arg_first);
        case GL_DOUBLE: return UniformMatrixv_d(L, location, 1, dim, transpose, arg_first);
        case NONGL_BOOLEAN:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return luaL_error(L, "operation not supported for this type");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
#undef arg_first
        }
    return 0;
    }


/*------------------------------------------------------------------------------*
 | program_uniform_matrix() and program_uniform_matrixv()                       |
 *------------------------------------------------------------------------------*/

#define PROGRAM_UNIFORM_MATRIXV_FUNC(T, nt, t, checkt)                      \
static int ProgramUniformMatrixv_##nt                                       \
        (lua_State *L, GLuint program, GLint location, GLsizei count,       \
                GLenum dim, GLboolean transpose, int arg)                   \
    {                                                                       \
    T *values;                                                              \
    int i, n = Dimension(dim)*count;                                        \
    for(i=0; i < n; i++)                                                    \
        check##checkt(L, arg+i);                                            \
    values = (T*)Malloc(L, n*sizeof(T));                                    \
    for(i=0; i < n; i++)                                                    \
        {                                                                   \
        values[i] = check##checkt(L, arg+i);                                \
        }                                                                   \
    switch(dim)                                                             \
        {                                                                   \
        case NONGL_MATRIX_2x2: glProgramUniformMatrix2##t##v                      \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_3x3: glProgramUniformMatrix3##t##v                      \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_4x4: glProgramUniformMatrix4##t##v                      \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_2x3: glProgramUniformMatrix2x3##t##v                    \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_3x2: glProgramUniformMatrix3x2##t##v                    \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_2x4: glProgramUniformMatrix2x4##t##v                    \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_4x2: glProgramUniformMatrix4x2##t##v                    \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_3x4: glProgramUniformMatrix3x4##t##v                    \
                        (program, location, count, transpose, values); break;   \
        case NONGL_MATRIX_4x3: glProgramUniformMatrix4x3##t##v                    \
                        (program, location, count, transpose, values); break;   \
        default:                                                            \
            Free(L, values); return luaL_error(L, UNEXPECTED_ERROR);        \
        }                                                                   \
    Free(L, values);                                                        \
    CheckError(L);                                                          \
    return 0;                                                               \
    }

PROGRAM_UNIFORM_MATRIXV_FUNC(GLfloat, f, f, number)
PROGRAM_UNIFORM_MATRIXV_FUNC(GLdouble, d, d, number)

static int ProgramUniformMatrixv(lua_State *L)
/* program_uniform_matrixv(program, location, count, type, dim, transpose, dim values) */
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    GLsizei count = luaL_checkinteger(L, 3);
    GLenum type = checkuniformtype(L, 4);
    GLenum dim = checkmatrixdimensions(L, 5);
    GLboolean transpose = checkboolean(L, 6);
    if(count<=0)
        return luaL_error(L, "count should be positive");
    switch(type)
        {
#define arg_first 7
        case GL_FLOAT: 
            return ProgramUniformMatrixv_f(L, program, location, count, dim, transpose, arg_first);
        case GL_DOUBLE: 
            return ProgramUniformMatrixv_d(L, program, location, count, dim, transpose, arg_first);
        case NONGL_BOOLEAN:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return luaL_error(L, "operation not supported for this type");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
#undef arg_first
        }
    return 0;
    }

static int ProgramUniformMatrix(lua_State *L)
/* program_uniform_matrix(program, location, type, dim, transpose, dim values) */
    {
    GLuint program = luaL_checkinteger(L, 1);
    GLint location = luaL_checkinteger(L, 2);
    GLenum type = checkuniformtype(L, 3);
    GLenum dim = checkmatrixdimensions(L, 4);
    GLboolean transpose = checkboolean(L, 5);
    switch(type)
        {
#define arg_first 6
        case GL_FLOAT: 
            return ProgramUniformMatrixv_f(L, program, location, 1, dim, transpose, arg_first);
        case GL_DOUBLE: 
            return ProgramUniformMatrixv_d(L, program, location, 1, dim, transpose, arg_first);
        case NONGL_BOOLEAN:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return luaL_error(L, "operation not supported for this type");
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
#undef arg_first
        }
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "uniform", Uniform },
        { "uniformv", Uniformv },
        { "uniform_matrix", UniformMatrix },
        { "uniform_matrixv", UniformMatrixv },
        { "program_uniform", ProgramUniform },
        { "program_uniformv", ProgramUniformv },
        { "program_uniform_matrix", ProgramUniformMatrix },
        { "program_uniform_matrixv", ProgramUniformMatrixv },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_uniform(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


