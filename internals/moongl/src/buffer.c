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

/****************************************************************************
 * Buffers management                                                       *
 ****************************************************************************/

#include "internal.h"

BITFIELD_STRINGS(StorageFlagsStrings) = {
    "dynamic storage",
    "map read",
    "map write",
    "map persistent",
    "map coherent",
    "client storage",
    NULL
};
BITFIELD_CODES(StorageFlagsCodes) = {
    GL_DYNAMIC_STORAGE_BIT,
    GL_MAP_READ_BIT,
    GL_MAP_WRITE_BIT,
    GL_MAP_PERSISTENT_BIT,
    GL_MAP_COHERENT_BIT,
    GL_CLIENT_STORAGE_BIT,
};
BITFIELD_T(StorageFlagsBitfield, StorageFlagsStrings, StorageFlagsCodes)
#define CheckStorageFlags(L, arg, mand) bitfieldCheck((L), (arg), (mand), &StorageFlagsBitfield)
#define PushStorageFlags(L, code) bitfieldPush((L), (code), &StorageFlagsBitfield)

BITFIELD_STRINGS(AccessFlagsStrings) = {
    "read",
    "write",
    "persistent",
    "coherent",
    "invalidate buffer",
    "invalidate range",
    "flush explicit",
    "unsynchronized",
    NULL
};
BITFIELD_CODES(AccessFlagsCodes) = {
    GL_MAP_READ_BIT,
    GL_MAP_WRITE_BIT,
    GL_MAP_PERSISTENT_BIT,
    GL_MAP_COHERENT_BIT,
    GL_MAP_INVALIDATE_BUFFER_BIT,
    GL_MAP_INVALIDATE_RANGE_BIT,
    GL_MAP_FLUSH_EXPLICIT_BIT,
    GL_MAP_UNSYNCHRONIZED_BIT,
};
BITFIELD_T(AccessFlagsBitfield, AccessFlagsStrings, AccessFlagsCodes)
#define CheckAccessFlags(L, arg, mand) bitfieldCheck((L), (arg), (mand), &AccessFlagsBitfield)
#define PushAccessFlags(L, code) bitfieldPush((L), (code), &AccessFlagsBitfield)

/*------------------------------------------------------------------------------*
 | Buffer objects                                                               |
 *------------------------------------------------------------------------------*/

NEW_TARGET_FUNC(Buffer, OTYPE_BUFFER, checkbuffertarget)
GEN_FUNC(Buffer, OTYPE_BUFFER)
BIND_TARGET_FUNC(Buffer, checkbuffertarget)
DELETE_FUNC(Buffer, OTYPE_BUFFER)
IS_FUNC(Buffer)
CREATE_FUNC(Buffer, OTYPE_BUFFER)

static int BindBufferRange(lua_State *L)
    {
    GLenum target = checkbufferrangetarget(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint buffer = luaL_checkinteger(L, 3); 
    GLintptr offset = luaL_checkinteger(L, 4);
    GLsizeiptr size = luaL_checkinteger(L, 5);
    glBindBufferRange(target, index, buffer, offset, size);
    CheckError(L);
    return 0;
    }

static int BindBufferBase(lua_State *L)
    {
    GLenum target = checkbufferrangetarget(L, 1);
    GLuint index = luaL_checkinteger(L, 2);
    GLuint buffer = luaL_checkinteger(L, 3); 
    glBindBufferBase(target, index, buffer);
    CheckError(L);
    return 0;
    }

static int BindBuffersRange(lua_State *L) //@@ NO
    {
//void glBindBuffersRange(GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
    NOT_AVAILABLE;
    }

static int BindBuffersBase(lua_State *L) //@@ NO
    {
//void glBindBuffersBase(GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
    NOT_AVAILABLE;
    }

/*------------------------------------------------------------------------------*
 | Create/modify buffer object data                                             |
 *------------------------------------------------------------------------------*/

static int BufferStorage(lua_State *L)
/* size = buffer_storage(target|buffer, data, flag1, flag2, ... ) */
    {
    size_t len;
    const char *data;
    GLsizeiptr size;
    GLbitfield flags;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    if(lua_isnoneornil(L, 2))
        {
        data = NULL;
        size = 0;
        }
    else if(lua_isstring(L, 2))
        {
        data = lua_tolstring(L, 2, &len);
        size = len;
        }
    else if(lua_istable(L, 2))
        {
        data = checkdataptr(L, 2, &len);
        size = len;
        }
    else
        return luaL_argerror(L, 2, "invalid type");
    flags = CheckStorageFlags(L, 3, 0);

    if(buffer==0)
        glBufferStorage(target, size, data, flags);
    else
        glNamedBufferStorage(buffer, size, data, flags);
    CheckError(L);
    lua_pushinteger(L, size);
    return 1;
    }

static int BufferData(lua_State *L)
/* size = buffer_data(target|buffer, data, usage) data = binary string (see string.pack())
 * size = buffer_data(target|buffer, size, usage) allocates memory only, without storing
 *                                              any data in it
 */
    {
    size_t len;
    const char *data;
    GLsizeiptr size;
    GLenum target, usage;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);

    if(lua_isinteger(L, 2))
        {
        data = NULL;
        size = lua_tointeger(L, 2);
        }
    else if(lua_isstring(L, 2))
        {
        data = lua_tolstring(L, 2, &len);
        size = len;
        }
    else if(lua_istable(L, 2))
        {
        data = checkdataptr(L, 2, &len);
        size = len;
        }
    else
        return luaL_argerror(L, 2, "invalid type");
    usage = checkbufferusage(L, 3);
    
    if(buffer == 0)
        glBufferData(target, size, data, usage);
    else
        glNamedBufferData(buffer, size, data, usage);
    CheckError(L);
    lua_pushinteger(L, size);
    return 1;
    }

static int BufferSubData(lua_State *L)
/* size = buffer_sub_data(target|buffer, offset, data) 
 * data = binary string (see string.pack())
 */
    {
    size_t len;
    const char *data;
    GLsizeiptr size;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLintptr offset = lua_tointeger(L, 2);
    if(lua_isstring(L, 3))
        {
        data = lua_tolstring(L, 3, &len);
        size = len;
        }
    else if(lua_istable(L, 3))
        {
        data = checkdataptr(L, 3, &len);
        size = len;
        }
    else
        return luaL_argerror(L, 3, "invalid type");
    
    if(buffer == 0)
        glBufferSubData(target, offset, size, data);
    else
        glNamedBufferSubData(buffer, offset, size, data);

    CheckError(L);
    lua_pushinteger(L, size);
    return 1;
    }


static int ClearBufferData(lua_State *L)
    {
    size_t len;
    const void* data;
    int arg = 1;
    GLenum target;
    GLuint buffer = checktargetorname(L, arg++, &target, DOMAIN_BUFFER_TARGET);
    GLenum internalformat = checkinternalformat(L, arg++);
    GLenum format = checkformat(L, arg++);
    GLenum type = checktype(L, arg++);
    if(lua_isnoneornil(L, arg))
        data = NULL;
    else if(lua_isstring(L, arg))
        data = lua_tolstring(L, arg, &len);
    else if(lua_istable(L, arg))
        data = checkdataptr(L, arg, &len);
    else
        return luaL_argerror(L, arg, "invalid type");
    if(buffer == 0)
        glClearBufferData(target, internalformat, format, type, (void*)data);
    else
        glClearNamedBufferData(buffer, internalformat, format, type, (void*)data);
    return 0;
    }


static int ClearBufferSubData(lua_State *L)
    {
    int arg = 1;
    size_t len;
    const void* data;
    GLenum target;
    GLuint buffer = checktargetorname(L, arg++, &target, DOMAIN_BUFFER_TARGET);
    GLenum internalformat = checkinternalformat(L, arg++);
    GLintptr offset = luaL_checkinteger(L, arg++);
    GLsizei size = luaL_checkinteger(L, arg++);
    GLenum format = checkformat(L, arg++);
    GLenum type = checktype(L, arg++);
    if(lua_isnoneornil(L, arg))
        data = NULL;
    else if(lua_isstring(L, arg))
        data = lua_tolstring(L, arg, &len);
    else if(lua_istable(L, arg))
        data = checkdataptr(L, arg, &len);
    else
        return luaL_argerror(L, arg, "invalid type");
    if(buffer == 0)
        glClearBufferSubData(target, internalformat, offset, size, format, type, (void*)data);
    else
        glClearNamedBufferSubData(buffer,internalformat, offset, size, format, type, (void*)data);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Get buffer data                                                              |
 *------------------------------------------------------------------------------*/

static int GetBufferSubData(lua_State *L)
/* data = get_buffer_sub_data(target|buffer, offset, size) */
    {
    char *data;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLintptr offset = lua_tointeger(L, 2);
    GLsizeiptr size = lua_tointeger(L, 3);
    data = (char*)Malloc(L, size*sizeof(char));
    
    if(buffer == 0)
        glGetBufferSubData(target, offset, size, data);
    else
        glGetNamedBufferSubData(buffer, offset, size, data);
    CheckErrorFree(L, data);
    lua_pushlstring(L, data, size);
    Free(L, data);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Copy buffer data                                                             |
 *------------------------------------------------------------------------------*/

static int CopyBufferSubData(lua_State *L)
/* data = copy_buffer_sub_data(src, dst, src_offset, dst_offset, size) 
 * src,dst = target,target | buffer,buffer
 */
    {
    GLenum src_target, dst_target;
    GLuint src_buffer = checktargetorname(L, 1, &src_target, DOMAIN_BUFFER_TARGET);
    GLuint dst_buffer = checktargetorname(L, 2, &dst_target, DOMAIN_BUFFER_TARGET);
    GLintptr src_offset = lua_tointeger(L, 3);
    GLintptr dst_offset = lua_tointeger(L, 4);
    GLsizeiptr size = lua_tointeger(L, 5);
    if((src_buffer==0) &&(dst_buffer != 0))
        glCopyBufferSubData(src_target, dst_target, src_offset, dst_offset, size);
    else if((dst_buffer!=0) && (dst_buffer != 0))
        glCopyNamedBufferSubData(src_buffer, dst_buffer, src_offset, dst_offset, size); 
    else
        return luaL_error(L, "invalid arguments (can not mix names and targets)");
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Invalidate buffer data                                                       |
 *------------------------------------------------------------------------------*/

static int InvalidateBufferData(lua_State *L)
    {
    GLuint buffer = luaL_checkinteger(L, 1);
    glInvalidateBufferData(buffer);
    CheckError(L);
    return 0;
    }

static int InvalidateBufferSubData(lua_State *L)
    {
    GLuint buffer = luaL_checkinteger(L, 1);
    GLintptr offset = luaL_checkinteger(L, 2);
    GLsizeiptr length = luaL_checkinteger(L, 3);
    glInvalidateBufferSubData(buffer, offset, length);
    CheckError(L);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Get buffer parameter                                                         |
 *------------------------------------------------------------------------------*/

static int GetEnum(lua_State *L, GLenum target, GLuint buffer, GLenum pname, uint32_t domain)
    {
    GLint param;
    if(buffer==0)
        glGetBufferParameteriv(target, pname, &param);
    else
        glGetNamedBufferParameteriv(buffer, pname, &param);
    CheckError(L);
    return enums_push(L, domain, param);
    }

static GLbitfield GetBitfield_(lua_State *L, GLenum target, GLuint buffer, GLenum pname)
    {
    GLint param;
    if(buffer==0)
        glGetBufferParameteriv(target, pname, &param);
    else
        glGetNamedBufferParameteriv(buffer, pname, &param);
    CheckError(L);
    return (GLbitfield)param;
    }


static int GetBitfield(lua_State *L, GLenum target, GLuint buffer, GLenum pname, bitfield_t *b)
    {
    GLbitfield param = GetBitfield_(L, target, buffer, pname);
    return bitfieldPush(L, param, b);
    }

static int GetInt64_(lua_State *L, GLenum target, GLuint buffer, GLenum pname)
    {
    GLint64 param;
    if(buffer==0)
        glGetBufferParameteri64v(target, pname, &param);
    else
        glGetNamedBufferParameteri64v(buffer, pname, &param);
    CheckError(L);
    return param;
    }


static int GetInt64(lua_State *L, GLenum target, GLuint buffer, GLenum pname)
    {
    GLint64 param = GetInt64_(L, target, buffer, pname);
    lua_pushnumber(L, param);
    return 1;
    }

static int GetBoolean(lua_State *L, GLenum target, GLuint buffer, GLenum pname)
    {
    GLint param;
    if(buffer==0)
        glGetBufferParameteriv(target, pname, &param);
    else
        glGetNamedBufferParameteriv(buffer, pname, &param);
    CheckError(L);
    lua_pushboolean(L, (param == GL_TRUE) );
    return 1;
    }


static int GetBufferParameter(lua_State *L)
/* get_buffer_parameter(target|buffer, pname) */
    {
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLenum pname = checkbufferpname(L, 2);
    switch(pname)
        {
        case GL_BUFFER_SIZE: return GetInt64(L, target, buffer, pname);
        case GL_BUFFER_USAGE: return GetEnum(L, target, buffer, pname, DOMAIN_BUFFER_USAGE);
        case GL_BUFFER_ACCESS: return GetEnum(L, target, buffer, pname, DOMAIN_BUFFER_ACCESS);
        case GL_BUFFER_ACCESS_FLAGS:
                    return GetBitfield(L, target, buffer, pname, &AccessFlagsBitfield);
        case GL_BUFFER_IMMUTABLE_STORAGE: return GetBoolean(L, target, buffer, pname);
        case GL_BUFFER_MAPPED: return GetBoolean(L, target, buffer, pname);
/*      case GL_BUFFER_MAP_POINTER: NOT_AVAILABLE; used internally (see mapbuffer.c) */
        case GL_BUFFER_MAP_OFFSET: return GetInt64(L, target, buffer, pname);
        case GL_BUFFER_MAP_LENGTH: return GetInt64(L, target, buffer, pname);
        case GL_BUFFER_STORAGE_FLAGS: 
                    return GetBitfield(L, target, buffer, pname, &StorageFlagsBitfield);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }



/*------------------------------------------------------------------------------*
 | Mapped buffer                                                                |
 *------------------------------------------------------------------------------*/

static int MapBuffer(lua_State *L)
/* map_buffer(target|buffer, access) */
    {
    void *ptr;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLenum access = checkbufferaccess(L, 2);
    if(buffer == 0)
        ptr = glMapBuffer(target, access);
    else
        ptr = glMapNamedBuffer(buffer, access);
    CheckError(L);
    if(ptr==NULL)
        return luaL_error(L, "cannot bind buffer");
    lua_pushlightuserdata(L, ptr);
    return 1;
    }

static int MapBufferRange(lua_State *L)
/* map_buffer_range(target|buffer, offset, length, accessbit1, ...) */
    {
    void *ptr;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLintptr offset = luaL_checkinteger(L, 2);
    GLsizei length = luaL_checkinteger(L, 3);
    GLbitfield access = CheckAccessFlags(L, 4, 0);
    if(buffer == 0)
        ptr = glMapBufferRange(target, offset, length, access);
    else
        ptr = glMapNamedBufferRange(buffer, offset, length, access);
    CheckError(L);
    if(ptr==NULL)
        return luaL_error(L, "cannot bind buffer");
    lua_pushlightuserdata(L, ptr);
    return 1;
    }


static int FlushMappedBufferRange(lua_State *L)
    {
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    GLintptr offset = luaL_checkinteger(L, 2);
    GLsizei length = luaL_checkinteger(L, 3);
    if(buffer == 0)
        glFlushMappedBufferRange(target, offset, length);
    else
        glFlushMappedNamedBufferRange(buffer, offset, length);
    CheckError(L);
    return 0;
    }


static int UnmapBuffer(lua_State *L)
/* unmap_buffer(targe|buffer)
 * -> boolean
 */
    {
    GLboolean res;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    if(buffer == 0)
        res = glUnmapBuffer(target);
    else
        res = glUnmapNamedBuffer(buffer);
    CheckError(L);
    lua_pushboolean(L, res);
    return 1;
    }


static void *GetPointer(lua_State *L, GLenum target, GLuint buffer)
    {
    void *ptr = NULL;
    if(buffer == 0)
        glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &ptr);
    else
        glGetNamedBufferPointerv(buffer, GL_BUFFER_MAP_POINTER, &ptr);
    CheckErrorp(L);
    if(ptr == NULL)
        { luaL_error(L, "cannot retrieve buffer pointer"); return NULL; }
    return ptr;
    }

#define GetMapOffset(L, target, buffer) GetInt64_((L),(target),(buffer),GL_BUFFER_MAP_OFFSET)
#define GetMapLength(L, target, buffer) GetInt64_((L),(target),(buffer),GL_BUFFER_MAP_LENGTH)
#define GetAccessFlags(L, target, buffer) GetBitfield_((L),(target),(buffer),GL_BUFFER_ACCESS_FLAGS)

static int MapWrite(lua_State *L) /* nongl */
/* mapped_buffer_write(target|buffer, offset, bstring) */
    {
    size_t length, buflen;
    const char *data;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    char *ptr = (char*)GetPointer(L, target, buffer);
    GLintptr offset = luaL_checkinteger(L, 2);
    if(lua_isstring(L, 3))
        data = luaL_checklstring(L, 3, &length);
    else if(lua_istable(L, 3))
        data = checkdataptr(L, 3, &length);
    else
        return luaL_argerror(L, 3, "invalid data");
    /* access and boundary checks */
    GLbitfield flags = GetAccessFlags(L, target, buffer);
    if(!(flags & GL_MAP_WRITE_BIT))
        return luaL_error(L, "no write access");
    buflen = GetMapLength(L, target, buffer);
    if(length > (buflen - offset))
        return luaL_error(L, "length is too large");
    memcpy(ptr + offset, data, length);
    return 0;
    }

static int MapRead(lua_State *L) /* nongl */
/* bstring = mapped_buffer_read(target|buffer, offset, length) */
    {
    size_t buflen;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    char *ptr = (char*)GetPointer(L, target, buffer);
    GLintptr offset = luaL_checkinteger(L, 2);
    size_t length =  luaL_checkinteger(L, 3);
    /* access and boundary checks */
    GLbitfield flags = GetAccessFlags(L, target, buffer);
    if(!(flags & GL_MAP_READ_BIT))
        return luaL_error(L, "no read access");
    buflen = GetMapLength(L, target, buffer);
    if(length > (buflen - offset))
        return luaL_error(L, "length is too large");
    lua_pushlstring(L, ptr+offset, length);
    return 1;
    }

static int MapCopyFrom(lua_State *L) /* nongl */
/* mapped_buffer_copy_from(target|buffer, offset, length, srcptr, [srcoffset]) */
    {
    size_t buflen;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    char *ptr = (char*)GetPointer(L, target, buffer);
    GLintptr offset = luaL_checkinteger(L, 2);
    size_t length = luaL_checkinteger(L, 3);
    char *srcptr = (char*)checklightuserdata(L, 4);
    size_t srcoffset = luaL_optinteger(L, 5, 0);
    /* access and boundary checks */
    GLbitfield flags = GetAccessFlags(L, target, buffer);
    if(!(flags & GL_MAP_WRITE_BIT))
        return luaL_error(L, "no write access");
    buflen = GetMapLength(L, target, buffer);
    if(length > (buflen - offset))
        return luaL_error(L, "length is too large");
    memcpy(ptr + offset, srcptr + srcoffset, length);
    return 0;
    }

static int MapCopyTo(lua_State *L) /* nongl */
/* bstring = mapped_buffer_copy_to(target|buffer, offset, length, dstptr, [dstoffset]) */
    {
    size_t buflen;
    GLenum target;
    GLuint buffer = checktargetorname(L, 1, &target, DOMAIN_BUFFER_TARGET);
    char *ptr = (char*)GetPointer(L, target, buffer);
    GLintptr offset = luaL_checkinteger(L, 2);
    size_t length =  luaL_checkinteger(L, 3);
    char *dstptr = (char*)checklightuserdata(L, 4);
    size_t dstoffset = luaL_optinteger(L, 5, 0);
    /* access and boundary checks */
    GLbitfield flags = GetAccessFlags(L, target, buffer);
    if(!(flags & GL_MAP_READ_BIT))
        return luaL_error(L, "no read access");
    buflen = GetMapLength(L, target, buffer);
    if(length > (buflen - offset))
        return luaL_error(L, "length is too large");
    memcpy(dstptr + dstoffset, ptr + offset, length);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "new_buffer", NewBuffer },
        { "gen_buffers", GenBuffers },
        { "create_buffers", CreateBuffers },
        { "bind_buffer", BindBuffer },
        { "is_buffer", IsBuffer },
        { "delete_buffers", DeleteBuffers },
        { "bind_buffer_range", BindBufferRange },
        { "bind_buffers_range", BindBuffersRange },
        { "bind_buffer_base", BindBufferBase },
        { "bind_buffers_base", BindBuffersBase },
        { "buffer_storage", BufferStorage },
        { "buffer_data", BufferData },
        { "buffer_sub_data", BufferSubData },
        { "clear_buffer_data", ClearBufferData },
        { "clear_buffer_sub_data", ClearBufferSubData },
        { "get_buffer_sub_data", GetBufferSubData },
        { "copy_buffer_sub_data", CopyBufferSubData },
        { "invalidate_buffer_data", InvalidateBufferData },
        { "invalidate_buffer_sub_data", InvalidateBufferSubData },
        { "get_buffer_parameter", GetBufferParameter },
        { "map_buffer", MapBuffer },
        { "map_buffer_range", MapBufferRange },
        { "flush_mapped_buffer_range", FlushMappedBufferRange },
        { "unmap_buffer", UnmapBuffer },
        { "mapped_buffer_write", MapWrite },
        { "mapped_buffer_read", MapRead },
        { "mapped_buffer_copy_from", MapCopyFrom },
        { "mapped_buffer_copy_to", MapCopyTo },
        { NULL, NULL } /* sentinel */
    };


void moongl_open_buffer(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

