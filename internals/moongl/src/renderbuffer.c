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

static int RenderbufferStorageMultisample(lua_State *L)
    {
    int arg = 1;
    GLenum target;
    GLuint renderbuffer = checktargetorname(L, arg++, &target, DOMAIN_RENDERBUFFER_TARGET);
    GLsizei samples = luaL_checkinteger(L, arg++);
    GLenum internalformat = checkinternalformat(L, arg++);
    GLsizei width = luaL_checkinteger(L, arg++);
    GLsizei height = luaL_checkinteger(L, arg++);
    if(renderbuffer==0)
        glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
    else
        glNamedRenderbufferStorageMultisample(renderbuffer, samples, internalformat, width, height);
    CheckError(L);
    return 0;
    }

static int RenderbufferStorage(lua_State *L)
    {
    int arg = 1;
    GLenum target;
    GLuint renderbuffer = checktargetorname(L, arg++, &target, DOMAIN_RENDERBUFFER_TARGET);
    GLenum internalformat = checkinternalformat(L, arg++);
    GLsizei width = luaL_checkinteger(L, arg++);
    GLsizei height = luaL_checkinteger(L, arg++);
    if(renderbuffer==0)
        glRenderbufferStorage(target, internalformat, width, height);
    else
        glNamedRenderbufferStorage(renderbuffer, internalformat, width, height);
    CheckError(L);
    return 0;
    }


static int GetInt(lua_State *L, GLenum target, GLuint renderbuffer, GLenum pname)
    {
    GLint param;
    if(renderbuffer==0)
        glGetRenderbufferParameteriv(target, pname, &param);
    else
        glGetNamedRenderbufferParameteriv(renderbuffer, pname, &param);
    CheckError(L);
    lua_pushinteger(L, param);
    return 1;
    }

static int GetInternalFormat(lua_State *L, GLenum target, GLuint renderbuffer, GLenum pname)
    {
    GLint param;
    if(renderbuffer==0)
        glGetRenderbufferParameteriv(target, pname, &param);
    else
        glGetNamedRenderbufferParameteriv(renderbuffer, pname, &param);
    CheckError(L);
    return pushinternalformat(L, param);
    }


static int GetRenderbufferParameter(lua_State *L)
    {
    int arg = 1;
    GLenum target;
    GLuint renderbuffer = checktargetorname(L, arg++, &target, DOMAIN_RENDERBUFFER_TARGET);
    GLenum pname = checkrenderbufferpname(L, arg++);
    switch(pname)
        {   
        case GL_RENDERBUFFER_WIDTH:
        case GL_RENDERBUFFER_HEIGHT:
        case GL_RENDERBUFFER_SAMPLES:
        case GL_RENDERBUFFER_RED_SIZE: 
        case GL_RENDERBUFFER_GREEN_SIZE: 
        case GL_RENDERBUFFER_BLUE_SIZE: 
        case GL_RENDERBUFFER_ALPHA_SIZE: 
        case GL_RENDERBUFFER_DEPTH_SIZE:
        case GL_RENDERBUFFER_STENCIL_SIZE: return GetInt(L, target, renderbuffer, pname);
        case GL_RENDERBUFFER_INTERNAL_FORMAT: 
            return GetInternalFormat(L, target, renderbuffer, pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


NEW_TARGET_FUNC(Renderbuffer, OTYPE_RENDERBUFFER, checkrenderbuffertarget)
GEN_FUNC(Renderbuffer, OTYPE_RENDERBUFFER)
BIND_TARGET_FUNC(Renderbuffer, checkrenderbuffertarget)
DELETE_FUNC(Renderbuffer, OTYPE_RENDERBUFFER)
IS_FUNC(Renderbuffer)
CREATE_FUNC(Renderbuffer, OTYPE_RENDERBUFFER)


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_renderbuffer", NewRenderbuffer },
        { "gen_renderbuffers", GenRenderbuffers },
        { "create_renderbuffers", CreateRenderbuffers },
        { "bind_renderbuffer", BindRenderbuffer },
        { "is_renderbuffer", IsRenderbuffer },
        { "delete_renderbuffers", DeleteRenderbuffers },
        { "renderbuffer_storage_multisample", RenderbufferStorageMultisample },
        { "renderbuffer_storage", RenderbufferStorage },
        { "get_renderbuffer_parameter", GetRenderbufferParameter },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_renderbuffer(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


