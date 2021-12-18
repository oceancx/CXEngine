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

NEW_TARGET_FUNC(TransformFeedback, OTYPE_TRANSFORM_FEEDBACK, checktransformtarget)
GEN_FUNC(TransformFeedback, OTYPE_TRANSFORM_FEEDBACK)
BIND_TARGET_FUNC(TransformFeedback, checktransformtarget)
DELETE_FUNC(TransformFeedback, OTYPE_TRANSFORM_FEEDBACK)
IS_FUNC(TransformFeedback)
CREATE_FUNC(TransformFeedback, OTYPE_TRANSFORM_FEEDBACK)


static int BeginTransformFeedback(lua_State *L)
    {
    GLenum primitiveMode = checkprimmode(L, 1);
    glBeginTransformFeedback(primitiveMode);
    CheckError(L);
    return 0;
    }

VOID_FUNC(EndTransformFeedback)
VOID_FUNC(PauseTransformFeedback)
VOID_FUNC(ResumeTransformFeedback)

static int TransformFeedbackBufferBase(lua_State *L)
    {
    GLuint xfb = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint buffer = luaL_checkinteger(L, 3);
    glTransformFeedbackBufferBase(xfb, index, buffer);
    CheckError(L);
    return 0;
    }

static int TransformFeedbackBufferRange(lua_State *L)
    {
    GLuint xfb = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint buffer = luaL_checkinteger(L, 3);
    GLintptr offset = luaL_checkinteger(L, 4);
    GLsizei size = luaL_checkinteger(L, 5);
    glTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
    CheckError(L);
    return 0;
    }


static int DrawTransformFeedback(lua_State *L)
    {
    GLsizei instancecount;
    GLenum mode = checkdrawmode(L, 1);
    GLuint id = luaL_checkinteger(L, 2);
    if(lua_isnoneornil(L, 3))
        glDrawTransformFeedback(mode, id);
    else
        {
        instancecount = luaL_checkinteger(L, 3);
        glDrawTransformFeedbackInstanced(mode, id, instancecount);
        }
    CheckError(L);
    return 0;
    }

static int DrawTransformFeedbackStream(lua_State *L)
    {
    GLsizei instancecount;
    GLenum mode = checkdrawmode(L, 1);
    GLuint id = luaL_checkinteger(L, 2);
    GLuint stream = luaL_checkinteger(L, 3);
    if(lua_isnoneornil(L, 4))
        glDrawTransformFeedbackStream(mode, id, stream);
    else
        {
        instancecount = luaL_checkinteger(L, 4);
        glDrawTransformFeedbackStreamInstanced(mode, id, stream, instancecount);
        }
    CheckError(L);
    return 0;
    }


static int TransformFeedbackVaryings(lua_State *L)
/* transform_feedback_varyings(program, mode, varying1 [, varying2, ...])
 */
    {
    GLsizei count, arg, i;
    const GLchar **varyings;
    GLuint program = luaL_checkinteger(L, 1);
    GLenum mode = checkbuffermode(L, 2);
    arg = 3;
    while(!lua_isnoneornil(L, arg))
        luaL_checkstring(L, arg++);
    if(arg == 3)
        luaL_checkstring(L, arg); /* raise an error */
    count = arg - 3;
    varyings = (const GLchar**)Malloc(L, count*sizeof(GLchar*));
    arg = 3;
    for(i=0; i<count; i++)
        varyings[i] = lua_tostring(L, arg++);
    glTransformFeedbackVaryings(program, count, varyings, mode);
    Free(L, varyings);  
    CheckError(L);
    return 0;
    }

static int GetTransformFeedbackVarying(lua_State *L)
/* get_transform_feedback_varying(program, index)
 * -> name, type, size
 */
    {
    GLchar *name;
    GLenum type;
    GLsizei bufsz, length, size;
    GLuint program = luaL_checkinteger(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    bufsz = programGet(L, program, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH);
    name = (GLchar*)Malloc(L, bufsz*sizeof(GLchar));
    glGetTransformFeedbackVarying(program, index, bufsz, &length, &size, &type, name);
    CheckErrorFree(L, name);
    if(length == 0)
        { Free(L, name); return 0; }
    lua_pushstring(L, name);
    pushglsltype(L, type);
    lua_pushinteger(L, size);
    Free(L, name);
    return 3;
    }

/*------------------------------------------------------------------------------*
 | GetTransformFeedback                                                         |
 *------------------------------------------------------------------------------*/

static int GetTransformFeedback(lua_State *L)
    {
    GLuint index;
    GLint param;
    GLint64 param64;
    GLuint xfb = luaL_checkinteger(L, 1);
    GLenum pname = checktransformpname(L, 2);
    switch(pname)
        {
        case GL_TRANSFORM_FEEDBACK_PAUSED:
        case GL_TRANSFORM_FEEDBACK_ACTIVE:
                    glGetTransformFeedbackiv(xfb, pname, &param);
                    CheckError(L);
                    lua_pushboolean(L, param);
                    return 1;
        case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
                    index = luaL_checkinteger(L, 3);
                    glGetTransformFeedbacki_v(xfb, pname, index, &param);
                    CheckError(L);
                    lua_pushinteger(L, param);
                    return 1;
        case GL_TRANSFORM_FEEDBACK_BUFFER_START:
        case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
                    index = luaL_checkinteger(L, 3);
                    glGetTransformFeedbacki64_v(xfb, pname, index, &param64);
                    CheckError(L);
                    lua_pushinteger(L, param64);
                    return 1;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Viewport                                                                     |
 *------------------------------------------------------------------------------*/

static int ProvokingVertex(lua_State *L)
    {
    GLenum mode = checkprovokemode(L, 1);
    glProvokingVertex(mode);
    CheckError(L);
    return 0;
    }


static int ClipControl(lua_State *L)
    {
    GLenum origin = checkorigin(L, 1);
    GLenum depth = checkdepth(L, 2);
    glClipControl(origin, depth);
    CheckError(L);
    return 0;
    }

static int DepthRange(lua_State *L)
    {
    GLuint index;
    GLdouble near = luaL_checknumber(L, 1);
    GLdouble far = luaL_checknumber(L, 2);
    if(lua_isnoneornil(L, 3))
        glDepthRange(near, far);
    else
        {
        index = luaL_checkinteger(L, 3);
        glDepthRangeIndexed(index, near, far);
        }
    CheckError(L);
    return 0;
    }

static int DepthRangeArray(lua_State *L)
    {
    GLdouble *v;
    GLsizei count, i, arg;
    GLuint first = luaL_checkinteger(L, 1);
    arg=2;
    while(!lua_isnoneornil(L, arg)) 
        luaL_checknumber(L, arg++);
    count = arg - 2;    
    if((count==0) || ((count % 2) != 0))
        return luaL_error(L, NARGS_ERROR);
    v = (GLdouble*)Malloc(L, count*sizeof(GLdouble));
    arg=2;
    for(i = 0; i < count; i++)
        v[i] = lua_tonumber(L, arg++);
    glDepthRangeArrayv(first, count/2, v);
    Free(L, v);
    CheckError(L);
    return 0;
    }


static int Viewport(lua_State *L)
    {
    GLuint index;
    GLfloat  v[4];
    GLint x, y;
    GLsizei w, h;
    if(lua_isnoneornil(L, 5))
        {
        x = luaL_checkinteger(L, 1);
        y = luaL_checkinteger(L, 2);
        w = luaL_checkinteger(L, 3);
        h = luaL_checkinteger(L, 4);
        glViewport(x, y, w, h);
        }
    else
        {
        index = luaL_checkinteger(L, 5);
        v[0] = luaL_checknumber(L, 1);
        v[1] = luaL_checknumber(L, 2);
        v[2] = luaL_checknumber(L, 3);
        v[3] = luaL_checknumber(L, 4);
        glViewportIndexedfv(index, v);
        }
    CheckError(L);
    return 0;
    }

static int ViewportArray(lua_State *L)
    {
    GLfloat *v;
    GLsizei count, i, arg;
    GLuint first = luaL_checkinteger(L, 1);
    arg=2;
    count=0;
    while(lua_isnumber(L,arg++)) count++;
    if((count==0) || ((count % 4) != 0))
        return luaL_error(L, NARGS_ERROR);
    v = (GLfloat*)Malloc(L, count*sizeof(GLfloat));
    i=0; arg=2;
    while(i<count)
        v[i++] = lua_tonumber(L, arg++);
    glViewportArrayv(first, count/4, v);
    Free(L, v);
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_transform_feedback", NewTransformFeedback },
        { "gen_transform_feedbacks", GenTransformFeedbacks },
        { "create_transform_feedbacks", CreateTransformFeedbacks },
        { "bind_transform_feedback", BindTransformFeedback },
        { "is_transform_feedback", IsTransformFeedback },
        { "delete_transform_feedbacks", DeleteTransformFeedbacks },
        { "begin_transform_feedback", BeginTransformFeedback },
        { "end_transform_feedback", EndTransformFeedback },
        { "pause_transform_feedback", PauseTransformFeedback },
        { "resume_transform_feedback", ResumeTransformFeedback },
        { "transform_feedback_buffer_base", TransformFeedbackBufferBase },
        { "transform_feedback_buffer_range", TransformFeedbackBufferRange },
        { "draw_transform_feedback", DrawTransformFeedback },
        { "draw_transform_feedback_stream", DrawTransformFeedbackStream },
        { "transform_feedback_varyings", TransformFeedbackVaryings },
        { "get_transform_feedback_varying", GetTransformFeedbackVarying },
        { "get_transform_feedback", GetTransformFeedback },
        { "provoking_vertex", ProvokingVertex },
        { "clip_control", ClipControl },
        { "depth_range", DepthRange },
        { "depth_range_array", DepthRangeArray },
        { "viewport", Viewport },
        { "viewport_array", ViewportArray },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_transform(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


