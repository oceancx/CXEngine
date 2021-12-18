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

BITFIELD_STRINGS(BufferBitStrings) = {
    "color", 
    "depth", 
    "stencil", 
    NULL
};
BITFIELD_CODES(BufferBitCodes) = {
    GL_COLOR_BUFFER_BIT,
    GL_DEPTH_BUFFER_BIT,
    GL_STENCIL_BUFFER_BIT
};
BITFIELD_T(BufferBitBitfield, BufferBitStrings, BufferBitCodes)
#define CheckBufferBit(L, arg, mand) bitfieldCheck((L), (arg), (mand), &BufferBitBitfield)
#define PushBufferBit(L, code) bitfieldPush((L), (code), &BufferBitBitfield)

bitfield_t *bitfieldBuffer(void)
    { return &BufferBitBitfield; }

static GLenum* CheckBufList(lua_State *L, int arg, GLsizei *n)
/* the caller must Free() the returned bufs */
    {
    GLenum *bufs;
    int i, err;
    if(lua_istable(L, arg))
        {
        *n = luaL_len(L, arg);
        if(*n==0) /* raise an error */
            checkbuffer(L, arg);
        bufs = (GLenum*)Malloc(L, (*n)*sizeof(GLenum));
        for(i = 0; i < (*n); i++)
            {
            lua_rawgeti(L, arg, i+1);
            bufs[i] = testbuffer(L, -1, &err);
            if(err) /* raise an error */
                { Free(L, bufs); checkbuffer(L, -1); }
            lua_pop(L, 1);
            }
        }
    else
        {
        i = arg;
        while(!lua_isnoneornil(L, i))
            { checkbuffer(L, i); i++; }
        if(i==arg) /* raise an error */
            checkbuffer(L, arg);
        *n = i - arg;
        bufs = (GLenum*)Malloc(L, (*n)*sizeof(GLenum));
        i = 0;
        for(i = 0; i < (*n); i++)
            bufs[i] = checkbuffer(L, arg + i);
        }
    return bufs;
    }

static int ColorMask(lua_State *L)
    {
    GLuint buf;
    GLboolean r, g, b, a;
    if(lua_isinteger(L, 1))
        {
        buf = luaL_checkinteger(L, 1);
        r = checkboolean(L, 2);
        g = checkboolean(L, 3);
        b = checkboolean(L, 4);
        a = checkboolean(L, 5);
        glColorMaski(buf, r, g, b, a);
        }
    else
        {
        r = checkboolean(L, 1);
        g = checkboolean(L, 2);
        b = checkboolean(L, 3);
        a = checkboolean(L, 4);
        glColorMask(r, g, b, a);
        }
    CheckError(L);
    return 0;
    }

BOOLEAN_FUNC(DepthMask)

static int StencilMask(lua_State *L)
    {
    GLenum face;
    GLuint mask = luaL_checkinteger(L, 1);
    if(lua_isstring(L, 2))
        {
        face = checkface(L, 2);
        glStencilMaskSeparate(face, mask);
        }
    else
        glStencilMask(mask);
    CheckError(L);
    return 0;
    }

static int Clear(lua_State *L)
    {
    GLbitfield mask = CheckBufferBit(L, 1, 0);
    glClear(mask);
    CheckError(L);
    return 0;
    }

static int ClearColor(lua_State *L)
    {
    GLfloat color[4];
    checkcolor(L, 1, color);
    glClearColor(color[0], color[1], color[2], color[3]);
    CheckError(L);
    return 0;
    }

DOUBLE_FUNC(ClearDepth)

INT_FUNC(ClearStencil)

static int DrawBuffer(lua_State *L)
    {
    GLenum buf;
    GLuint framebuffer;
    if(lua_isinteger(L, 1))
        {
        framebuffer = luaL_checkinteger(L, 1);
        buf = checkbuffer(L, 2);
        glNamedFramebufferDrawBuffer(framebuffer, buf);
        }
    else
        {
        buf = checkbuffer(L, 1);
        glDrawBuffer(buf);
        }
    CheckError(L);
    return 0;
    }

static int DrawBuffers(lua_State *L)
    {
    int named, arg = 1;
    GLuint framebuffer = 0;
    GLsizei n;
    GLenum *bufs;
    if(lua_isinteger(L, arg))
        {
        named = 1;
        framebuffer = luaL_checkinteger(L, arg++);
        }
    else
        named = 0;
    bufs = CheckBufList(L, arg, &n);
    if(named)
        glNamedFramebufferDrawBuffers(framebuffer, n, bufs);
    else
        glDrawBuffers(n, bufs);
    Free(L, bufs);
    CheckError(L);
    return 0;
    }



static int ClearBuffer(lua_State *L)
/* clear_buffer("color", drawbuffer, red, green, blue, alpha)
 * clear_buffer("depth", value)
 * clear_buffer("stencil", value)
 */
    {
    int named, arg = 1;
    GLuint framebuffer;
    GLint drawbuffer;
    GLfloat value[4];
    GLint stencil;
    GLenum buffer;
    if(lua_isinteger(L, 1))
        { 
        named = 1; 
        framebuffer = luaL_checkinteger(L, arg++);
        }
    else
        named = 0;
    buffer = checkclearbuffer(L, arg++);
    switch(buffer)
        {
        case GL_COLOR:      drawbuffer = luaL_checkinteger(L, arg++);
                            checkcolor(L, arg, value);
                            if(named)
                                glClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value);
                            else
                                glClearBufferfv(buffer, drawbuffer, value);
                            break;

        case GL_DEPTH:      value[0] = luaL_checknumber(L, arg++);
                            if(named)
                                glClearNamedFramebufferfv(framebuffer, buffer, 0, value);
                            else
                                glClearBufferfv(buffer, 0, value);
                            break;
        case GL_STENCIL:    stencil = luaL_checkinteger(L, arg++);
                            if(named)
                                glClearNamedFramebufferiv(framebuffer, buffer, 0, &stencil);
                            else
                                glClearBufferiv(buffer, 0, &stencil);
                            break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

static int ClearBufferfi(lua_State *L)
/* clear_bufferfi(depth, stencil)
 */
    {
    int named, arg = 1;
    GLuint framebuffer;
    GLfloat depth;
    GLint stencil;
    if(lua_isinteger(L, 1))
        {
        named = 1;
        framebuffer = luaL_checkinteger(L, arg++);
        }
    else
        named = 0;
    depth = luaL_checknumber(L, arg++);
    stencil = luaL_checkinteger(L, arg++);
    if(named)
        {
#if 0
/*@@NOTE: The prototype for glClearNamedFramebufferfi() in glcorearb.h is/was wrong,
 *        (rfr: https://www.khronos.org/bugzilla/show_bug.cgi?id=1394 ),
 *        so this may not compile with OpenGL v4.5.
 *      In such a case, you can update the glcorearb.h file on your system with the
 *      correct version, or just disable this function by setting to 0 the above "#if 1".
 */
        glClearNamedFramebufferfi(framebuffer, GL_DEPTH_STENCIL, 0, depth, stencil);
#else
        (void)framebuffer;
        NOT_AVAILABLE;
#endif
        }
    else
        glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "color_mask", ColorMask },
        { "depth_mask", DepthMask },
        { "stencil_mask", StencilMask },
        { "clear", Clear },
        { "clear_color", ClearColor },
        { "clear_depth", ClearDepth },
        { "clear_stencil", ClearStencil },
        { "draw_buffer", DrawBuffer },
        { "draw_buffers", DrawBuffers },
        { "clear_buffer", ClearBuffer },
        { "clear_bufferfi", ClearBufferfi },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_whole_framebuffer(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

