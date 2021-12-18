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

BITFIELD_STRINGS(ContextFlagStrings) = {
    "forward compatible",
    "debug",
    "robust access",
    "no error",
    NULL
};
BITFIELD_CODES(ContextFlagCodes) = {
    GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT,
    GL_CONTEXT_FLAG_DEBUG_BIT,
    GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT,
    GL_CONTEXT_FLAG_NO_ERROR_BIT, //GL_VERSION_4_6
};
BITFIELD_T(ContextFlagBitfield, ContextFlagStrings, ContextFlagCodes)
#define CheckContextFlag(L, arg, mand) bitfieldCheck((L), (arg), (mand), &ContextFlagBitfield)
#define PushContextFlag(L, code) bitfieldPush((L), (code), &ContextFlagBitfield)

/*------------------------------------------------------------------------------*
 | exported get() functions                                                     |
 *------------------------------------------------------------------------------*/

GLsizei getSizei(lua_State *L, GLenum pname)
    {
    GLint64 len;
    glGetInteger64v(pname, &len);
    CheckError(L);
    return (GLsizei)len;
    }

/*------------------------------------------------------------------------------*
 | Get()                                                                        |
 *------------------------------------------------------------------------------*/

static int GetActiveTexture(lua_State *L, GLenum pname)
    {
    GLint val;
    glGetIntegerv(pname, &val);
    CheckError(L);
    switch(val)
        {
#define CASE(n) case GL_TEXTURE##n: lua_pushinteger(L, n); break
        CASE(0); CASE(1); CASE(2); CASE(3); CASE(4); CASE(5);
        CASE(6); CASE(7); CASE(8); CASE(9); CASE(10); CASE(11);
        CASE(12); CASE(13); CASE(14); CASE(15); CASE(16); CASE(17);
        CASE(18); CASE(19); CASE(20); CASE(21); CASE(22); CASE(23);
        CASE(24); CASE(25); CASE(26); CASE(27); CASE(28); CASE(29);
        CASE(30); CASE(31);
#undef CASE
        default:
            luaL_error(L, UNEXPECTED_ERROR);
        }
    return 1;
    }

static int GetEnum(lua_State *L, GLenum pname, uint32_t domain)
    {
    GLint val;
    glGetIntegerv(pname, &val);
    CheckError(L);
    return enums_push(L, domain, val);
    }

static int GetEnum2(lua_State *L, GLenum pname, uint32_t domain)
    {
    GLint val[2];
    glGetIntegerv(pname, val);
    CheckError(L);
    enums_push(L, domain, val[0]);
    enums_push(L, domain, val[1]);
    return 2;
    }

static int GetEnumN(lua_State *L, GLenum pname, GLenum numpname, uint32_t domain)
    {
    GLint *val;
    GLsizei i, num = getSizei(L, numpname);
    if(num==0) return 0;
    val = (GLint*)Malloc(L, num*sizeof(GLint));
    glGetIntegerv(pname, val);
    CheckErrorFree(L, val);
    luaL_checkstack(L, num, NULL);
    for(i = 0; i<num; i++)
        enums_push(L, domain, val[i]);
    Free(L, val);
    return num;
    }

static int GetEnumOptIndex(lua_State *L, GLenum pname, uint32_t domain) /* index is optional */
    {
    GLint data;
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetIntegeri_v(pname, index, &data);
        }
    else
        glGetIntegerv(pname, &data);
    CheckError(L);
    return enums_push(L, domain, data);
    }

static int GetBitfield(lua_State *L, GLenum pname, bitfield_t *b)
    {
    GLint val;
    glGetIntegerv(pname, &val);
    CheckError(L);
    return bitfieldPush(L, val, b);
    }

static int GetInt(lua_State *L, GLenum pname)
    {
    GLint64 data;
    glGetInteger64v(pname, &data);
    CheckError(L);
    lua_pushinteger(L, data);
    return 1;
    }

#if 0
static int GetInt2(lua_State *L, GLenum pname)
    {
    GLint64 data[2];
    glGetInteger64v(pname, data);
    CheckError(L);
    lua_pushinteger(L, data[0]);
    lua_pushinteger(L, data[1]);
    return 2;
    }

static int GetInt4(lua_State *L, GLenum pname)
    {
    GLint64 data[4];
    glGetInteger64v(pname, data);
    CheckError(L);
    lua_pushinteger(L, data[0]);
    lua_pushinteger(L, data[1]);
    lua_pushinteger(L, data[2]);
    lua_pushinteger(L, data[3]);
    return 4;
    }
#endif

static int GetIntN(lua_State *L, GLenum pname, GLenum numpname)
    {
    GLint64 *data;
    GLsizei i, num = getSizei(L, numpname);
    if(num==0) return 0;
    data = (GLint64*)Malloc(L, num*sizeof(GLint64));
    glGetInteger64v(pname, data);
    CheckErrorFree(L, data);
    for(i = 0; i<num; i++)
        lua_pushinteger(L, data[i]);
    Free(L, data);
    return num;
    }

static int GetIntOptIndex(lua_State *L, GLenum pname) /* index is optional */
    {
    GLint64 data;
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetInteger64i_v(pname, index, &data);
        }
    else
        glGetInteger64v(pname, &data);
    CheckError(L);
    lua_pushinteger(L, data);
    return 1;
    }

static int GetInt4OptIndex(lua_State *L, GLenum pname) /* index is optional */
    {
    GLint64 data[4];
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetInteger64i_v(pname, index, data);
        }
    else
        glGetInteger64v(pname, data);
    CheckError(L);
    lua_pushinteger(L, data[0]);
    lua_pushinteger(L, data[1]);
    lua_pushinteger(L, data[2]);
    lua_pushinteger(L, data[3]);
    return 4;
    }

static int GetIntIndex(lua_State *L, GLenum pname)
    {
    GLint64 data;
    GLuint index = luaL_checkinteger(L, 2);
    glGetInteger64i_v(pname, index, &data);
    CheckError(L);
    lua_pushinteger(L, data);
    return 1;
    }

static int GetBoolean(lua_State *L, GLenum pname)
    {
    GLboolean data;
    glGetBooleanv(pname, &data);
    CheckError(L);
    lua_pushboolean(L, (data == GL_TRUE));
    return 1;
    }

#if 0
static int GetBoolean4(lua_State *L, GLenum pname)
    {
    GLboolean data[4];
    glGetBooleanv(pname, data);
    CheckError(L);
    lua_pushboolean(L, (data[0] == GL_TRUE));
    lua_pushboolean(L, (data[1] == GL_TRUE));
    lua_pushboolean(L, (data[2] == GL_TRUE));
    lua_pushboolean(L, (data[3] == GL_TRUE));
    return 4;
    }

static int GetBooleanIndex(lua_State *L, GLenum pname)
    {
    GLboolean data;
    GLuint index;
    index = luaL_checkinteger(L, 2);
    glGetBooleani_v(pname, index, &data);
    CheckError(L);
    lua_pushboolean(L, data);
    return 1;
    }
#endif

static int GetBooleanOptIndex(lua_State *L, GLenum pname) /* index is optional */
    {
    GLboolean data;
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetBooleani_v(pname, index, &data);
        }
    else
        glGetBooleanv(pname, &data);
    CheckError(L);
    lua_pushboolean(L, data);
    return 1;
    }

static int GetBoolean4Index(lua_State *L, GLenum pname)
    {
    GLboolean data[4];
    GLuint index;
    index = luaL_checkinteger(L, 2);
    glGetBooleani_v(pname, index, data);
    CheckError(L);
    lua_pushboolean(L, data[0]);
    lua_pushboolean(L, data[1]);
    lua_pushboolean(L, data[2]);
    lua_pushboolean(L, data[3]);
    return 4;
    }

static int GetBoolean4OptIndex(lua_State *L, GLenum pname) /* index is optional */
    {
    GLboolean data[4];
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetBooleani_v(pname, index, data);
        }
    else
        glGetBooleanv(pname, data);
    CheckError(L);
    lua_pushboolean(L, data[0]);
    lua_pushboolean(L, data[1]);
    lua_pushboolean(L, data[2]);
    lua_pushboolean(L, data[3]);
    return 4;
    }



static int GetFloat(lua_State *L, GLenum pname)
    {
    GLfloat data;
    glGetFloatv(pname, &data);
    CheckError(L);
    lua_pushnumber(L, data);
    return 1;
    }

static int GetFloat2(lua_State *L, GLenum pname)
    {
    GLfloat data[2];
    glGetFloatv(pname, data);
    CheckError(L);
    lua_pushnumber(L, data[0]);
    lua_pushnumber(L, data[1]);
    return 2;
    }

static int GetFloat4(lua_State *L, GLenum pname)
    {
    GLfloat data[4];
    glGetFloatv(pname, data);
    CheckError(L);
    lua_pushnumber(L, data[0]);
    lua_pushnumber(L, data[1]);
    lua_pushnumber(L, data[2]);
    lua_pushnumber(L, data[3]);
    return 4;
    }

static int GetFloat4Index(lua_State *L, GLenum pname)
    {
    GLfloat data[4];
    GLuint index;
    index = luaL_checkinteger(L, 2);
    glGetFloati_v(pname, index, data);
    CheckError(L);
    lua_pushnumber(L, data[0]);
    lua_pushnumber(L, data[1]);
    lua_pushnumber(L, data[2]);
    lua_pushnumber(L, data[3]);
    return 4;
    }


static int GetFloat4OptIndex(lua_State *L, GLenum pname) /* index is optional */
    {
    GLfloat data[4];
    GLuint index;
    if(!lua_isnoneornil(L, 2))
        {
        index = luaL_checkinteger(L, 2);
        glGetFloati_v(pname, index, data);
        }
    else
        glGetFloatv(pname, data);
    CheckError(L);
    lua_pushnumber(L, data[0]);
    lua_pushnumber(L, data[1]);
    lua_pushnumber(L, data[2]);
    lua_pushnumber(L, data[3]);
    return 4;
    }


#if 0
void glGetBooleanv (GLenum pname, GLboolean *data);
void glGetDoublev (GLenum pname, GLdouble *data);
void glGetFloatv (GLenum pname, GLfloat *data);
void glGetIntegerv (GLenum pname, GLint *data);
void glGetInteger64v (GLenum pname, GLint64 *data);
void glGetBooleani_v(GLenum target, GLuint index, GLboolean *data);
void glGetIntegeri_v(GLenum target, GLuint index, GLint *data);
void glGetFloati_v(GLenum target, GLuint index, GLfloat *data);
void glGetDoublei_v(GLenum target, GLuint index, GLdouble *data);
void glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data);
void glGetPointerv(GLenum pname, void **params);
#endif

static int Get(lua_State *L)
    {
    GLenum pname = checkpname(L, 1);
    switch(pname)
        {
        case GL_CONTEXT_FLAGS: return GetBitfield(L, pname, &ContextFlagBitfield);
        case GL_MAJOR_VERSION: return GetInt(L, pname);
        case GL_MAX_SERVER_WAIT_TIMEOUT: return GetInt(L, pname);
        case GL_MIN_MAP_BUFFER_ALIGNMENT: return GetInt(L, pname);
        case GL_MINOR_VERSION: return GetInt(L, pname);
        case GL_NUM_EXTENSIONS: return GetInt(L, pname);
        case GL_NUM_SPIR_V_EXTENSIONS: return GetInt(L, pname); //GL_VERSION_4_6
        case GL_NUM_SHADING_LANGUAGE_VERSIONS: return GetInt(L, pname);
        case GL_TIMESTAMP: return GetInt(L, pname);
        /* Buffer Binding State */
        case GL_ARRAY_BUFFER_BINDING: return GetInt(L, pname);
        case GL_COPY_READ_BUFFER_BINDING: return GetInt(L, pname);
        case GL_COPY_WRITE_BUFFER_BINDING: return GetInt(L, pname);
        case GL_DRAW_INDIRECT_BUFFER_BINDING: return GetInt(L, pname);
        case GL_ELEMENT_ARRAY_BUFFER_BINDING: return GetInt(L, pname);
        case GL_PARAMETER_BUFFER_BINDING: return GetInt(L, pname); //GL_VERSION_4_6
        case GL_QUERY_BUFFER_BINDING: return GetInt(L, pname);
        case GL_TEXTURE_BUFFER_BINDING: return GetInt(L, pname);
        case GL_VERTEX_ARRAY_BINDING: return GetInt(L, pname);
        case GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS: return GetInt(L, pname);
        case GL_ATOMIC_COUNTER_BUFFER_BINDING: return GetIntOptIndex(L, pname);
        case GL_ATOMIC_COUNTER_BUFFER_START: return GetIntOptIndex(L, pname);
        case GL_ATOMIC_COUNTER_BUFFER_SIZE: return GetIntOptIndex(L, pname);
        case GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: return GetInt(L, pname);
        case GL_SHADER_STORAGE_BUFFER_BINDING: return GetIntOptIndex(L, pname);
        case GL_SHADER_STORAGE_BUFFER_START: return GetIntIndex(L, pname);
        case GL_SHADER_STORAGE_BUFFER_SIZE: return GetIntIndex(L, pname);
    /*  case GL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS: */
        case GL_MAX_TRANSFORM_FEEDBACK_BUFFERS: return GetInt(L, pname);
        case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING: return GetIntOptIndex(L, pname);
        case GL_TRANSFORM_FEEDBACK_BUFFER_START: return GetIntIndex(L, pname);
        case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE: return GetIntIndex(L, pname);
        case GL_MAX_UNIFORM_BUFFER_BINDINGS: return GetInt(L, pname);
        case GL_UNIFORM_BUFFER_BINDING: return GetIntOptIndex(L, pname);
        case GL_UNIFORM_BUFFER_START: return GetIntIndex(L, pname);
        case GL_UNIFORM_BUFFER_SIZE: return GetIntIndex(L, pname);
        case GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS: return GetInt(L, pname);
        case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS: return GetInt(L, pname);
        case GL_TRANSFORM_FEEDBACK_PAUSED: return GetBoolean(L, pname);
        case GL_TRANSFORM_FEEDBACK_ACTIVE: return GetBoolean(L, pname);
        /* Debug Output State */
        case GL_DEBUG_GROUP_STACK_DEPTH: return GetInt(L, pname);
        case GL_DEBUG_LOGGED_MESSAGES: return GetInt(L, pname);
        case GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH: return GetInt(L, pname);
        case GL_DEBUG_OUTPUT: return GetBoolean(L, pname);
        case GL_DEBUG_OUTPUT_SYNCHRONOUS: return GetBoolean(L, pname);
        case GL_MAX_DEBUG_LOGGED_MESSAGES: return GetInt(L, pname);
        case GL_MAX_DEBUG_MESSAGE_LENGTH: return GetInt(L, pname);
        case GL_MAX_DEBUG_GROUP_STACK_DEPTH: return GetInt(L, pname);
        case GL_MAX_LABEL_LENGTH: return GetInt(L, pname);
        /* Framebuffers */
        case GL_COLOR_CLEAR_VALUE: return GetFloat4(L, pname);
        case GL_COLOR_WRITEMASK: return GetBoolean4OptIndex(L, pname);
        case GL_DEPTH_CLEAR_VALUE: return GetFloat(L, pname);
        case GL_DEPTH_FUNC: return GetEnum(L, pname, DOMAIN_STENCIL_FUNC);
        case GL_DEPTH_TEST: return GetBoolean(L, pname);
        case GL_DEPTH_WRITEMASK: return GetBoolean(L, pname);
        case GL_DOUBLEBUFFER: return GetBoolean(L, pname);
        case GL_DRAW_BUFFER:
        case GL_DRAW_BUFFER0:
        case GL_DRAW_BUFFER1:
        case GL_DRAW_BUFFER2:
        case GL_DRAW_BUFFER3:
        case GL_DRAW_BUFFER4:
        case GL_DRAW_BUFFER5:
        case GL_DRAW_BUFFER6:
        case GL_DRAW_BUFFER7:
        case GL_DRAW_BUFFER8:
        case GL_DRAW_BUFFER9:
        case GL_DRAW_BUFFER10:
        case GL_DRAW_BUFFER11:
        case GL_DRAW_BUFFER12:
        case GL_DRAW_BUFFER13:
        case GL_DRAW_BUFFER14:
        case GL_DRAW_BUFFER15: return GetEnum(L, pname, DOMAIN_BUFFER);
        case GL_DRAW_FRAMEBUFFER_BINDING: return GetInt(L, pname);
        case GL_MAX_COLOR_ATTACHMENTS: return GetInt(L, pname);
        case GL_MAX_COLOR_TEXTURE_SAMPLES: return GetInt(L, pname);
        case GL_MAX_DEPTH_TEXTURE_SAMPLES: return GetInt(L, pname);
        case GL_MAX_DRAW_BUFFERS: return GetInt(L, pname);
        case GL_MAX_DUAL_SOURCE_DRAW_BUFFERS: return GetInt(L, pname);
        case GL_MAX_FRAMEBUFFER_HEIGHT: return GetInt(L, pname);
        case GL_MAX_FRAMEBUFFER_LAYERS: return GetInt(L, pname);
        case GL_MAX_FRAMEBUFFER_SAMPLES: return GetInt(L, pname);
        case GL_MAX_FRAMEBUFFER_WIDTH: return GetInt(L, pname);
        case GL_MAX_INTEGER_SAMPLES: return GetInt(L, pname);
        case GL_MAX_SAMPLES: return GetInt(L, pname);
        case GL_READ_BUFFER: return GetEnum(L, pname, DOMAIN_BUFFER);
        case GL_READ_FRAMEBUFFER_BINDING: return GetInt(L, pname);
        case GL_RENDERBUFFER_BINDING: return GetInt(L, pname);
        case GL_STENCIL_BACK_FAIL: return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_BACK_FUNC:  return GetEnum(L, pname, DOMAIN_STENCIL_FUNC);
        case GL_STENCIL_BACK_PASS_DEPTH_FAIL:  return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_BACK_PASS_DEPTH_PASS:  return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_BACK_REF: return GetInt(L, pname);
        case GL_STENCIL_BACK_VALUE_MASK:  return GetInt(L, pname);
        case GL_STENCIL_BACK_WRITEMASK: return GetInt(L, pname);
        case GL_STENCIL_CLEAR_VALUE: return GetInt(L, pname);
        case GL_STENCIL_FAIL: return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_FUNC: return GetEnum(L, pname, DOMAIN_STENCIL_FUNC);
        case GL_STENCIL_PASS_DEPTH_FAIL:  return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_PASS_DEPTH_PASS:  return GetEnum(L, pname, DOMAIN_STENCIL_OP);
        case GL_STENCIL_REF: return GetInt(L, pname);
        case GL_STENCIL_TEST: return GetBoolean(L, pname);
        case GL_STENCIL_VALUE_MASK: return GetInt(L, pname);
        case GL_STENCIL_WRITEMASK: return GetInt(L, pname);
        case GL_STEREO: return GetBoolean(L, pname);
        /* Hints */
        case GL_FRAGMENT_SHADER_DERIVATIVE_HINT: return GetEnum(L, pname, DOMAIN_HINT_MODE);
        case GL_LINE_SMOOTH_HINT: return GetEnum(L, pname, DOMAIN_HINT_MODE);
        case GL_POLYGON_SMOOTH_HINT:  return GetEnum(L, pname, DOMAIN_HINT_MODE);
        case GL_TEXTURE_COMPRESSION_HINT: return GetEnum(L, pname, DOMAIN_HINT_MODE);
        /* Image State */
        case GL_IMAGE_BINDING_ACCESS: return GetIntIndex(L, pname);
        case GL_IMAGE_BINDING_FORMAT: return GetIntIndex(L, pname);
        case GL_IMAGE_BINDING_NAME: return GetIntIndex(L, pname);
        case GL_IMAGE_BINDING_LAYER: return GetIntIndex(L, pname);
        case GL_IMAGE_BINDING_LAYERED: return GetIntIndex(L, pname);
        case GL_IMAGE_BINDING_LEVEL: return GetIntIndex(L, pname);
        /* Multisampling */
        case GL_MAX_SAMPLE_MASK_WORDS: return GetInt(L, pname);
        case GL_MULTISAMPLE: return GetBoolean(L, pname);
        case GL_SAMPLE_ALPHA_TO_COVERAGE: return GetBoolean(L, pname);
        case GL_SAMPLE_ALPHA_TO_ONE: return GetBoolean(L, pname);
        case GL_SAMPLE_BUFFERS: return GetInt(L, pname);
        case GL_SAMPLE_COVERAGE: return GetBoolean(L, pname);
        case GL_SAMPLE_COVERAGE_INVERT: return GetBoolean(L, pname);
        case GL_SAMPLE_COVERAGE_VALUE: return GetFloat(L, pname);
        case GL_SAMPLE_MASK: return GetBoolean(L, pname);
        case GL_SAMPLE_MASK_VALUE: return GetIntIndex(L, pname);
        case GL_SAMPLES: return GetInt(L, pname);
        /* Pixel Operations */
        case GL_BLEND: return GetBooleanOptIndex(L, pname);
        case GL_BLEND_COLOR: return GetFloat4OptIndex(L, pname);
        case GL_BLEND_DST_ALPHA: return GetEnum(L, pname, DOMAIN_BLEND_FACTOR);
        case GL_BLEND_DST_RGB:  return GetEnumOptIndex(L, pname, DOMAIN_BLEND_FACTOR);
        case GL_BLEND_EQUATION_RGB: return GetEnumOptIndex(L, pname, DOMAIN_BLEND_MODE);
        case GL_BLEND_EQUATION_ALPHA: return GetEnumOptIndex(L, pname, DOMAIN_BLEND_MODE);
        case GL_BLEND_SRC_ALPHA: return GetEnumOptIndex(L, pname, DOMAIN_BLEND_FACTOR);
        case GL_BLEND_SRC_RGB: return GetEnumOptIndex(L, pname, DOMAIN_BLEND_FACTOR);
        case GL_COLOR_LOGIC_OP: return GetBoolean(L, pname);
        case GL_DITHER: return GetBoolean(L, pname);
        case GL_LOGIC_OP_MODE: return GetEnum(L, pname, DOMAIN_LOGIC_OP);
        case GL_SCISSOR_BOX: return GetFloat4Index(L, pname);
        case GL_SCISSOR_TEST: return GetBoolean4Index(L, pname);
        /* Pixel Transfer Operations */
        case GL_CLAMP_READ_COLOR: return GetEnum(L, pname, DOMAIN_CLAMP);
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT: return GetEnum(L, pname, DOMAIN_FORMAT);
        case GL_IMPLEMENTATION_COLOR_READ_TYPE: return GetEnum(L, pname, DOMAIN_TYPE);
        case GL_PACK_ALIGNMENT: return GetInt(L, pname);
        case GL_PACK_COMPRESSED_BLOCK_DEPTH: return GetInt(L, pname);
        case GL_PACK_COMPRESSED_BLOCK_HEIGHT: return GetInt(L, pname);
        case GL_PACK_COMPRESSED_BLOCK_SIZE: return GetInt(L, pname);
        case GL_PACK_COMPRESSED_BLOCK_WIDTH: return GetInt(L, pname);
        case GL_PACK_IMAGE_HEIGHT: return GetInt(L, pname);
        case GL_PACK_LSB_FIRST: return GetBoolean(L, pname);
        case GL_PACK_ROW_LENGTH: return GetInt(L, pname);
        case GL_PACK_SKIP_IMAGES: return GetInt(L, pname);
        case GL_PACK_SKIP_PIXELS: return GetInt(L, pname);
        case GL_PACK_SKIP_ROWS: return GetInt(L, pname);
        case GL_PACK_SWAP_BYTES: return GetBoolean(L, pname);
        case GL_PIXEL_PACK_BUFFER_BINDING: return GetInt(L, pname);
        case GL_PIXEL_UNPACK_BUFFER_BINDING: return GetInt(L, pname);
        case GL_UNPACK_ALIGNMENT: return GetInt(L, pname);
        case GL_UNPACK_COMPRESSED_BLOCK_DEPTH: return GetInt(L, pname);
        case GL_UNPACK_COMPRESSED_BLOCK_HEIGHT: return GetInt(L, pname);
        case GL_UNPACK_COMPRESSED_BLOCK_SIZE: return GetInt(L, pname);
        case GL_UNPACK_COMPRESSED_BLOCK_WIDTH: return GetInt(L, pname);
        case GL_UNPACK_IMAGE_HEIGHT: return GetInt(L, pname);
        case GL_UNPACK_LSB_FIRST: return GetBoolean(L, pname);
        case GL_UNPACK_ROW_LENGTH: return GetInt(L, pname);
        case GL_UNPACK_SKIP_IMAGES: return GetInt(L, pname);
        case GL_UNPACK_SKIP_PIXELS: return GetInt(L, pname);
        case GL_UNPACK_SKIP_ROWS: return GetInt(L, pname);
        case GL_UNPACK_SWAP_BYTES: return GetBoolean(L, pname);
        /* Programs */
        case GL_CURRENT_PROGRAM: return GetInt(L, pname);
        case GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE: return GetInt(L, pname);
        case GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES: return GetInt(L, pname);
        case GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS: return GetInt(L, pname);
        case GL_MAX_IMAGE_SAMPLES: return GetInt(L, pname);
        case GL_MAX_IMAGE_UNITS: return GetInt(L, pname);
        case GL_MAX_PROGRAM_TEXEL_OFFSET: return GetInt(L, pname);
        case GL_MAX_SHADER_STORAGE_BLOCK_SIZE: return GetInt(L, pname);
        case GL_MAX_SUBROUTINES: return GetInt(L, pname);
        case GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS: return GetInt(L, pname);
        case GL_MAX_UNIFORM_BLOCK_SIZE: return GetInt(L, pname);
        case GL_MAX_UNIFORM_LOCATIONS: return GetInt(L, pname);
        case GL_MAX_VARYING_VECTORS: return GetInt(L, pname);
        case GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET: return GetInt(L, pname);
        case GL_MAX_VERTEX_ATTRIB_BINDINGS: return GetInt(L, pname);
        case GL_MAX_VERTEX_ATTRIB_STRIDE: return GetInt(L, pname);
        case GL_MIN_PROGRAM_TEXEL_OFFSET: return GetInt(L, pname);
        case GL_NUM_PROGRAM_BINARY_FORMATS: return GetInt(L, pname);
        case GL_NUM_SHADER_BINARY_FORMATS: return GetInt(L, pname);
        case GL_PROGRAM_BINARY_FORMATS: return GetIntN(L, pname, GL_NUM_PROGRAM_BINARY_FORMATS);
        case GL_PROGRAM_PIPELINE_BINDING: return GetInt(L, pname);
        case GL_SHADER_BINARY_FORMATS: return GetEnumN(L, pname, GL_NUM_SHADER_BINARY_FORMATS, DOMAIN_SHADER_BINARY_FORMAT); //GL_VERSION_4_6
        case GL_SHADER_COMPILER: return GetBoolean(L, pname);
        case GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT: return GetInt(L, pname);
        case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: return GetInt(L, pname);
        /* Provoking Vertices */
        case GL_VIEWPORT_INDEX_PROVOKING_VERTEX:  return GetEnum(L, pname, DOMAIN_PROVOKE_MODE);
        case GL_LAYER_PROVOKING_VERTEX: return GetEnum(L, pname, DOMAIN_PROVOKE_MODE);
        case GL_PROVOKING_VERTEX: return GetEnum(L, pname, DOMAIN_PROVOKE_MODE);
        /* Rasterization */
        case GL_ALIASED_LINE_WIDTH_RANGE: return GetFloat2(L, pname);
        case GL_CULL_FACE: return GetBoolean(L, pname);
        case GL_CULL_FACE_MODE: return GetEnum(L, pname, DOMAIN_CULL_FACE_MODE); 
        case GL_FRONT_FACE: return GetEnum(L, pname, DOMAIN_FRONT_FACE_MODE); 
        case GL_LINE_SMOOTH: return GetBoolean(L, pname);
        case GL_LINE_WIDTH: return GetFloat(L, pname);
        case GL_POINT_FADE_THRESHOLD_SIZE: return GetFloat(L, pname);
        case GL_POINT_SIZE: return GetFloat(L, pname);
        case GL_POINT_SIZE_GRANULARITY: return GetFloat(L, pname);
        case GL_POINT_SIZE_RANGE: return GetFloat2(L, pname);
        case GL_POINT_SPRITE_COORD_ORIGIN: return GetEnum(L, pname, DOMAIN_ORIGIN);
        case GL_POLYGON_MODE: return GetEnum2(L, pname, DOMAIN_POLYGON_MODE_MODE); 
        case GL_POLYGON_OFFSET_FACTOR: return GetFloat(L, pname);
        case GL_POLYGON_OFFSET_FILL: return GetBoolean(L, pname);
        case GL_POLYGON_OFFSET_LINE: return GetBoolean(L, pname);
        case GL_POLYGON_OFFSET_POINT: return GetBoolean(L, pname);
        case GL_POLYGON_OFFSET_CLAMP: return GetFloat(L, pname); //GL_VERSION_4_6
        case GL_POLYGON_OFFSET_UNITS: return GetFloat(L, pname);
        case GL_POLYGON_SMOOTH: return GetBoolean(L, pname);
        case GL_PROGRAM_POINT_SIZE: return GetBoolean(L, pname);
        case GL_RASTERIZER_DISCARD: return GetBoolean(L, pname);
        case GL_SMOOTH_LINE_WIDTH_RANGE: return GetFloat2(L, pname);
        case GL_SMOOTH_LINE_WIDTH_GRANULARITY: return GetFloat(L, pname);
        case GL_SUBPIXEL_BITS: return GetInt(L, pname);
        /* Shader Execution */
        case GL_FRAGMENT_INTERPOLATION_OFFSET_BITS: return GetInt(L, pname);
        case GL_MAX_FRAGMENT_INTERPOLATION_OFFSET: return GetFloat(L, pname);
        case GL_MIN_FRAGMENT_INTERPOLATION_OFFSET: return GetFloat(L, pname);
        /* Shaders */
        case GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS:
        case GL_MAX_COMBINED_ATOMIC_COUNTERS:
        case GL_MAX_COMPUTE_ATOMIC_COUNTERS:
        case GL_MAX_FRAGMENT_ATOMIC_COUNTERS:
        case GL_MAX_GEOMETRY_ATOMIC_COUNTERS:
        case GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS:
        case GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS:
        case GL_MAX_VERTEX_ATOMIC_COUNTERS:
        case GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
        case GL_MAX_COMBINED_IMAGE_UNIFORMS:
        case GL_MAX_COMPUTE_IMAGE_UNIFORMS:
        case GL_MAX_FRAGMENT_IMAGE_UNIFORMS:
        case GL_MAX_GEOMETRY_IMAGE_UNIFORMS:
        case GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS:
        case GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS:
        case GL_MAX_VERTEX_IMAGE_UNIFORMS:
        case GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS:
        case GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS:
        case GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS:
        case GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS:
        case GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS:
        case GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS:
        case GL_MAX_COMPUTE_UNIFORM_COMPONENTS:
        case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
        case GL_MAX_GEOMETRY_UNIFORM_COMPONENTS:
        case GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS:
        case GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS:
        case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
        case GL_MAX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
        case GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS:
        case GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS:
        case GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS:
        case GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS:
        case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_COMBINED_UNIFORM_BLOCKS:
        case GL_MAX_COMPUTE_UNIFORM_BLOCKS:
        case GL_MAX_FRAGMENT_UNIFORM_BLOCKS:
        case GL_MAX_GEOMETRY_UNIFORM_BLOCKS:
        case GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS:
        case GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS:
        case GL_MAX_VERTEX_UNIFORM_BLOCKS: return GetInt(L, pname);
        /* Compute Shaders */
        case GL_DISPATCH_INDIRECT_BUFFER_BINDING: return GetInt(L, pname);
        case GL_MAX_COMPUTE_SHARED_MEMORY_SIZE: return GetInt(L, pname);
        case GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: return GetInt(L, pname);
        case GL_MAX_COMPUTE_WORK_GROUP_COUNT: return GetIntIndex(L, pname);
        case GL_MAX_COMPUTE_WORK_GROUP_SIZE: return GetIntIndex(L, pname);
        /* Fragment Shaders */
        case GL_MAX_FRAGMENT_INPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_FRAGMENT_UNIFORM_VECTORS: return GetInt(L, pname);
        case GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET: return GetInt(L, pname);
        case GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET: return GetInt(L, pname);
        /* Geometry Shaders */
        case GL_MAX_GEOMETRY_INPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_GEOMETRY_OUTPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_GEOMETRY_OUTPUT_VERTICES: return GetInt(L, pname);
        case GL_MAX_GEOMETRY_SHADER_INVOCATIONS: return GetInt(L, pname);
        case GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_VERTEX_STREAMS: return GetInt(L, pname);
        /* Tessellation Control Shaders */
        case GL_MAX_PATCH_VERTICES: return GetInt(L, pname);
        case GL_MAX_TESS_CONTROL_INPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_TESS_GEN_LEVEL: return GetInt(L, pname);
        case GL_MAX_TESS_PATCH_COMPONENTS: return GetInt(L, pname);
        case GL_PATCH_DEFAULT_INNER_LEVEL: return GetFloat2(L, pname);
        case GL_PATCH_DEFAULT_OUTER_LEVEL: return GetFloat4(L, pname);
        case GL_PATCH_VERTICES: return GetInt(L, pname);
        /* Tessellation Evaluation Shaders */
        case GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS: return GetInt(L, pname);
        /* Vertex Shaders */
        case GL_MAX_VERTEX_ATTRIBS: return GetInt(L, pname);
        case GL_MAX_VERTEX_OUTPUT_COMPONENTS: return GetInt(L, pname);
        case GL_MAX_VERTEX_UNIFORM_VECTORS: return GetInt(L, pname);
        /* Textures */
        case GL_ACTIVE_TEXTURE: return GetActiveTexture(L, pname); 
        case GL_COMPRESSED_TEXTURE_FORMATS:
                return GetEnumN(L, pname, GL_NUM_COMPRESSED_TEXTURE_FORMATS, DOMAIN_INTERNAL_FORMAT);
        case GL_MAX_3D_TEXTURE_SIZE: return GetInt(L, pname);
        case GL_MAX_ARRAY_TEXTURE_LAYERS: return GetInt(L, pname);
        case GL_MAX_CUBE_MAP_TEXTURE_SIZE: return GetInt(L, pname);
        case GL_MAX_RECTANGLE_TEXTURE_SIZE: return GetInt(L, pname);
        case GL_MAX_RENDERBUFFER_SIZE: return GetInt(L, pname);
        case GL_MAX_TEXTURE_BUFFER_SIZE: return GetInt(L, pname);
        case GL_MAX_TEXTURE_LOD_BIAS: return GetFloat(L, pname);
        case GL_MAX_TEXTURE_SIZE: return GetInt(L, pname);
        case GL_MAX_TEXTURE_MAX_ANISOTROPY: return GetFloat(L, pname); //GL_VERSION_4_6
        case GL_NUM_COMPRESSED_TEXTURE_FORMATS: return GetInt(L, pname);
        case GL_SAMPLER_BINDING: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_1D: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_1D_ARRAY: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_2D: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_2D_ARRAY: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_2D_MULTISAMPLE: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_3D: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_BUFFER: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_CUBE_MAP: return GetInt(L, pname);
        case GL_TEXTURE_BINDING_RECTANGLE: return GetInt(L, pname);
        case GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT: return GetInt(L, pname);
        case GL_TEXTURE_CUBE_MAP_SEAMLESS: return GetBoolean(L, pname);
        /* Transformation State */
        case GL_CLIP_DISTANCE0:
        case GL_CLIP_DISTANCE1:
        case GL_CLIP_DISTANCE2:
        case GL_CLIP_DISTANCE3:
        case GL_CLIP_DISTANCE4:
        case GL_CLIP_DISTANCE5:
        case GL_CLIP_DISTANCE6:
        case GL_CLIP_DISTANCE7: return GetBoolean(L, pname);
        case GL_DEPTH_CLAMP: return GetBoolean(L, pname);
        case GL_DEPTH_RANGE: return GetFloat2(L, pname);
        case GL_MAX_CLIP_DISTANCES: return GetInt(L, pname);
        case GL_MAX_VIEWPORT_DIMS: return GetFloat2(L, pname);
        case GL_MAX_VIEWPORTS: return GetInt(L, pname);
        case GL_TRANSFORM_FEEDBACK_BINDING: return GetInt(L, pname);
        case GL_VIEWPORT: return GetInt4OptIndex(L, pname);
        case GL_VIEWPORT_BOUNDS_RANGE: return GetFloat2(L, pname);
        case GL_VIEWPORT_SUBPIXEL_BITS: return GetInt(L, pname);
        /* Vertex Arrays */
        case GL_MAX_ELEMENT_INDEX: return GetInt(L, pname);
        case GL_MAX_ELEMENTS_INDICES: return GetInt(L, pname);
        case GL_MAX_ELEMENTS_VERTICES: return GetInt(L, pname);
        case GL_PRIMITIVE_RESTART: return GetBoolean(L, pname);
        case GL_PRIMITIVE_RESTART_FIXED_INDEX:  return GetBoolean(L, pname);
        case GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED:  return GetBoolean(L, pname);
        case GL_PRIMITIVE_RESTART_INDEX: return GetInt(L, pname);
        case GL_VERTEX_BINDING_DIVISOR: return GetIntIndex(L, pname);
        case GL_VERTEX_BINDING_OFFSET: return GetIntIndex(L, pname);
        case GL_VERTEX_BINDING_STRIDE: return GetIntIndex(L, pname);
//      case GL_MAX_VARYING_COMPONENTS = GL_MAX_VARYING_FLOATS
        case GL_MAX_VARYING_FLOATS: return GetInt(L, pname);
        case GL_RESET_NOTIFICATION_STRATEGY: return GetEnum(L, pname, DOMAIN_RESET_NOTIFICATION_STRATEGY);
        default:
            luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


static int GetGraphicsResetStatus(lua_State *L)
    {
    GLenum status = glGetGraphicsResetStatus();
    pushgraphicsresetstatus(L, status);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "get", Get },
        { "get_graphics_reset_status", GetGraphicsResetStatus },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_get(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }



