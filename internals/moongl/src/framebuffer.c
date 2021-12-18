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

/*--------------------------------------------------------------------------*
 | framebuffer_parameter()                                                  |
 *--------------------------------------------------------------------------*/

static int FramebufferParameter(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum pname = checkframebufferpname(L, 2);
    GLint param = luaL_checkinteger(L, 3);
    if(framebuffer==0)
        glFramebufferParameteri(target, pname, param);
    else
        glNamedFramebufferParameteri(framebuffer, pname, param);
    CheckError(L);
    return 0;
    }

/*--------------------------------------------------------------------------*
 | get_framebuffer_parameter()                                              |
 *--------------------------------------------------------------------------*/

static int GetInt_(lua_State *L, GLenum target, GLuint framebuffer, GLenum pname, int boolean)
#define GetInt(L, target, framebuffer, pname) GetInt_((L),(target),(framebuffer),(pname), 0)
#define GetBoolean(L, target, framebuffer, pname) GetInt_((L),(target),(framebuffer),(pname), 1)
    {
    GLint param;
    if(framebuffer==0)
        glGetFramebufferParameteriv(target, pname, &param);
    else
        glGetNamedFramebufferParameteriv(framebuffer, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param);
    else
        lua_pushinteger(L, param);
    return 1;
    }

static int GetEnum(lua_State *L, GLenum target, GLuint framebuffer, GLenum pname, uint32_t domain)
    {
    GLint param;
    if(framebuffer==0)
        glGetFramebufferParameteriv(target, pname, &param);
    else
        glGetNamedFramebufferParameteriv(framebuffer, pname, &param);
    CheckError(L);
    return enums_push(L, domain, param);
    }

static int GetFramebufferParameter(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum pname = checkframebuffergpname(L, 2);
    switch(pname)
        {
        case GL_FRAMEBUFFER_DEFAULT_WIDTH:
        case GL_FRAMEBUFFER_DEFAULT_HEIGHT:
        case GL_FRAMEBUFFER_DEFAULT_LAYERS:
        case GL_FRAMEBUFFER_DEFAULT_SAMPLES:
        case GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS:
        case GL_SAMPLES:
        case GL_SAMPLE_BUFFERS:
                return GetInt(L, target, framebuffer, pname);
        case GL_DOUBLEBUFFER:
        case GL_STEREO:
                return GetBoolean(L, target, framebuffer, pname);
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
                return GetEnum(L, target, framebuffer, pname, DOMAIN_FORMAT);
        case GL_IMPLEMENTATION_COLOR_READ_TYPE:
                return GetEnum(L, target, framebuffer, pname, DOMAIN_TYPE);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }

/*--------------------------------------------------------------------------*
 | get_framebuffer_attachment_parameter()                                   |
 *--------------------------------------------------------------------------*/


static int AGetInt_(lua_State *L, GLenum target, GLuint framebuffer, 
    GLenum attachment, GLenum pname, int boolean)
#define AGetInt(L, target, framebuffer, attachment, pname) \
            AGetInt_((L),(target),(framebuffer), (attachment), (pname), 0)
#define AGetBoolean(L, target, framebuffer, attachment, pname) \
            AGetInt_((L),(target),(framebuffer), (attachment), (pname), 0)
    {
    GLint param;
    if(framebuffer==0)
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &param);
    else
        glGetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param);
    else
        lua_pushinteger(L, param);
    return 1;
    }

static int AGetEnum(lua_State *L, GLenum target, GLuint framebuffer, GLenum attachment, GLenum pname, uint32_t domain)
    {
    GLint param;
    if(framebuffer==0)
        glGetFramebufferAttachmentParameteriv(target, attachment, pname, &param);
    else
        glGetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, &param);
    CheckError(L);
    return enums_push(L, domain, param);
    }

static int GetFramebufferAttachmentParameter(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum attachment = checkbuffer(L, 2);
    GLenum pname = checkframebufferapname(L, 3);
    switch(pname)
        {
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
                return AGetEnum(L, target, framebuffer, attachment, pname, DOMAIN_OBJECT_TYPE);
        case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
                        return AGetInt(L, target, framebuffer, attachment, pname);
        case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
                return AGetEnum(L, target, framebuffer, attachment, pname, DOMAIN_COMPONENT_TYPE);
        case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE: 
        case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE: 
        case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
        case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE: 
                            return AGetInt(L, target, framebuffer, attachment, pname);
        case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
                return AGetEnum(L, target, framebuffer, attachment, pname, DOMAIN_COLOR_ENCODING);
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
                            return AGetInt(L, target, framebuffer, attachment, pname);
        case GL_FRAMEBUFFER_ATTACHMENT_LAYERED:
                            return AGetBoolean(L, target, framebuffer, attachment, pname);
        case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
                return AGetEnum(L, target, framebuffer, attachment, pname, DOMAIN_TEXTURE_TARGET);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


/*--------------------------------------------------------------------------*
 | check_framebuffer_status()                                               |
 *--------------------------------------------------------------------------*/

static int CheckFramebufferStatus(lua_State *L)
    {
    GLuint framebuffer;
    GLenum status;
    GLuint target = checkframebuffertarget(L, 1);
    if(lua_isnoneornil(L, 2))
        status = glCheckFramebufferStatus(target);
    else 
        {
        framebuffer = luaL_checkinteger(L, 2);
        status = glCheckNamedFramebufferStatus(framebuffer, target);
        }
    CheckError(L);
    pushframebufferstatus(L, status);
    return 1;
    }

/*--------------------------------------------------------------------------*
 | FramebufferRenderbuffer                                                  |
 *--------------------------------------------------------------------------*/

static int FramebufferRenderbuffer(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum attachment = checkbuffer(L, 2);
    GLenum rbtarget = checkrenderbuffertarget(L, 3);
    GLuint renderbuffer = luaL_checkinteger(L, 4);
    if(framebuffer == 0)
        glFramebufferRenderbuffer(target, attachment, rbtarget, renderbuffer);
    else
        glNamedFramebufferRenderbuffer(framebuffer, attachment, rbtarget, renderbuffer);
    CheckError(L);
    return 0;
    }

static int FramebufferTexture(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum attachment = checkbuffer(L, 2);
    GLuint texture = luaL_checkinteger(L, 3);
    GLint level = luaL_checkinteger(L, 4);
    if(framebuffer == 0)
        glFramebufferTexture(target, attachment, texture, level);
    else
        glNamedFramebufferTexture(framebuffer,attachment, texture, level);
    CheckError(L);
    return 0;
    }

static int FramebufferTexture1D(lua_State *L)
    {
    GLenum target = checkframebuffertarget(L, 1);
    GLenum attachment = checkbuffer(L, 2);
    GLenum textarget = checktexturetarget(L, 3);
    GLuint texture = luaL_checkinteger(L, 4);
    GLint level = luaL_checkinteger(L, 5);
    glFramebufferTexture1D(target, attachment, textarget, texture, level);
    CheckError(L);
    return 0;
    }

static int FramebufferTexture2D(lua_State *L)
    {
    GLenum target = checkframebuffertarget(L, 1);
    GLenum attachment = checkbuffer(L, 2);
    GLenum textarget = checktexturetarget(L, 3);
    GLuint texture = luaL_checkinteger(L, 4);
    GLint level = luaL_checkinteger(L, 5);
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
    CheckError(L);
    return 0;
    }

static int FramebufferTexture3D(lua_State *L)
    {
    GLenum target = checkframebuffertarget(L, 1);
    GLenum attachment = checkbuffer(L, 2);
    GLenum textarget = checktexturetarget(L, 3);
    GLuint texture = luaL_checkinteger(L, 4);
    GLint level = luaL_checkinteger(L, 5);
    GLint layer = luaL_checkinteger(L, 6);
    glFramebufferTexture3D(target, attachment, textarget, texture, level, layer);
    CheckError(L);
    return 0;
    }

static int FramebufferTextureLayer(lua_State *L)
    {
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum attachment = checkbuffer(L, 2);
    GLuint texture = luaL_checkinteger(L, 3);
    GLint level = luaL_checkinteger(L, 4);
    GLint layer = luaL_checkinteger(L, 5);
    if(framebuffer == 0)
        glFramebufferTextureLayer(target, attachment, texture, level, layer);
    else
        glNamedFramebufferTextureLayer(framebuffer, attachment, texture, level, layer);
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Invalidating                                                                 |
 *------------------------------------------------------------------------------*/

static GLenum *CheckAttachments(lua_State *L, int argfirst, GLsizei *count)
    {
    GLsizei i, arg = argfirst;
    GLenum *attachments;
    while(!lua_isnoneornil(L, arg))
        luaL_checkstring(L, arg++);
    if(arg == argfirst)
        checkbuffer(L, argfirst); /* raise an error */
    *count = arg - argfirst;
    attachments = (GLenum*)Malloc(L, (*count)*sizeof(GLenum));
    arg = argfirst;
    for(i = 0; i < *count; i++)
        attachments[i] = checkbuffer(L, arg++);
    return attachments;
    }



static int InvalidateSubFramebuffer(lua_State *L)
/* invalidate_sub_framebuffer(target, x, y, w, h, attachm1 [, attachm2, ...]) 
 */
    {
    GLsizei count;
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLuint x = luaL_checkinteger(L, 2);
    GLuint y = luaL_checkinteger(L, 3);
    GLuint width = luaL_checkinteger(L, 4);
    GLuint height = luaL_checkinteger(L, 5);
    GLenum *attachments = CheckAttachments(L, 6, &count);
    if(framebuffer)
        glInvalidateNamedFramebufferSubData(framebuffer, count, attachments, x, y, width, height);
    else
        glInvalidateSubFramebuffer(target, count, attachments, x, y, width, height);
    Free(L, attachments);
    CheckError(L);
    return 0;
    }

static int InvalidateFramebuffer(lua_State *L)
    {
    GLsizei count;
    GLenum target;
    GLuint framebuffer = checktargetorname(L, 1, &target, DOMAIN_FRAMEBUFFER_TARGET);
    GLenum *attachments = CheckAttachments(L, 2, &count);
    if(framebuffer)
        glInvalidateNamedFramebufferData(framebuffer, count, attachments);
    else
        glInvalidateFramebuffer(target, count, attachments);
    Free(L, attachments);
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Gen, bind etc                                                                |
 *------------------------------------------------------------------------------*/


NEW_TARGET_FUNC(Framebuffer, OTYPE_FRAMEBUFFER, checkframebuffertarget)
GEN_FUNC(Framebuffer, OTYPE_FRAMEBUFFER)
BIND_TARGET_FUNC(Framebuffer, checkframebuffertarget)
DELETE_FUNC(Framebuffer, OTYPE_FRAMEBUFFER)
IS_FUNC(Framebuffer)
CREATE_FUNC(Framebuffer, OTYPE_FRAMEBUFFER)

static int TextureBarrier(lua_State *L) 
    { 
    glTextureBarrier();
    CheckError(L);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_framebuffer", NewFramebuffer },
        { "gen_framebuffers", GenFramebuffers },
        { "create_framebuffers", CreateFramebuffers },
        { "bind_framebuffer", BindFramebuffer },
        { "is_framebuffer", IsFramebuffer },
        { "delete_framebuffers", DeleteFramebuffers },
        { "framebuffer_parameter", FramebufferParameter },
        { "get_framebuffer_parameter", GetFramebufferParameter },
        { "get_framebuffer_attachment_parameter", GetFramebufferAttachmentParameter },
        { "check_framebuffer_status", CheckFramebufferStatus },
        { "framebuffer_renderbuffer", FramebufferRenderbuffer },
        { "framebuffer_texture", FramebufferTexture },
        { "framebuffer_texture_1d", FramebufferTexture1D },
        { "framebuffer_texture_2d", FramebufferTexture2D },
        { "framebuffer_texture_3d", FramebufferTexture3D },
        { "framebuffer_texture_layer", FramebufferTextureLayer },
        { "texture_barrier", TextureBarrier },
        { "invalidate_sub_framebuffer", InvalidateSubFramebuffer },
        { "invalidate_framebuffer", InvalidateFramebuffer },
        { NULL, NULL } /* sentinel */
    };


void moongl_open_framebuffer(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

