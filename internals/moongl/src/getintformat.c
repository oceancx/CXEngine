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

static GLsizei GetNum(lua_State *L, GLenum target, GLenum internalformat, GLenum pname)
    {
    GLint param;
    glGetInternalformativ(target, internalformat, pname, sizeof(param), &param);
    CheckError(L);
    return param;
    }

static int GetInt_(lua_State *L, GLenum target, GLenum internalformat, GLenum pname, int boolean)
#define GetInt(L, target, intfmt, pname) GetInt_((L), (target), (intfmt), (pname), 0)
#define GetBoolean(L, target, intfmt, pname) GetInt_((L), (target), (intfmt), (pname), 1)
    {
    GLint64 param;
    glGetInternalformati64v(target, internalformat, pname, sizeof(param), &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param);
    else
        lua_pushinteger(L, param);
    return 1;
    }

static int GetN(lua_State *L, GLenum target, GLenum internalformat, GLenum pname, GLenum numpname)
    {
    GLint64 *params;
    GLsizei i, num = GetNum(L, target, internalformat, numpname);
    if(num==0) return 0;
    params = (GLint64*)Malloc(L, num*sizeof(GLint64));
    glGetInternalformati64v(target, internalformat, pname, num*sizeof(GLint64), params);
    CheckErrorFree(L, params);
    for(i = 0; i<num; i++)
        lua_pushinteger(L, params[i]);
    Free(L, params);
    return num;
    }

static int GetEnum(lua_State *L, GLenum target, GLenum internalformat, GLenum pname, uint32_t domain)
    {
    GLint param;
    glGetInternalformativ(target, internalformat, pname, sizeof(param), &param);
    CheckError(L);
    return enums_push(L, domain, param);
    }

static int GetInternalformat(lua_State *L)
    {
    GLenum target = checktexturetarget(L, 1);
    GLenum internalformat = checkinternalformat(L, 2);
    GLenum pname = checkintformatpname(L, 3);
    switch(pname)
        {
        /* supported operations queries: */
        case GL_CLEAR_BUFFER:
        case GL_CLEAR_TEXTURE:
        case GL_COMPUTE_TEXTURE:
        case GL_FILTER:
        case GL_FRAGMENT_TEXTURE:
        case GL_FRAMEBUFFER_BLEND:
        case GL_FRAMEBUFFER_RENDERABLE:
        case GL_FRAMEBUFFER_RENDERABLE_LAYERED:
        case GL_GEOMETRY_TEXTURE:
        case GL_MANUAL_GENERATE_MIPMAP:
        case GL_GENERATE_MIPMAP:
        case GL_AUTO_GENERATE_MIPMAP:
        case GL_READ_PIXELS:
        case GL_SHADER_IMAGE_ATOMIC:
        case GL_SHADER_IMAGE_LOAD:
        case GL_SHADER_IMAGE_STORE:
        case GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST:
        case GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE:
        case GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST:
        case GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE:
        case GL_SRGB_READ:
        case GL_SRGB_WRITE:
        case GL_TESS_CONTROL_TEXTURE:
        case GL_TESS_EVALUATION_TEXTURE:
        case GL_TEXTURE_GATHER:
        case GL_TEXTURE_GATHER_SHADOW:
        case GL_TEXTURE_SHADOW:
        case GL_TEXTURE_VIEW:
        case GL_VERTEX_TEXTURE:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_SUPPORTED_OP);
        /* booleans */
        case GL_COLOR_COMPONENTS:
        case GL_COLOR_RENDERABLE:
        case GL_DEPTH_COMPONENTS:
        case GL_DEPTH_RENDERABLE:
        case GL_INTERNALFORMAT_SUPPORTED:  
        case GL_MIPMAP:
        case GL_STENCIL_COMPONENTS:
        case GL_STENCIL_RENDERABLE:
        case GL_TEXTURE_COMPRESSED:
                        return GetBoolean(L, target, internalformat, pname);
        /* integer */
        case GL_INTERNALFORMAT_RED_SIZE:
        case GL_INTERNALFORMAT_GREEN_SIZE:
        case GL_INTERNALFORMAT_ALPHA_SIZE:
        case GL_INTERNALFORMAT_DEPTH_SIZE:
        case GL_INTERNALFORMAT_STENCIL_SIZE:
        case GL_INTERNALFORMAT_BLUE_SIZE:
        case GL_INTERNALFORMAT_SHARED_SIZE:
        case GL_MAX_COMBINED_DIMENSIONS:
        case GL_MAX_WIDTH:
        case GL_MAX_HEIGHT:
        case GL_MAX_DEPTH:
        case GL_MAX_LAYERS:
        case GL_NUM_SAMPLE_COUNTS:
        case GL_TEXTURE_COMPRESSED_BLOCK_SIZE:
        case GL_TEXTURE_COMPRESSED_BLOCK_WIDTH:
        case GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT:
        case GL_IMAGE_TEXEL_SIZE:
                         return GetInt(L, target, internalformat, pname);
        /* N integers */
        case GL_SAMPLES:
                    return GetN(L, target, internalformat, pname, GL_NUM_SAMPLE_COUNTS);
        /* enums */
        case GL_COLOR_ENCODING:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_COLOR_ENCODING);
        case GL_GET_TEXTURE_IMAGE_FORMAT:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_FORMAT);
        case GL_GET_TEXTURE_IMAGE_TYPE:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_TYPE);
        case GL_IMAGE_COMPATIBILITY_CLASS:
                return GetEnum(L, target, internalformat, pname, DOMAIN_IMAGE_COMPATIBILITY_CLASS);
        case GL_IMAGE_FORMAT_COMPATIBILITY_TYPE:
                return GetEnum(L, target, internalformat, pname, DOMAIN_IMAGE_FORMAT_COMP_TYPE);
        case GL_IMAGE_PIXEL_FORMAT:
                return GetEnum(L, target, internalformat, pname, DOMAIN_FORMAT);
        case GL_IMAGE_PIXEL_TYPE:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_TYPE);
        case GL_VIEW_COMPATIBILITY_CLASS:
                return GetEnum(L, target, internalformat, pname, DOMAIN_VIEW_COMPATIBILITY_TYPE);
        case GL_INTERNALFORMAT_PREFERRED:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_INTERNAL_FORMAT);
        case GL_INTERNALFORMAT_RED_TYPE:
        case GL_INTERNALFORMAT_GREEN_TYPE:
        case GL_INTERNALFORMAT_BLUE_TYPE:
        case GL_INTERNALFORMAT_ALPHA_TYPE:
        case GL_INTERNALFORMAT_DEPTH_TYPE:
        case GL_INTERNALFORMAT_STENCIL_TYPE:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_COMPONENT_TYPE);
        case GL_READ_PIXELS_FORMAT:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_FORMAT);
        case GL_READ_PIXELS_TYPE: 
                    return GetEnum(L, target, internalformat, pname, DOMAIN_TYPE);
        case GL_TEXTURE_IMAGE_FORMAT:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_FORMAT);
        case GL_TEXTURE_IMAGE_TYPE:
                    return GetEnum(L, target, internalformat, pname, DOMAIN_TYPE);

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
        { "get_internalformat", GetInternalformat },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_getintformat(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


