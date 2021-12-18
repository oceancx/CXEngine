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

static int PointParameter(lua_State *L)
    {
    GLenum pname = checkpointpname(L, 1);
    switch(pname)
        {
        case GL_POINT_FADE_THRESHOLD_SIZE: glPointParameterf(pname, luaL_checknumber(L, 2));
                                            break;
        case GL_POINT_SPRITE_COORD_ORIGIN: glPointParameteri(pname, checkorigin(L, 2)); 
                                            break;
        default: 
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

FLOAT_FUNC(PointSize)
FLOAT_FUNC(LineWidth)
FLOAT_FUNC(MinSampleShading)

static int GetMultisample(lua_State *L)
    {
    GLenum pname = checkmultisamplepname(L, 1);
    switch(pname)
        {
        case GL_SAMPLE_POSITION:
                {
                GLfloat val[2];
                GLuint index = luaL_checkinteger(L, 2);
                glGetMultisamplefv(pname,index,val);
                CheckError(L);
                lua_pushnumber(L, val[0]);
                lua_pushnumber(L, val[1]);
                return 2;
                }
        default: 
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


static int CullFace(lua_State *L)
    {
    GLenum mode = checkcullfacemode(L, 1);
    glCullFace(mode);
    CheckError(L);
    return 0;
    }

static int FrontFace(lua_State *L)
    {
    GLenum mode = checkfrontfacemode(L, 1);
    glFrontFace(mode);
    CheckError(L);
    return 0;
    }

static int PolygonMode(lua_State *L)
    {
    GLenum face = checkpolygonmodeface(L, 1);
    GLenum mode = checkpolygonmodemode(L, 2);
    glPolygonMode(face, mode);
    CheckError(L);
    return 0;
    }

FLOAT2_FUNC(PolygonOffset)
FLOAT3_FUNC(PolygonOffsetClamp)

static const struct luaL_Reg Functions[] = 
    {
        { "min_sample_shading", MinSampleShading },
        { "point_parameter", PointParameter },
        { "point_size", PointSize },
        { "line_width", LineWidth },
        { "get_multisample", GetMultisample },
        { "cull_face", CullFace },
        { "front_face", FrontFace },
        { "polygon_mode", PolygonMode },
        { "polygon_offset", PolygonOffset },
        { "polygon_offset_clamp", PolygonOffsetClamp }, //GL_VERSION_4_6
        { NULL, NULL } /* sentinel */
    };

void moongl_open_raster(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

