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
 | Set parameter                                                            |   
 *--------------------------------------------------------------------------*/

static int SetInt(lua_State *L, GLuint texture, GLenum target, GLenum pname, int arg)
    {
    GLint param = luaL_checkinteger(L, arg);
    if(texture==0)
        glTexParameteri(target, pname, param);
    else 
        glTextureParameteri(texture, pname, param);
    CheckError(L);
    return 0;
    }

static int SetFloat(lua_State *L, GLuint texture, GLenum target, GLenum pname, int arg)
    {
    GLfloat param = luaL_checkinteger(L, arg);
    if(texture==0)
        glTexParameterf(target, pname, param);
    else 
        glTextureParameterf(texture, pname, param);
    CheckError(L);
    return 0;
    }

static int SetColor(lua_State *L, GLuint texture, GLenum target, GLenum pname, int arg)
    {
    GLfloat param[4];
    checkcolor(L, arg, param);
    if(texture==0)
        glTexParameterfv(target, pname, param);
    else 
        glTextureParameterfv(texture, pname, param);
    CheckError(L);
    return 0;
    }

static int SetEnum(lua_State *L, GLuint texture, GLenum target, GLenum pname, int arg, uint32_t domain) 
    {
    GLint param = enums_check(L, domain, arg);
    if(texture==0)
        glTexParameteri(target, pname, param);
    else 
        glTextureParameteri(texture, pname, param);
    CheckError(L);
    return 0;
    }

static int SetEnum4(lua_State *L, GLuint texture, GLenum target, GLenum pname, int arg, uint32_t domain) 
    {
    GLint param[4];
    param[0] = enums_check(L, domain, arg++);
    param[1] = enums_check(L, domain, arg++);
    param[2] = enums_check(L, domain, arg++);
    param[3] = enums_check(L, domain, arg++);
    if(texture==0)
        glTexParameteriv(target, pname, param);
    else 
        glTextureParameteriv(texture, pname, param);
    CheckError(L);
    return 0;
    }

static int TextureParameter(lua_State *L) 
/* texture_parameter(texture|target, pname, ...) */
    {
    int arg = 1;
    GLenum target, pname;
    GLuint texture = checktargetorname(L, arg++, &target, DOMAIN_TEXTURE_TARGET);
    pname = checktexturepname(L, arg++);
    
    switch(pname)
        {
        case GL_DEPTH_STENCIL_TEXTURE_MODE: 
                return SetEnum(L, texture, target, pname, arg, DOMAIN_DEPTH_STENCIL);
        case GL_TEXTURE_BASE_LEVEL: 
        case GL_TEXTURE_MAX_LEVEL:  return SetInt(L, texture, target, pname, arg);
        case GL_TEXTURE_BORDER_COLOR:   return SetColor(L, texture, target, pname, arg);
        case GL_TEXTURE_COMPARE_FUNC:
                return SetEnum(L, texture, target, pname, arg, DOMAIN_COMPARE_FUNC);
        case GL_TEXTURE_COMPARE_MODE:   
                return SetEnum(L, texture, target, pname, arg, DOMAIN_COMPARE_MODE);
        case GL_TEXTURE_LOD_BIAS:
        case GL_TEXTURE_MIN_LOD:
        case GL_TEXTURE_MAX_LOD:   return SetFloat(L, texture, target, pname, arg);
        case GL_TEXTURE_SWIZZLE_R:
        case GL_TEXTURE_SWIZZLE_G:
        case GL_TEXTURE_SWIZZLE_B:
        case GL_TEXTURE_SWIZZLE_A:   
                return SetEnum(L, texture, target, pname, arg, DOMAIN_RGBA);
        case GL_TEXTURE_SWIZZLE_RGBA:   
                return SetEnum4(L, texture, target, pname, arg, DOMAIN_RGBA);
        case GL_TEXTURE_MIN_FILTER:
                return SetEnum(L, texture, target, pname, arg, DOMAIN_MIN_FILTER);
        case GL_TEXTURE_MAG_FILTER:  
                return SetEnum(L, texture, target, pname, arg, DOMAIN_MAG_FILTER);
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_WRAP_R:  
                return SetEnum(L, texture, target, pname, arg, DOMAIN_WRAP);
        case GL_TEXTURE_MAX_ANISOTROPY:   return SetFloat(L, texture, target, pname, arg); //GL_VERSION_4_6
        default:
            return luaL_error(L, "cannot set parameter '%s'",  luaL_checkstring(L, arg));
        }
    return 0;
    }

/*--------------------------------------------------------------------------*
 | Get parameter                                                            |   
 *--------------------------------------------------------------------------*/

static int GetInt_(lua_State *L, GLuint texture, GLenum target, GLenum pname, int boolean)
#define GetInt(L,texture,target,pname) GetInt_((L),(texture),(target),(pname),0)
#define GetBoolean(L,texture,target,pname) GetInt_((L),(texture),(target),(pname),1)
    {
    GLint param;
    if(texture==0)
        glGetTexParameteriv(target, pname, &param);
    else
        glGetTextureParameteriv(texture, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param); 
    else
        lua_pushinteger(L, param); 
    return 1;
    }

static int GetFloat(lua_State *L, GLuint texture, GLenum target, GLenum pname)
    {
    GLfloat param;
    if(texture==0)
        glGetTexParameterfv(target, pname, &param);
    else
        glGetTextureParameterfv(texture, pname, &param);
    CheckError(L);
    lua_pushnumber(L, param); 
    return 1;
    }

static int GetFloat4(lua_State *L, GLuint texture, GLenum target, GLenum pname)
    {
    GLfloat param[4];
    if(texture==0)
        glGetTexParameterfv(target, pname, param);
    else
        glGetTextureParameterfv(texture, pname, param);
    CheckError(L);
    lua_pushnumber(L, param[0]); 
    lua_pushnumber(L, param[1]); 
    lua_pushnumber(L, param[2]); 
    lua_pushnumber(L, param[3]); 
    return 4;
    }

static int GetEnum(lua_State *L, GLuint texture, GLenum target, GLenum pname, uint32_t domain)
    {
    GLint param;
    if(texture==0)
        glGetTexParameteriv(target, pname, &param);
    else
        glGetTextureParameteriv(texture, pname, &param);
    CheckError(L);
    enums_push(L, domain, param);
    return 1;
    }

static int GetEnum4(lua_State *L, GLuint texture, GLenum target, GLenum pname, uint32_t domain)
    {
    GLint param[4];
    if(texture==0)
        glGetTexParameteriv(target, pname, param);
    else
        glGetTextureParameteriv(texture, pname, param);
    CheckError(L);
    enums_push(L, domain, param[0]);
    enums_push(L, domain, param[1]);
    enums_push(L, domain, param[2]);
    enums_push(L, domain, param[3]);
    return 4;
    }

static int GetTextureParameter(lua_State *L) 
/* get_texture_parameter(texture|target, pname) */
    {
    GLenum target, pname;
    GLuint texture = checktargetorname(L, 1, &target, DOMAIN_TEXTURE_TARGET);
    pname = checktexturepname(L, 2);

    switch(pname)
        {
        case GL_DEPTH_STENCIL_TEXTURE_MODE:
                return GetEnum(L, texture, target, pname, DOMAIN_DEPTH_STENCIL);
        case GL_TEXTURE_BASE_LEVEL: 
        case GL_TEXTURE_MAX_LEVEL:  return GetInt(L, texture, target, pname);

        case GL_TEXTURE_BORDER_COLOR:   return GetFloat4(L, texture, target, pname);
        case GL_TEXTURE_COMPARE_FUNC:
                return GetEnum(L, texture, target, pname, DOMAIN_COMPARE_FUNC);
        case GL_TEXTURE_COMPARE_MODE:
                return GetEnum(L, texture, target, pname, DOMAIN_COMPARE_MODE);
        case GL_TEXTURE_LOD_BIAS:
        case GL_TEXTURE_MIN_LOD:
        case GL_TEXTURE_MAX_LOD:    return GetFloat(L, texture, target, pname);
        case GL_TEXTURE_SWIZZLE_R:
        case GL_TEXTURE_SWIZZLE_G:
        case GL_TEXTURE_SWIZZLE_B:
        case GL_TEXTURE_SWIZZLE_A:  
                return GetEnum(L, texture, target, pname, DOMAIN_RGBA);
        case GL_TEXTURE_SWIZZLE_RGBA: 
            return GetEnum4(L, texture, target, pname, DOMAIN_RGBA);

        case GL_TEXTURE_MIN_FILTER:
                return GetEnum(L, texture, target, pname, DOMAIN_MIN_FILTER);
        case GL_TEXTURE_MAG_FILTER:
                return GetEnum(L, texture, target, pname, DOMAIN_MAG_FILTER);

        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_WRAP_R:
                return GetEnum(L, texture, target, pname, DOMAIN_WRAP);
        case GL_IMAGE_FORMAT_COMPATIBILITY_TYPE:
                return GetEnum(L, texture, target, pname, DOMAIN_IMAGE_FORMAT_COMPATIBILITY);
        case GL_TEXTURE_IMMUTABLE_FORMAT:   return GetBoolean(L, texture, target, pname); 
        case GL_TEXTURE_IMMUTABLE_LEVELS:
        case GL_TEXTURE_VIEW_MIN_LEVEL:
        case GL_TEXTURE_VIEW_NUM_LEVELS:
        case GL_TEXTURE_VIEW_MIN_LAYER:
        case GL_TEXTURE_VIEW_NUM_LAYERS:    return GetInt(L, texture, target, pname);
        case GL_TEXTURE_TARGET:     return GetInt(L, texture, target, pname);
        case GL_TEXTURE_MAX_ANISOTROPY:    return GetFloat(L, texture, target, pname); //GL_VERSION_4_6
        default:
            {
            const char *name = luaL_checkstring(L, 2);
            return luaL_error(L, "cannot get parameter '%s'", name);
            }
        }
    return 0;
    }

/*--------------------------------------------------------------------------*
 | Get level parameter                                                      |   
 *--------------------------------------------------------------------------*/

static int LGetInt_(lua_State *L, GLuint texture, GLenum target, GLint level, GLenum pname, int boolean)
#define LGetInt(L,texture,target,level,pname) LGetInt_((L),(texture),(target),(level),(pname),0)
#define LGetBoolean(L,texture,target,level,pname) LGetInt_((L),(texture),(target),(level),(pname),1)
    {
    GLint param;
    if(texture==0)
        glGetTexLevelParameteriv(target, level, pname, &param);
    else
        glGetTextureLevelParameteriv(texture, level, pname, &param);
    CheckError(L);
    if(boolean)
        lua_pushboolean(L, param); 
    else
        lua_pushinteger(L, param); 
    return 1;
    }

static int LGetEnum(lua_State *L, GLuint texture, GLenum target, GLint level, GLenum pname, uint32_t domain)
    {
    GLint param;
    if(texture==0)
        glGetTexLevelParameteriv(target, level, pname, &param);
    else
        glGetTextureLevelParameteriv(texture, level, pname, &param);
    CheckError(L);
    enums_push(L, domain, param);
    return 1;
    }

//void glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);
//void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
//void glGetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat *params);
//void glGetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint *params);
static int GetTextureLevelParameter(lua_State *L)
    {
    GLenum target;
    GLuint texture = checktargetorname(L, 1, &target, DOMAIN_TEXTURE_TARGET);
    GLint level = luaL_checkinteger(L, 2);
    GLenum pname = checklevelpname(L, 3);
    switch(pname)
        {
        case GL_TEXTURE_COMPRESSED:
        case GL_TEXTURE_FIXED_SAMPLE_LOCATIONS:  return LGetBoolean(L,texture,target,level,pname);
        case GL_TEXTURE_WIDTH:
        case GL_TEXTURE_HEIGHT:
        case GL_TEXTURE_DEPTH:
        case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
        case GL_TEXTURE_SAMPLES:
        case GL_TEXTURE_SHARED_SIZE:
        case GL_TEXTURE_BUFFER_OFFSET:
        case GL_TEXTURE_BUFFER_SIZE:
        case GL_TEXTURE_RED_SIZE:
        case GL_TEXTURE_GREEN_SIZE:
        case GL_TEXTURE_BLUE_SIZE:
        case GL_TEXTURE_ALPHA_SIZE:
        case GL_TEXTURE_DEPTH_SIZE: return LGetInt(L,texture,target,level,pname);
        case GL_TEXTURE_RED_TYPE:
        case GL_TEXTURE_GREEN_TYPE:
        case GL_TEXTURE_BLUE_TYPE:
        case GL_TEXTURE_ALPHA_TYPE:
        case GL_TEXTURE_DEPTH_TYPE: 
                return LGetEnum(L, texture, target, level, pname, DOMAIN_COMPONENT_TYPE);
        case GL_TEXTURE_INTERNAL_FORMAT:  
                    return LGetEnum(L, texture, target, level, pname, DOMAIN_INTERNAL_FORMAT);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


/*--------------------------------------------------------------------------*
 | Gen, bind etc                                                            |
 *--------------------------------------------------------------------------*/

NEW_TARGET_FUNC(Texture, OTYPE_TEXTURE, checktexturetarget)
GEN_FUNC(Texture, OTYPE_TEXTURE)
BIND_TARGET_FUNC(Texture, checktexturetarget)
DELETE_FUNC(Texture, OTYPE_TEXTURE)
IS_FUNC(Texture)
BINDN_FUNC(Texture)
UINT2_FUNC(BindTextureUnit)

static int CreateTextures(lua_State *L)
    {
    GLuint* names;
    GLsizei i;
    GLenum target = checktexturetarget(L, 1);
    GLsizei n = luaL_optinteger(L, 2, 1);
    check_init_called(L);
    luaL_checkstack(L, n, NULL);
    names = (GLuint*)Malloc(L, n*sizeof(GLuint));
    glCreateTextures(target, n, names);
    CheckErrorFree(L, names);
    for(i = 0; i < n; i++)
        {
        object_new(L, OTYPE_QUERY, names[i]);
        lua_pushinteger(L, names[i]);
        }
    Free(L, names);
    return n;
    }


static int ActiveTexture(lua_State *L)
    {
    GLuint n = luaL_checkinteger(L, 1);
    GLenum texture;
    switch(n)
        {
#define C(i) case i: texture = GL_TEXTURE##i; break
        C(0); C(1); C(2); C(3); C(4); C(5); C(6); C(7); C(8); C(9); 
        C(10); C(11); C(12); C(13); C(14); C(15); C(16); C(17); C(18); C(19); C(20); 
        C(21); C(22); C(23); C(24); C(25); C(26); C(27); C(28); C(29); C(30); C(31); 
#undef C
        default: return luaL_argerror(L, 1, "out of range");
        }
    glActiveTexture(texture);
    CheckError(L);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_texture", NewTexture },
        { "gen_textures", GenTextures },
        { "bind_texture", BindTexture },
        { "bind_textures", BindTextures },
        { "is_texture", IsTexture },
        { "create_textures", CreateTextures },
        { "delete_textures", DeleteTextures },
        { "bind_texture_unit", BindTextureUnit },
        { "active_texture", ActiveTexture },
        { "texture_parameter", TextureParameter },
        { "get_texture_parameter", GetTextureParameter },
        { "get_texture_level_parameter", GetTextureLevelParameter },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_texture(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


