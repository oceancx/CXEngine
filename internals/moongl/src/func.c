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

/*------------------------------------------------------------------------------*
 | malloc() wrappers                                                            |
 *------------------------------------------------------------------------------*/

void *Malloc(lua_State *L, size_t size)
    {
    void *ptr;
    if(size==0)
        { luaL_error(L, "malloc request with size = 0"); return NULL; }
    if((ptr = malloc(size)) == NULL)
        { luaL_error(L, "cannot allocate memory"); return NULL; }
    return ptr;
    }

void* Malloc2(lua_State *L, size_t size, void **ptr2, size_t size2)
    {
    void *ptr;
    if((size==0) || (size2 == 0))
        { luaL_error(L, "malloc request with size = 0"); return NULL; }
    if((ptr = malloc(size)) == NULL)
        { luaL_error(L, "cannot allocate memory"); return NULL; }
    if((*ptr2 = malloc(size2)) == NULL)
        { Free(L, ptr); luaL_error(L, "cannot allocate memory"); return NULL; }
    return ptr;
    }

void* Malloc3(lua_State *L, size_t size, void **ptr2, size_t size2, void **ptr3, size_t size3)
    {
    void *ptr;
    if((size==0) || (size2 == 0) || (size3 == 0))
        { luaL_error(L, "malloc request with size = 0"); return NULL; }
    if((ptr = malloc(size)) == NULL)
        { luaL_error(L, "cannot allocate memory"); return NULL; }
    if((*ptr2 = malloc(size2)) == NULL)
        { 
        Free(L, ptr); 
        luaL_error(L, "cannot allocate memory"); return NULL; 
        }
    if((*ptr3 = malloc(size3)) == NULL)
        { 
        Free(L, ptr); 
        Free(L, *ptr2); 
        luaL_error(L, "cannot allocate memory"); return NULL; 
        }
    return ptr;
    }

void *MallocNoErr(lua_State *L, size_t size) /* do not raise errors (check the retval) */
    {
    void *ptr = malloc(size);
    (void)L;
    if(ptr==NULL)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
    }

char *Strdup(lua_State *L, const char *s)
    {
    size_t len = strnlen(s, 256);
    char *ptr = (char*)Malloc(L, len + 1);
    if(len>0)
        memcpy(ptr, s, len);
    ptr[len]='\0';
    return ptr;
    }

void Free(lua_State *L, void *ptr)
    {
    (void)L;
    if(ptr) free(ptr);
    }

/*------------------------------------------------------------------------------*
 | glGetError()                                                                 |
 *------------------------------------------------------------------------------*/

static const char* ErrorString(int err) /* replaces gluErrorString */
    {
    switch(err)
        {
        case GL_NO_ERROR: return "no error";
        case GL_INVALID_ENUM: return "invalid enum";
        case GL_INVALID_VALUE: return "invalid value";
        case GL_INVALID_OPERATION: return "invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid framebuffer operation";
        case GL_OUT_OF_MEMORY: return "out of memory";
        case GL_STACK_UNDERFLOW: return "stack underflow";
        case GL_STACK_OVERFLOW: return "stack overflow";
        default:
            return "unknown gl error";
        }
    return NULL; /* unreachable */
    }



int CheckError(lua_State *L)
    {
    int rc = glGetError();
    if(rc != 0) return luaL_error((L), (char*)ErrorString(rc));
    return 0;
    }

void *CheckErrorp(lua_State *L)
/* Same as CheckError() but returns a pointer instead */
    {
    CheckError(L);
    return NULL;
    }

int CheckErrorFree(lua_State *L, void *ptr)
/* On error, free ptr before raising the error */
    {
    int rc = glGetError();
    if(rc != 0) 
        {
        Free(L, ptr);
        return luaL_error(L, (char*)ErrorString(rc));
        }
    return 0;
    }

int CheckErrorFree2(lua_State *L, void *ptr1, void *ptr2)
/* On error, free ptr1 and ptr2 before raising the error */
    {
    int rc = glGetError();
    if(rc != 0) 
        {
        Free(L, ptr1);
        Free(L, ptr2);
        return luaL_error(L, (char*)ErrorString(rc));
        }
    return 0;
    }

int CheckErrorFree3(lua_State *L, void *ptr1, void *ptr2, void *ptr3)
    {
    int rc = glGetError();
    if(rc != 0) 
        {
        Free(L, ptr1);
        Free(L, ptr2);
        Free(L, ptr3);
        return luaL_error(L, (char*)ErrorString(rc));
        }
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Internal error codes                                                         |
 *------------------------------------------------------------------------------*/

const char* errstring(int err)
    {
    switch(err)
        {
        case 0: return "success";
        case ERR_GENERIC: return "generic error";
        case ERR_TABLE: return "not a table";
        case ERR_EMPTY: return "empty list";
        case ERR_TYPE: return "invalid type";
        case ERR_VALUE: return "invalid value";
        case ERR_NOTPRESENT: return "missing";
        case ERR_MEMORY: return "out of memory";
        case ERR_MALLOC_ZERO: return "zero bytes malloc";
        case ERR_LENGTH: return "invalid length";
        case ERR_POOL: return "elements are not from the same pool";
        case ERR_BOUNDARIES: return "invalid boundaries";
        case ERR_UNKNOWN: return "unknown field name";
        default:
            return "???";
        }
    return NULL; /* unreachable */
    }


