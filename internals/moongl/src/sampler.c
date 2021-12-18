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

GEN_FUNC(Sampler, OTYPE_SAMPLER)
CREATE_FUNC(Sampler, OTYPE_SAMPLER)
DELETE_FUNC(Sampler, OTYPE_SAMPLER)
IS_FUNC(Sampler)
BINDN_FUNC(Sampler)


static int BindSampler(lua_State *L)
    {
    GLuint unit = luaL_optinteger(L, 1, 0);
    GLuint sampler = luaL_optinteger(L, 2, 0);
    glBindSampler(unit, sampler);
    CheckError(L);
    return 0;
    }

static int NewSampler(lua_State *L)
    {
    GLuint sampler;
    GLuint unit = luaL_optinteger(L, 1, 0);
    check_init_called(L);
    glGenSamplers(1, &sampler);
    CheckError(L);
    object_new(L, OTYPE_SAMPLER, sampler);
    glBindSampler(unit, sampler);
    CheckError(L);
    lua_pushinteger(L, sampler);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Set                                                                          |
 *------------------------------------------------------------------------------*/


static int SetFloat(lua_State *L, GLuint sampler, GLenum pname, int arg)
    {
    GLfloat param = luaL_checknumber(L, arg);
    glSamplerParameterf(sampler, pname, param);
    CheckError(L);
    return 0;
    }

static int SetColor(lua_State *L, GLuint sampler, GLenum pname, int arg)
    {
    GLfloat params[4];
    checkcolor(L, arg, params);
    glSamplerParameterfv(sampler, pname, params);
    CheckError(L);
    return 0;
    }

static int SetEnum(lua_State *L, GLuint sampler, GLenum pname, int arg, uint32_t domain)
    {
    GLenum param = enums_check(L, domain, arg);
    glSamplerParameteri(sampler, pname, param);
    CheckError(L);
    return 0;
    }


static int SamplerParameter(lua_State *L)
    {
    GLuint sampler = luaL_checkinteger(L, 1);
    GLenum pname = checksamplerpname(L, 2);
    switch(pname)
        {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T: 
        case GL_TEXTURE_WRAP_R: return SetEnum(L, sampler, pname, 3, DOMAIN_WRAP);
        case GL_TEXTURE_MIN_FILTER:  return SetEnum(L, sampler, pname, 3, DOMAIN_MIN_FILTER);
        case GL_TEXTURE_MAG_FILTER:  return SetEnum(L, sampler, pname, 3, DOMAIN_MAG_FILTER);
        case GL_TEXTURE_BORDER_COLOR:  return SetColor(L, sampler, pname, 3);
        case GL_TEXTURE_MIN_LOD:  return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_MAX_LOD:  return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_LOD_BIAS: return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_COMPARE_MODE:  return SetEnum(L, sampler, pname, 3, DOMAIN_COMPARE_MODE);
        case GL_TEXTURE_COMPARE_FUNC:  return SetEnum(L, sampler, pname, 3, DOMAIN_COMPARE_FUNC);
        case GL_TEXTURE_MAX_ANISOTROPY:  return SetFloat(L, sampler, pname, 3); //GL_VERSION_4_6
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Get                                                                          |
 *------------------------------------------------------------------------------*/

static int GetEnum(lua_State *L, GLuint sampler, GLenum pname, uint32_t domain)
    {
    GLint param;
    glGetSamplerParameteriv(sampler, pname, &param);
    CheckError(L);
    enums_push(L, domain, param);
    return 1;
    }

static int GetFloat(lua_State *L, GLuint sampler, GLenum pname)
    {
    GLfloat param;
    CheckError(L);
    glGetSamplerParameterfv(sampler, pname, &param);
    lua_pushnumber(L, param);
    return 1;
    }

static int GetFloat4(lua_State *L, GLuint sampler, GLenum pname)
    {
    GLfloat params[4];
    CheckError(L);
    glGetSamplerParameterfv(sampler, pname, params);
    lua_pushnumber(L, params[0]);
    lua_pushnumber(L, params[1]);
    lua_pushnumber(L, params[2]);
    lua_pushnumber(L, params[3]);
    return 4;
    }

static int GetSamplerParameter(lua_State *L)
    {
    GLuint sampler = luaL_checkinteger(L, 1);
    GLenum pname = checksamplerpname(L, 2);
    switch(pname)
        {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_WRAP_R: return GetEnum(L, sampler, pname, DOMAIN_WRAP);
        case GL_TEXTURE_MIN_FILTER:  return GetEnum(L, sampler, pname, DOMAIN_MIN_FILTER);
        case GL_TEXTURE_MAG_FILTER:  return GetEnum(L, sampler, pname, DOMAIN_MAG_FILTER);
        case GL_TEXTURE_BORDER_COLOR:  return GetFloat4(L, sampler, pname);
        case GL_TEXTURE_MIN_LOD:  return GetFloat(L, sampler, pname);
        case GL_TEXTURE_MAX_LOD:  return GetFloat(L, sampler, pname);
        case GL_TEXTURE_LOD_BIAS: return GetFloat(L, sampler, pname);
        case GL_TEXTURE_COMPARE_MODE:  return GetEnum(L, sampler, pname, DOMAIN_COMPARE_MODE);
        case GL_TEXTURE_COMPARE_FUNC: return GetEnum(L, sampler, pname, DOMAIN_COMPARE_FUNC);
        case GL_TEXTURE_MAX_ANISOTROPY:  return GetFloat(L, sampler, pname); //GL_VERSION_4_6
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_sampler", NewSampler },
        { "gen_samplers", GenSamplers },
        { "create_samplers", CreateSamplers },
        { "delete_samplers", DeleteSamplers },
        { "bind_sampler", BindSampler },
        { "bind_samplers", BindSamplers },
        { "is_sampler", IsSampler },
        { "sampler_parameter", SamplerParameter },
        { "get_sampler_parameter", GetSamplerParameter },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_sampler(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

