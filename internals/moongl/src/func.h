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

/*------------------------------------------------------------------------------*
 | Malloc, Free, CheckError                                                     |
 *------------------------------------------------------------------------------*/

#define Malloc moongl_Malloc
void *Malloc(lua_State *L, size_t size);
#define Malloc2 moongl_Malloc2
void* Malloc2(lua_State *L, size_t size, void **ptr2, size_t size2);
#define Malloc3 moongl_Malloc3
void* Malloc3(lua_State *L, size_t size, void **ptr2, size_t size2, void **ptr3, size_t size3);
#define MallocNoErr moongl_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moongl_Strdup
char *Strdup(lua_State *L, const char *s);
#define Free moongl_Free
void Free(lua_State *L, void *ptr);
#define CheckError moongl_CheckError
int CheckError(lua_State *L);
#define CheckErrorp moongl_CheckErrorp
void *CheckErrorp(lua_State *L);
#define CheckErrorFree moongl_CheckErrorFree
int CheckErrorFree(lua_State *L, void *ptr);
#define CheckErrorFree2 moongl_CheckErrorFree2
int CheckErrorFree2(lua_State *L, void *ptr1, void *ptr2);
#define CheckErrorFree3 moongl_CheckErrorFree3
int CheckErrorFree3(lua_State *L, void *ptr1, void *ptr2, void *ptr3);

/*------------------------------------------------------------------------------*
 | Gen, Bind & C                                                                |
 *------------------------------------------------------------------------------*/

#define NEW_FUNC(what, otype)  /* glGen() + glBind() */                 \
    static int New##what(lua_State *L)                                  \
    {                                                                   \
    GLuint name;                                                        \
    check_init_called(L);                                               \
    glGen##what##s(1, &name);                                           \
    CheckError(L);                                                      \
    object_new(L, otype, name);                                         \
    glBind##what(name);                                                 \
    CheckError(L);                                                      \
    lua_pushinteger(L, name);                                           \
    return 1;                                                           \
    }

#define NEW_TARGET_FUNC(what, otype, checkxxx)    /* glGen() + glBind() */  \
    static int New##what(lua_State *L)                                      \
    {                                                                       \
    GLuint name;                                                            \
    GLenum target = checkxxx(L, 1);                                         \
    check_init_called(L);                                                   \
    glGen##what##s(1, &name);                                               \
    CheckError(L);                                                          \
    object_new(L, otype, name);                                             \
    glBind##what(target, name);                                             \
    CheckError(L);                                                          \
    lua_pushinteger(L, name);                                               \
    return 1;                                                               \
    }

#define GEN_FUNC(what, otype)                                       \
    static int Gen##what##s(lua_State *L)                           \
    {                                                               \
    int i;                                                          \
    GLuint* names;                                                  \
    GLsizei n = luaL_optinteger(L, 1, 1); /* defaults to a single name */   \
    if(n <= 0)                                                      \
        return luaL_argerror(L, 1, "positive number expected");     \
    check_init_called(L);                                           \
    luaL_checkstack(L, n, NULL);                                    \
    names = (GLuint*)Malloc(L, n*sizeof(GLuint));                   \
    glGen##what##s(n, names);                                       \
    CheckErrorFree(L, names);                                       \
    for(i = 0; i < n; i++)                                          \
        {                                                           \
        object_new(L, otype, names[i]);                             \
        lua_pushinteger(L, names[i]);                               \
        }                                                           \
    Free(L, names);                                                 \
    return n;                                                       \
    }


#define BIND_FUNC(what)                             \
    static int Bind##what(lua_State *L)             \
    {                                               \
    GLuint name = luaL_optinteger(L, 1, 0);         \
    glBind##what(name);                             \
    CheckError(L);                                  \
    return 0;                                       \
    }

#define BIND_TARGET_FUNC(what, checkxxx)            \
static int Bind##what(lua_State *L)                 \
    {                                               \
    GLenum target = checkxxx(L, 1);                 \
    GLuint name = luaL_optinteger(L, 2, 0);         \
    glBind##what(target, name);                     \
    CheckError(L);                                  \
    return 0;                                       \
    }
    
#define BINDN_FUNC(what)                                        \
static int Bind##what##s(lua_State *L)                          \
    {                                                           \
    int i;                                                      \
    GLuint* names;                                              \
    GLuint first = luaL_checkinteger(L, 1);                     \
    GLsizei n = 2;                                              \
    while(lua_isinteger(L, n)) n++; /* get the number of names */\
    n = n - 2;                                                  \
    if(--n==0)  return luaL_argerror(L, 2, "integer expected"); \
    names = (GLuint*)Malloc(L, n*sizeof(GLuint));               \
    for(i = 0; i < n; i++)                                      \
        names[i] = lua_tointeger(L, i+2);                       \
    glBind##what##s(first, n, names);                           \
    CheckErrorFree(L, names);                                   \
    Free(L, names);                                             \
    return 0;                                                   \
    }

#ifndef GL_VERSION_4_5
#define CREATE_FUNC(what, otype)                                    \
    static int Create##what##s(lua_State *L) { NOT_AVAILABLE; }
#else
#define CREATE_FUNC(what, otype)                                    \
static int Create##what##s(lua_State *L)                            \
    {                                                               \
    int i;                                                          \
    GLuint* names;                                                  \
    GLsizei n = luaL_optinteger(L, 1, 1);                           \
    check_init_called(L);                                           \
    luaL_checkstack(L, n, NULL);                                    \
    names = (GLuint*)Malloc(L, n*sizeof(GLuint));                   \
    glCreate##what##s(n, names);                                    \
    CheckErrorFree(L, names);                                       \
    for(i = 0; i < n; i++)                                          \
        {                                                           \
        object_new(L, otype, names[i]);                             \
        lua_pushinteger(L, names[i]);                               \
        }                                                           \
    Free(L, names);                                                 \
    return n;                                                       \
    }
#endif

#define DELETE_FUNC(what, otype)                                    \
    static int Delete##what##s(lua_State *L)                        \
    {                                                               \
    GLuint name;                                                    \
    int arg = 1;                                                    \
    while(!lua_isnoneornil(L, arg))                                 \
        {                                                           \
        name = luaL_checkinteger(L, arg++);                         \
        object_free(L, otype, name);                                \
        }                                                           \
    return 0;                                                       \
    }

#define IS_FUNC(what)                                               \
    static int Is##what(lua_State *L)                               \
    {                                                               \
    GLuint arg = luaL_checkinteger(L, 1);                           \
    GLboolean res = glIs##what(arg);                                \
    CheckError(L);                                                  \
    lua_pushboolean(L, res);                                        \
    return 1;                                                       \
    }

/*------------------------------------------------------------------------------*
 | void glXxx() generic functions                                               |
 *------------------------------------------------------------------------------*/

#define VOID_FUNC(func) /* void func(void) */ \
static int func(lua_State *L)   \
    {                           \
    gl##func();                 \
    CheckError(L);              \
    return 0;                   \
    }

#define BOOLEAN_FUNC(func)  /* void func(GLboolean) */\
static int func(lua_State *L)               \
    {                                       \
    GLboolean arg = checkboolean(L, 1);     \
    gl##func(arg);                          \
    CheckError(L);                          \
    return 0;                               \
    }

#define INT_FUNC(func)  /* void func(GLint) */  \
static int func(lua_State *L)               \
    {                                       \
    GLint arg = luaL_checkinteger(L, 1);    \
    gl##func(arg);                          \
    CheckError(L);                          \
    return 0;                               \
    }

#define UINT_FUNC(func) /* void func(GLuint) */ \
static int func(lua_State *L)               \
    {                                       \
    GLuint arg = luaL_checkinteger(L, 1);   \
    gl##func(arg);                          \
    CheckError(L);                          \
    return 0;                               \
    }

#define UINT2_FUNC(func) /* void func(GLuint, GLuint) */\
static int func(lua_State *L)               \
    {                                       \
    GLuint arg1 = luaL_checkinteger(L, 1);  \
    GLuint arg2 = luaL_checkinteger(L, 2);  \
    gl##func(arg1, arg2);                   \
    CheckError(L);                          \
    return 0;                               \
    }

#define UINT3_FUNC(func) /* void func(GLuint, GLuint, GLuint) */\
static int func(lua_State *L)               \
    {                                       \
    GLuint arg1 = luaL_checkinteger(L, 1);  \
    GLuint arg2 = luaL_checkinteger(L, 2);  \
    GLuint arg3 = luaL_checkinteger(L, 3);  \
    gl##func(arg1, arg2, arg3);             \
    CheckError(L);                          \
    return 0;                               \
    }


#define FLOAT_FUNC(func) /* void func(GLfloat) */   \
static int func(lua_State *L)               \
    {                                       \
    GLfloat arg = luaL_checknumber(L, 1);   \
    gl##func(arg);                          \
    CheckError(L);                          \
    return 0;                               \
    }

#define FLOAT2_FUNC(func) /* void func(GLfloat, GLfloat) */\
static int func(lua_State *L)               \
    {                                       \
    GLfloat arg1 = luaL_checknumber(L, 1);  \
    GLfloat arg2 = luaL_checknumber(L, 2);  \
    gl##func(arg1, arg2);                   \
    CheckError(L);                          \
    return 0;                               \
    }

#define FLOAT3_FUNC(func) /* void func(GLfloat, GLfloat, GLfloat) */\
static int func(lua_State *L)               \
    {                                       \
    GLfloat arg1 = luaL_checknumber(L, 1);  \
    GLfloat arg2 = luaL_checknumber(L, 2);  \
    GLfloat arg3 = luaL_checknumber(L, 3);  \
    gl##func(arg1, arg2, arg3);             \
    CheckError(L);                          \
    return 0;                               \
    }

#define DOUBLE_FUNC(func) /* void func(GLdouble) */     \
static int func(lua_State *L)               \
    {                                       \
    GLdouble arg = luaL_checknumber(L, 1);  \
    gl##func(arg);                          \
    CheckError(L);                          \
    return 0;                               \
    }

