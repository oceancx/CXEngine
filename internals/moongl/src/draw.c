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

static int DrawArrays(lua_State *L)
/* draw_arrays(mode, first, count [, instancecount [, baseinstance]])
 */
    {
    GLsizei instancecount;
    GLuint baseinstance;
    GLenum mode = checkdrawmode(L, 1);
    GLint first = luaL_checkinteger(L, 2);
    GLsizei count = luaL_checkinteger(L, 3);
    if(lua_isnoneornil(L, 4))
        {
        glDrawArrays(mode, first, count);
        CheckError(L);
        return 0;
        }
    instancecount = luaL_checkinteger(L, 4);
    if(lua_isnoneornil(L, 5))
        {
        glDrawArraysInstanced(mode, first, count, instancecount);
        CheckError(L);
        return 0;
        }
    baseinstance = luaL_checkinteger(L, 5);
    glDrawArraysInstancedBaseInstance(mode, first, count, instancecount, baseinstance);
    CheckError(L);
    return 0;
    }

static int DrawArraysIndirect(lua_State *L)
/* draw_arrays_indirect(mode, indirect)
 */
    {
    GLenum mode = checkdrawmode(L, 1);
    GLintptr indirect = luaL_checkinteger(L, 2);
    glDrawArraysIndirect(mode, (const void *)indirect);
    CheckError(L);
    return 0;
    }


static int MultiDrawArrays(lua_State *L)
/* multi_draw_arrays(mode, first1, count1 [, first2, count2, ...]) */
    {
    GLsizei arg, i, drawcount;
    GLint *first;
    GLsizei *count;
    GLenum mode = checkdrawmode(L, 1);
    arg = 2;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    drawcount = arg - 2;
    if((arg<4) || (drawcount%2 != 0))
        return luaL_error(L, NARGS_ERROR);
    drawcount /= 2;
    first = (GLint*)Malloc2(L, drawcount*sizeof(GLint), (void**)&count, drawcount*sizeof(GLsizei));
    arg = 2;
    for(i=0; i < drawcount; i++)
        {
        first[i] = lua_tointeger(L, arg++);
        count[i] = lua_tointeger(L, arg++);
        }
    glMultiDrawArrays(mode, first, count, drawcount);
    Free(L, first); 
    Free(L, count); 
    CheckError(L);
    return 0;
    }

static int MultiDrawArraysIndirect(lua_State *L)
/* multi_draw_arrays_indirect(mode, indirect, drawcount, stride)
 */
    {
    GLenum mode = checkdrawmode(L, 1);
    GLintptr indirect = luaL_checkinteger(L, 2);
    GLsizei drawcount = luaL_checkinteger(L, 3);
    GLsizei stride = luaL_checkinteger(L, 4);
    glMultiDrawArraysIndirect(mode, (const void *)indirect, drawcount, stride);
    CheckError(L);
    return 0;
    }

static int MultiDrawArraysIndirectCount(lua_State *L) //GL_VERSION_4_6
    {
    GLenum mode = checkdrawmode(L, 1);
    GLintptr indirect = luaL_checkinteger(L, 2);
    GLintptr drawcount = luaL_checkinteger(L, 3);
    GLsizei maxdrawcount = luaL_checkinteger(L, 4);
    GLsizei stride = luaL_checkinteger(L, 5);
    glMultiDrawArraysIndirectCount(mode, (void*)indirect, drawcount, maxdrawcount, stride);
    CheckError(L);
    return 0;
    }

static int DrawElements(lua_State *L)
/* draw_elements(mode, count, type, indices [, instancecount [, baseinstance]])
 */
    {
    GLsizei instancecount;
    GLuint baseinstance;
    GLenum mode = checkdrawmode(L, 1);
    GLsizei count = luaL_checkinteger(L, 2);
    GLenum type = checkdrawtype(L, 3);
    GLintptr indices = luaL_checkinteger(L, 4);
    if(lua_isnoneornil(L, 5))
        {
        glDrawElements(mode, count, type, (const void*)indices);
        CheckError(L);
        return 0;
        }
    instancecount = luaL_checkinteger(L, 5);
    if(lua_isnoneornil(L, 6))
        {
        glDrawElementsInstanced(mode, count, type, (const void*)indices, instancecount);
        CheckError(L);
        return 0;
        }
    baseinstance = luaL_checkinteger(L, 6);
    glDrawElementsInstancedBaseInstance(mode, count, type, (const void*)indices, 
                            instancecount, baseinstance);
    CheckError(L);
    return 0;
    }


static int DrawElementsIndirect(lua_State *L)
/* draw_elements_indirect(mode, type, indirect)
 */
    {
    GLenum mode = checkdrawmode(L, 1);
    GLenum type = checkdrawtype(L, 2);
    GLintptr indirect = luaL_checkinteger(L, 3);
    glDrawElementsIndirect(mode, type, (const void *)indirect);
    CheckError(L);
    return 0;
    }



static int MultiDrawElements(lua_State *L)
/* multi_draw_elements(mode, type, count1, indices1 [, count2, indices2, ...]) 
 */
    {
    GLsizei *count, drawcount, i, arg;
    GLintptr *indices;
    GLenum mode = checkdrawmode(L, 1);
    GLenum type = checkdrawtype(L, 2);
    arg = 3;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    drawcount = arg - 3;
    if(((arg<4) || (drawcount%2) != 0))
        return luaL_error(L, NARGS_ERROR);
    drawcount /= 2;
    indices = (GLintptr*)Malloc2(L, drawcount*sizeof(GLintptr), 
                    (void**)&count, drawcount*sizeof(GLsizei));
    arg = 3;
    for(i=0; i<drawcount; i++)
        {
        indices[i] = lua_tointeger(L, arg++);
        count[i] = lua_tointeger(L, arg++);
        }
    glMultiDrawElements(mode, count, type, (const void**)indices, drawcount);
    Free(L, indices); 
    Free(L, count);
    CheckError(L);
    return 0;
    }

static int MultiDrawElementsIndirect(lua_State *L)
/* draw_elements_indirect(mode, type, indirect, drawcount, stride)
 */
    {
    GLenum mode = checkdrawmode(L, 1);
    GLenum type = checkdrawtype(L, 2);
    GLintptr indirect = luaL_checkinteger(L, 3);
    GLsizei drawcount = luaL_checkinteger(L, 4);
    GLsizei stride = luaL_checkinteger(L, 5);
    glMultiDrawElementsIndirect(mode, type, (const void *)indirect, drawcount, stride);
    CheckError(L);
    return 0;
    }

static int MultiDrawElementsIndirectCount(lua_State *L) //GL_VERSION_4_6
    {
    GLenum mode = checkdrawmode(L, 1);
    GLenum type = checkdrawtype(L, 2);
    GLintptr indirect = luaL_checkinteger(L, 3);
    GLintptr drawcount = luaL_checkinteger(L, 4);
    GLsizei maxdrawcount = luaL_checkinteger(L, 5);
    GLsizei stride = luaL_checkinteger(L, 6);
    glMultiDrawElementsIndirectCount(mode, type, (const void *)indirect, drawcount, maxdrawcount, stride);
    CheckError(L);
    return 0;
    }

static int DrawElementsBaseVertex(lua_State *L)
/* draw_elements_base_vertex(mode,count, type, indices, basevertex [, instancecount, baseinstance]) 
 */
    {
    GLsizei instancecount;
    GLuint baseinstance;
    GLenum mode = checkdrawmode(L, 1);
    GLsizei count = luaL_checkinteger(L, 2);
    GLenum type = checkdrawtype(L, 3);
    GLintptr indices = luaL_checkinteger(L, 4);
    GLsizei basevertex = luaL_checkinteger(L, 5);
    if(lua_isnoneornil(L, 6))
        {
        glDrawElementsBaseVertex(mode, count, type, (void*)indices, basevertex);
        CheckError(L);
        return 0;
        }
    instancecount = luaL_checkinteger(L, 6);
    if(lua_isnoneornil(L, 7))
        {
        glDrawElementsInstancedBaseVertex(mode, count, type, (void*)indices,
                instancecount, basevertex);
        CheckError(L);
        return 0;
        }
    baseinstance = luaL_checkinteger(L, 7);
    glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, (void*)indices,
            instancecount, basevertex, baseinstance);
    CheckError(L);
    return 0;
    }

static int MultiDrawElementsBaseVertex(lua_State *L)
/* multi_draw_elements_base_vertex
 *  (mode, type, count1, indices1, basevertex1, [, count2, indices2, basevertex2, ...]) 
 */
    {
    GLsizei *count, drawcount, i, arg;
    GLintptr *indices;
    GLint *basevertex;
    GLenum mode = checkdrawmode(L, 1);
    GLenum type = checkdrawtype(L, 2);
    arg = 3;
    while(!lua_isnoneornil(L, arg))
        luaL_checkinteger(L, arg++);
    drawcount = arg - 3;
    if(((arg<5) || (drawcount%3) != 0))
        return luaL_error(L, NARGS_ERROR);
    drawcount /= 3;
    indices = (GLintptr*)Malloc3(L, drawcount*sizeof(GLintptr), 
                    (void**)&count, drawcount*sizeof(GLsizei),
                    (void**)&basevertex, drawcount*sizeof(GLint));
    arg = 3;
    for(i=0; i<drawcount; i++)
        {
        indices[i] = lua_tointeger(L, arg++);
        count[i] = lua_tointeger(L, arg++);
        basevertex[i] = lua_tointeger(L, arg++);
        }
    glMultiDrawElementsBaseVertex(mode, count, type, (const void* const*)indices, drawcount,basevertex);
    Free(L, indices); 
    Free(L, count);
    Free(L, basevertex);
    CheckError(L);
    return 0;
    }


static int DrawRangeElements(lua_State *L)
    {
    GLenum mode = checkdrawmode(L, 1);
    GLuint start = luaL_checkinteger(L, 2);
    GLuint end = luaL_checkinteger(L, 3);
    GLsizei count = luaL_checkinteger(L, 4);
    GLenum type = checkdrawtype(L, 5);
    GLintptr indices = luaL_checkinteger(L, 6);
    glDrawRangeElements(mode, start, end, count, type, (void*)indices);
    CheckError(L);
    return 0;
    }

static int DrawRangeElementsBaseVertex(lua_State *L)
    {
    GLenum mode = checkdrawmode(L, 1);
    GLuint start = luaL_checkinteger(L, 2);
    GLuint end = luaL_checkinteger(L, 3);
    GLsizei count = luaL_checkinteger(L, 4);
    GLenum type = checkdrawtype(L, 5);
    GLintptr indices = luaL_checkinteger(L, 6);
    GLint basevertex = luaL_checkinteger(L, 7);
    glDrawRangeElementsBaseVertex(mode, start, end, count, type, (void*)indices, basevertex);
    CheckError(L);
    return 0;
    }


static int PrimitiveRestartIndex(lua_State *L)
    {
    GLuint index = luaL_checkinteger(L, 1);
    glPrimitiveRestartIndex(index);
    CheckError(L);
    return 0;
    }

static int BeginConditionalRender(lua_State *L)
    {
    GLuint id = luaL_checkinteger(L, 1);
    GLenum mode = checkconditionalrendermode(L, 2);
    glBeginConditionalRender(id, mode);
    CheckError(L);
    return 0;
    }

VOID_FUNC(EndConditionalRender)

VOID_FUNC(Flush)
VOID_FUNC(Finish)

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "flush", Flush  },
        { "finish", Finish },
        { "draw_arrays", DrawArrays },
        { "draw_arrays_indirect", DrawArraysIndirect },
        { "multi_draw_arrays", MultiDrawArrays },
        { "multi_draw_arrays_indirect", MultiDrawArraysIndirect },
        { "multi_draw_arrays_indirect_count", MultiDrawArraysIndirectCount },
        { "draw_elements", DrawElements },
        { "draw_elements_indirect", DrawElementsIndirect },
        { "multi_draw_elements", MultiDrawElements },
        { "multi_draw_elements_indirect", MultiDrawElementsIndirect },
        { "multi_draw_elements_indirect_count", MultiDrawElementsIndirectCount },
        { "draw_elements_base_vertex", DrawElementsBaseVertex },
        { "multi_draw_elements_base_vertex", MultiDrawElementsBaseVertex },
        { "draw_range_elements", DrawRangeElements },
        { "draw_range_elements_base_vertex", DrawRangeElementsBaseVertex  },
        { "primitive_restart_index", PrimitiveRestartIndex },
        { "begin_conditional_render", BeginConditionalRender },
        { "end_conditional_render", EndConditionalRender },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_draw(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

