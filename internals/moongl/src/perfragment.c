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

static int Scissor(lua_State *L)
    {
    GLint x, y;
    GLsizei width, height;
    GLint v[4];
    GLint index;
    if(lua_isinteger(L, 5))
        {
        v[0] = luaL_checkinteger(L, 1);
        v[1] = luaL_checkinteger(L, 2);
        v[2] = luaL_checkinteger(L, 3);
        v[3] = luaL_checkinteger(L, 4);
        index = luaL_checkinteger(L, 5);
        glScissorIndexedv(index, v);
        }
    else
        {
        x = luaL_checkinteger(L, 1);
        y = luaL_checkinteger(L, 2);
        width = luaL_checkinteger(L, 3);
        height = luaL_checkinteger(L, 4);
        glScissor(x, y, width, height);
        }
    CheckError(L);
    return 0;
    }


static int ScissorArray(lua_State *L)
/* scissor_array(first, x1, x2, w1, h1, x2, y2, ...) */
    {
    int i, arg;
    GLsizei count;
    GLint *v;
    GLuint first = luaL_checkinteger(L, 1);
    count = 0;
    arg = 2;
    while(!lua_isnoneornil(L, arg++))
        luaL_checkinteger(L, arg);

    count = arg - 2;
    if((arg < 6) || ((count % 4)!=0))
        return luaL_error(L, NARGS_ERROR);
    v = (GLint*)Malloc(L, count*sizeof(GLint));
    i = 0;
    arg = 2;
    while(i < count)
        v[i++] = lua_tointeger(L, arg++);
    glScissorArrayv(first, count/4, v);
    Free(L, v);
    CheckError(L);
    return 0;
    }

static int SampleCoverage(lua_State *L)
    {
    GLfloat value = luaL_checknumber(L, 1);
    GLboolean invert = checkboolean(L, 2);
    glSampleCoverage(value, invert);
    CheckError(L);
    return 0;
    }

static int SampleMask(lua_State *L)
    {
    GLuint maskNumber = luaL_checkinteger(L, 1);
    GLbitfield mask = luaL_checkinteger(L, 2);
    glSampleMaski(maskNumber, mask);
    CheckError(L);
    return 0;
    }

static int StencilFunc(lua_State *L)
    {
    GLenum face;
    GLenum func = checkstencilfunc(L, 1);
    GLint ref = luaL_checkinteger(L, 2);
    GLuint mask = luaL_checkinteger(L, 3);
    if(lua_isstring(L, 4))
        {
        face = checkface(L, 4);
        glStencilFuncSeparate(face, func, ref, mask);
        }
    else
        glStencilFunc(func, ref, mask);
    CheckError(L);
    return 0;
    }

static int StencilOp(lua_State *L)
    {
    GLenum face;
    GLenum sfail = checkstencilop(L, 1);
    GLenum dpfail = checkstencilop(L, 2);
    GLenum dppass = checkstencilop(L, 3);
    if(lua_isstring(L, 4))
        {
        face = checkface(L, 4);
        glStencilOpSeparate(face, sfail, dpfail, dppass);
        }
    else
        glStencilOp(sfail, dpfail, dppass);
    CheckError(L);
    return 0;
    }

static int DepthFunc(lua_State *L)
    {
    GLenum func = checkstencilfunc(L, 1);
    glDepthFunc(func);
    CheckError(L);
    return 0;
    }

static int BlendEquation(lua_State *L)
    {
    GLuint buf;
    GLenum mode, alpha;

    if(lua_isinteger(L, 1))
        {
        buf = luaL_checkinteger(L, 1);
        mode = checkblendmode(L, 2);
        if(lua_isstring(L, 3))
            {
            alpha = checkblendmode(L,3);
            glBlendEquationSeparatei(buf, mode, alpha);
            }
        else
            glBlendEquationi(buf, mode);
        }
    else
        {
        mode = checkblendmode(L, 1);
        if(lua_isstring(L, 2))
            {
            alpha = checkblendmode(L,2);
            glBlendEquationSeparate(mode, alpha);
            }
        else
            glBlendEquation(mode);
        }
    CheckError(L);
    return 0;
    }


static int BlendFunc(lua_State *L)
    {
    GLuint buf;
    GLenum sfactor, dfactor, sfactorAlpha, dfactorAlpha;
    if(lua_isinteger(L, 1))
        {
        buf = luaL_checkinteger(L, 1);
        sfactor = checkblendfactor(L, 2);
        dfactor = checkblendfactor(L, 3);
        if(lua_isstring(L, 4))
            {
            sfactorAlpha = checkblendfactor(L, 4);
            dfactorAlpha = checkblendfactor(L, 5);
            glBlendFuncSeparatei(buf, sfactor, dfactor, sfactorAlpha, dfactorAlpha);
            }
        else
            glBlendFunci(buf, sfactor, dfactor);
        }
    else
        {
        sfactor = checkblendfactor(L, 1);
        dfactor = checkblendfactor(L, 2);
        if(lua_isstring(L, 3))
            {
            sfactorAlpha = checkblendfactor(L, 3);
            dfactorAlpha = checkblendfactor(L, 4);
            glBlendFuncSeparate(sfactor, dfactor, sfactorAlpha, dfactorAlpha);
            }
        else
            glBlendFunc(sfactor, dfactor);
        }
    CheckError(L);
    return 0;
    }


static int BlendColor(lua_State *L)
    {
    GLfloat r = luaL_checknumber(L, 1);
    GLfloat g = luaL_checknumber(L, 2);
    GLfloat b = luaL_checknumber(L, 3);
    GLfloat a = luaL_checknumber(L, 4);
    glBlendColor(r, g, b, a);
    CheckError(L);
    return 0;
    }

static int LogicOp(lua_State *L)
    {
    GLenum opcode = checklogicop(L, 1);
    glLogicOp(opcode);
    CheckError(L);
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "scissor", Scissor },
        { "scissor_array", ScissorArray },
        { "sample_coverage", SampleCoverage },
        { "sample_mask", SampleMask },
        { "stencil_func", StencilFunc },
        { "stencil_op", StencilOp },
        { "depth_func", DepthFunc },
        { "blend_equation", BlendEquation },
        { "blend_func", BlendFunc },
        { "blend_color", BlendColor },
        { "logic_op", LogicOp },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_perfragment(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

