/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Stefano Trettel
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
 | Code<->string map for enumerations                                           |
 *------------------------------------------------------------------------------*/


/* code <-> string record */
#define rec_t struct rec_s
struct rec_s {
    RB_ENTRY(rec_s) CodeEntry;
    RB_ENTRY(rec_s) StringEntry;
    uint32_t domain;
    uint32_t code;  /* (domain, code) = search key in code tree */
    char     *str;  /* (domain, str) = search key in string tree */
};

/* compare functions */
static int cmp_code(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return (rec1->code < rec2->code ? -1 : rec1->code > rec2->code);
    } 

static int cmp_str(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return strcmp(rec1->str, rec2->str);
    } 

static RB_HEAD(CodeTree, rec_s) CodeHead = RB_INITIALIZER(&CodeHead);
RB_PROTOTYPE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 
RB_GENERATE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 

static RB_HEAD(StringTree, rec_s) StringHead = RB_INITIALIZER(&StringHead);
RB_PROTOTYPE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
RB_GENERATE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
 
static rec_t *code_remove(rec_t *rec) 
    { return RB_REMOVE(CodeTree, &CodeHead, rec); }
static rec_t *code_insert(rec_t *rec) 
    { return RB_INSERT(CodeTree, &CodeHead, rec); }
static rec_t *code_search(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_FIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_first(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_NFIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_next(rec_t *rec)
    { return RB_NEXT(CodeTree, &CodeHead, rec); }
#if 0
static rec_t *code_prev(rec_t *rec)
    { return RB_PREV(CodeTree, &CodeHead, rec); }
static rec_t *code_min(void)
    { return RB_MIN(CodeTree, &CodeHead); }
static rec_t *code_max(void)
    { return RB_MAX(CodeTree, &CodeHead); }
static rec_t *code_root(void)
    { return RB_ROOT(&CodeHead); }
#endif
 
static rec_t *str_remove(rec_t *rec) 
    { return RB_REMOVE(StringTree, &StringHead, rec); }
static rec_t *str_insert(rec_t *rec) 
    { return RB_INSERT(StringTree, &StringHead, rec); }
static rec_t *str_search(uint32_t domain, const char* str) 
    { rec_t tmp; tmp.domain = domain; tmp.str = (char*)str; return RB_FIND(StringTree, &StringHead, &tmp); }
#if 0
static rec_t *str_first(uint32_t domain, const char* str ) 
    { rec_t tmp; tmp.domain = domain; tmp.str = str; return RB_NFIND(StringTree, &StringHead, &tmp); }
static rec_t *str_next(rec_t *rec)
    { return RB_NEXT(StringTree, &StringHead, rec); }
static rec_t *str_prev(rec_t *rec)
    { return RB_PREV(StringTree, &StringHead, rec); }
static rec_t *str_min(void)
    { return RB_MIN(StringTree, &StringHead); }
static rec_t *str_max(void)
    { return RB_MAX(StringTree, &StringHead); }
static rec_t *str_root(void)
    { return RB_ROOT(&StringHead); }
#endif


static int enums_new(lua_State *L, uint32_t domain, uint32_t code, const char *str)
    {
    rec_t *rec;
    if((rec = (rec_t*)Malloc(L, sizeof(rec_t))) == NULL) 
        return luaL_error(L, errstring(ERR_MEMORY));

    memset(rec, 0, sizeof(rec_t));
    rec->domain = domain;
    rec->code = code;
    rec->str = Strdup(L, str);
    if(code_search(domain, code) || str_search(domain, str))
        { 
        Free(L, rec->str);
        Free(L, rec); 
        return unexpected(L); /* duplicate value */
        }
    code_insert(rec);
    str_insert(rec);
    return 0;
    }

static void enums_free(lua_State *L, rec_t* rec)
    {
    if(code_search(rec->domain, rec->code) == rec)
        code_remove(rec);
    if(str_search(rec->domain, rec->str) == rec)
        str_remove(rec);
    Free(L, rec->str);
    Free(L, rec);   
    }

void enums_free_all(lua_State *L)
    {
    rec_t *rec;
    while((rec = code_first(0, 0)))
        enums_free(L, rec);
    }

#if 0
uint32_t enums_code(uint32_t domain, const char *str, int* found)
    {
    rec_t *rec = str_search(domain, str);
    if(!rec)
        { *found = 0; return 0; }
    *found = 1; 
    return rec->code;
    }

const char* enums_string(uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);
    if(!rec)
        return NULL;
    return rec->str;
    }

#endif


uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err)
    {
    rec_t *rec;
    const char *s = luaL_optstring(L, arg, NULL);

    if(!s)
        { *err = ERR_NOTPRESENT; return 0; }

    rec = str_search(domain, s);
    if(!rec)
        { *err = ERR_VALUE; return 0; }
    
    *err = ERR_SUCCESS;
    return rec->code;
    }

uint32_t enums_check(lua_State *L, uint32_t domain, int arg)
    {
    rec_t *rec;
    const char *s = luaL_checkstring(L, arg);

    rec = str_search(domain, s);
    if(!rec)
        return luaL_argerror(L, arg, badvalue(L, s));
    
    return rec->code;
    }

int enums_push(lua_State *L, uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);

    if(!rec)
        return unexpected(L);

    lua_pushstring(L, rec->str);
    return 1;
    }

int enums_values(lua_State *L, uint32_t domain)
    {
    int i;
    rec_t *rec;

    lua_newtable(L);
    i = 1;
    rec = code_first(domain, 0);
    while(rec)
        {
        if(rec->domain == domain)
            {
            lua_pushstring(L, rec->str);
            lua_rawseti(L, -2, i++);
            }
        rec = code_next(rec);
        }

    return 1;
    }


uint32_t* enums_checklist(lua_State *L, uint32_t domain, int arg, uint32_t *count, int *err)
    {
    uint32_t* list;
    uint32_t i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_NOTPRESENT; return NULL; }

    list = (uint32_t*)MallocNoErr(L, sizeof(uint32_t) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        list[i] = enums_test(L, domain, -1, err);
        lua_pop(L, 1);
        if(*err)
            { Free(L, list); *count = 0; return NULL; }
        }
    return list;
    }

void enums_freelist(lua_State *L, uint32_t *list)
    {
    if(!list)
        return;
    Free(L, list);
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/

static int Enum(lua_State *L)
/* { strings } = cl.enum('type') lists all the values for a given enum type */
    { 
    const char *s = luaL_checkstring(L, 1); 
#define CASE(xxx) if(strcmp(s, ""#xxx) == 0) return values##xxx(L)
    CASE(shadertype);
    CASE(stagepname);
    CASE(subroutinepname);
    CASE(pipelinepname);
    CASE(transformtarget);
    CASE(vertexpname);
    CASE(type);
    CASE(programpname);
    CASE(primmode);
    CASE(provokemode);
    CASE(origin);
    CASE(depth);
    CASE(transformpname);
    CASE(glsltype);
    CASE(renderbuffertarget);
    CASE(renderbufferpname);
    CASE(internalformat);
    CASE(format);
    CASE(texturetarget);
    CASE(texturepname);
    CASE(depthstencil);
    CASE(comparefunc);
    CASE(comparemode);
    CASE(rgba);
    CASE(wrap);
    CASE(magfilter);
    CASE(minfilter);
    CASE(imageformatcompatibility);
    CASE(levelpname);
    CASE(stencilfunc);
    CASE(face);
    CASE(stencilop);
    CASE(blendmode);
    CASE(blendfactor);
    CASE(logicop);
    CASE(framebuffertarget);
    CASE(framebufferpname);
    CASE(framebufferstatus);
    CASE(framebuffergpname);
    CASE(framebufferapname);
    CASE(objecttype);
    CASE(componenttype);
    CASE(colorencoding);
    CASE(drawmode);
    CASE(drawtype);
    CASE(conditionalrendermode);
    CASE(filter);
    CASE(clamptarget);
    CASE(clamp);
    CASE(imagetarget);
    CASE(pixelstorepname);
    CASE(uniformtype);
    CASE(matrixdimensions);
    CASE(buffertarget);
    CASE(bufferrangetarget);
    CASE(bufferusage);
    CASE(bufferpname);
    CASE(bufferaccess);
    CASE(shaderpname);
    CASE(precisionformat);
    CASE(condition);
    CASE(syncstatus);
    CASE(syncpname);
    CASE(interface);
    CASE(interfacepname);
    CASE(resourceproperty);
    CASE(samplerpname);
    CASE(capability);
    CASE(patchpname);
    CASE(hinttarget);
    CASE(hintmode);
    CASE(querytarget);
    CASE(querypname);
    CASE(queryobjectpname);
    CASE(stringname);
    CASE(uniformpname);
    CASE(blockpname);
    CASE(acbpname);
    CASE(buffer);
    CASE(clearbuffer);
    CASE(programparameterpname);
    CASE(geometryinputtype);
    CASE(geometryoutputtype);
    CASE(tessgenmode);
    CASE(tessgenspacing);
    CASE(tessgenvertexorder);
    CASE(buffermode);
    CASE(debugsource);
    CASE(debugtype);
    CASE(debugseverity);
    CASE(debugidentifier);
    CASE(textureaccess);
    CASE(pname);
    CASE(resetnotificationstrategy);
    CASE(graphicsresetstatus);
    CASE(polygonmodeface);
    CASE(polygonmodemode);
    CASE(frontfacemode);
    CASE(cullfacemode);
    CASE(multisamplepname);
    CASE(pointpname);
    CASE(intformatpname);
    CASE(supportedop);
    CASE(imagecompatibilityclass);
    CASE(imageformatcomptype);
    CASE(viewcompatibilitytype);
    CASE(shaderbinaryformat);
#undef CASE
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "enum", Enum },
        { NULL, NULL } /* sentinel */
    };


void moongl_open_enums(lua_State *L)
    {
    uint32_t domain;

    luaL_setfuncs(L, Functions, 0);

#if 0 //@@
    /* Add all the code<->string mappings and the cl.XXX constant strings */
#define ADD(what, s) do { enums_new(L, domain, NONCL_##what, s); } while(0)
    domain = DOMAIN_NONCL_TYPE; 
    ADD(TYPE_CHAR, "char");
    ADD(TYPE_UCHAR, "uchar");
    ADD(TYPE_SHORT, "short");
    ADD(TYPE_USHORT, "ushort");
    ADD(TYPE_INT, "int");
    ADD(TYPE_UINT, "uint");
    ADD(TYPE_LONG, "long");
    ADD(TYPE_ULONG, "ulong");
    ADD(TYPE_HALF, "half");
    ADD(TYPE_FLOAT, "float");
    ADD(TYPE_DOUBLE, "double");
#undef ADD
#endif

#define ADD(what, s) do {                               \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, GL_##what, s);                 \
} while(0)

    domain = DOMAIN_SHADER_TYPE;
    ADD(COMPUTE_SHADER, "compute");
    ADD(FRAGMENT_SHADER, "fragment");
    ADD(GEOMETRY_SHADER, "geometry");
    ADD(VERTEX_SHADER, "vertex");
    ADD(TESS_EVALUATION_SHADER, "tess evaluation");
    ADD(TESS_CONTROL_SHADER, "tess control");

    domain = DOMAIN_STAGE_PNAME;
    ADD(ACTIVE_SUBROUTINES, "subroutines");
    ADD(ACTIVE_SUBROUTINE_UNIFORMS, "subroutine uniforms");
    ADD(ACTIVE_SUBROUTINE_MAX_LENGTH, "subroutine max length");
    ADD(ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, "subroutine uniform locations");
    ADD(ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH, "subroutine uniform max length");

    domain = DOMAIN_SUBROUTINE_PNAME;
    ADD(NUM_COMPATIBLE_SUBROUTINES, "num compatible subroutines");
    ADD(COMPATIBLE_SUBROUTINES, "compatible subroutines");
    ADD(UNIFORM_SIZE, "uniform size");
    ADD(UNIFORM_NAME_LENGTH, "uniform name length");

    domain = DOMAIN_PIPELINE_PNAME;
    ADD(ACTIVE_PROGRAM, "active program");
    ADD(VALIDATE_STATUS, "validate status");
    ADD(VERTEX_SHADER, "vertex shader");
    ADD(GEOMETRY_SHADER, "geometry shader");
    ADD(FRAGMENT_SHADER, "fragment shader");
    ADD(COMPUTE_SHADER, "compute shader");
    ADD(TESS_CONTROL_SHADER, "tess control shader");
    ADD(TESS_EVALUATION_SHADER, "tess evaluation shader");
    ADD(INFO_LOG_LENGTH, "info log length");

    domain = DOMAIN_TRANSFORM_TARGET;
    ADD(TRANSFORM_FEEDBACK, "transform feedback");

    domain = DOMAIN_VERTEX_PNAME;
    ADD(VERTEX_ATTRIB_ARRAY_ENABLED, "enabled");
    ADD(VERTEX_ATTRIB_ARRAY_SIZE, "size");
    ADD(VERTEX_ATTRIB_ARRAY_STRIDE, "stride");
    ADD(VERTEX_ATTRIB_ARRAY_TYPE, "type");
    ADD(VERTEX_ATTRIB_ARRAY_NORMALIZED, "normalized");
    ADD(VERTEX_ATTRIB_ARRAY_INTEGER, "integer");
    ADD(VERTEX_ATTRIB_ARRAY_LONG, "long");
    ADD(VERTEX_ATTRIB_ARRAY_DIVISOR, "divisor");
    ADD(VERTEX_ATTRIB_RELATIVE_OFFSET, "relative offset");
    ADD(VERTEX_BINDING_OFFSET, "binding offset");
    ADD(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, "buffer binding");
    ADD(VERTEX_ATTRIB_BINDING, "attrib binding");
    ADD(CURRENT_VERTEX_ATTRIB, "current attrib");
    ADD(ELEMENT_ARRAY_BUFFER_BINDING, "element array buffer binding");

    domain = DOMAIN_TYPE;
    ADD(NONE, "none");
    ADD(UNSIGNED_BYTE, "ubyte");
    ADD(BYTE, "byte");
    ADD(UNSIGNED_SHORT, "ushort");
    ADD(SHORT, "short");
    ADD(UNSIGNED_INT, "uint");
    ADD(INT, "int");
    ADD(FIXED, "fixed");
    ADD(HALF_FLOAT, "half float");
    ADD(FLOAT, "float");
    ADD(DOUBLE, "double");
    ADD(UNSIGNED_BYTE_3_3_2, "ubyte 3 3 2");
    ADD(UNSIGNED_BYTE_2_3_3_REV, "ubyte 2 3 3 rev");
    ADD(UNSIGNED_SHORT_5_6_5, "ushort 5 6 5");
    ADD(UNSIGNED_SHORT_5_6_5_REV, "ushort 5 6 5 rev");
    ADD(UNSIGNED_SHORT_4_4_4_4, "ushort 4 4 4 4");
    ADD(UNSIGNED_SHORT_4_4_4_4_REV, "ushort 4 4 4 4 rev");
    ADD(UNSIGNED_SHORT_5_5_5_1, "ushort 5 5 5 1");
    ADD(UNSIGNED_SHORT_1_5_5_5_REV, "ushort 1 5 5 5 rev");
    ADD(UNSIGNED_INT_8_8_8_8, "uint 8 8 8 8");
    ADD(UNSIGNED_INT_8_8_8_8_REV, "uint 8 8 8 8 rev");
    ADD(UNSIGNED_INT_10_10_10_2, "uint 10 10 10 2");
    ADD(UNSIGNED_INT_2_10_10_10_REV, "uint 2 10 10 10 rev");
    ADD(UNSIGNED_INT_24_8, "uint 24 8");
    ADD(UNSIGNED_INT_10F_11F_11F_REV, "uint 10f 11f 11f rev");
    ADD(UNSIGNED_INT_5_9_9_9_REV, "uint 5 9 9 9 rev");
    ADD(FLOAT_32_UNSIGNED_INT_24_8_REV, "float 32 uint 24 8 rev");
    ADD(INT_2_10_10_10_REV, "int 2 10 10 10 rev");

    domain = DOMAIN_PROGRAM_PNAME;
    ADD(ACTIVE_ATOMIC_COUNTER_BUFFERS, "active atomic counter buffers");
    ADD(ACTIVE_ATTRIBUTES, "active attributes");
    ADD(ACTIVE_ATTRIBUTE_MAX_LENGTH, "active attribute max length");
    ADD(ACTIVE_UNIFORMS, "active uniforms");
    ADD(ACTIVE_UNIFORM_BLOCKS, "active uniform blocks");
    ADD(ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, "active uniform block max name length");
    ADD(ACTIVE_UNIFORM_MAX_LENGTH, "active uniform max length");
    ADD(ATTACHED_SHADERS, "attached shaders");
    ADD(VALIDATE_STATUS, "validate status");
    ADD(COMPUTE_WORK_GROUP_SIZE, "compute work group size");
    ADD(DELETE_STATUS, "delete status");
    ADD(GEOMETRY_INPUT_TYPE, "geometry input type");
    ADD(GEOMETRY_OUTPUT_TYPE, "geometry output type");
    ADD(GEOMETRY_SHADER_INVOCATIONS, "geometry shader invocations");
    ADD(GEOMETRY_VERTICES_OUT, "geometry vertices out");
    ADD(INFO_LOG_LENGTH, "info log length");
    ADD(LINK_STATUS, "link status");
    ADD(PROGRAM_SEPARABLE, "program separable");
    ADD(PROGRAM_BINARY_RETRIEVABLE_HINT, "program binary retrievable hint");
    ADD(TESS_CONTROL_OUTPUT_VERTICES, "tess control output vertices");
    ADD(TESS_GEN_MODE, "tess gen mode");
    ADD(TESS_GEN_SPACING, "tess gen spacing");
    ADD(TESS_GEN_VERTEX_ORDER, "tess gen vertex order");
    ADD(TESS_GEN_POINT_MODE, "tess gen point mode");
    ADD(PROGRAM_BINARY_LENGTH, "program binary length");
    ADD(TRANSFORM_FEEDBACK_BUFFER_MODE, "transform feedback buffer mode");
    ADD(TRANSFORM_FEEDBACK_VARYINGS, "transform feedback varyings");
    ADD(TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, "transform feedback varying max length");

    domain = DOMAIN_PRIM_MODE;
    ADD(TRIANGLES, "triangles");
    ADD(LINES, "lines");
    ADD(POINTS, "points");

    domain = DOMAIN_PROVOKE_MODE;
    ADD(FIRST_VERTEX_CONVENTION, "first vertex convention");
    ADD(LAST_VERTEX_CONVENTION, "last vertex convention");
    ADD(PROVOKING_VERTEX, "provoking vertex");
    ADD(UNDEFINED_VERTEX, "undefined vertex");

    domain = DOMAIN_ORIGIN;
    ADD(LOWER_LEFT, "lower left");
    ADD(UPPER_LEFT, "upper left");

    domain = DOMAIN_DEPTH;
    ADD(NEGATIVE_ONE_TO_ONE, "negative one to one");
    ADD(ZERO_TO_ONE, "zero to one");

    domain = DOMAIN_TRANSFORM_PNAME;
    ADD(TRANSFORM_FEEDBACK_PAUSED, "paused");
    ADD(TRANSFORM_FEEDBACK_ACTIVE, "active");
    ADD(TRANSFORM_FEEDBACK_BUFFER_BINDING, "buffer binding");
    ADD(TRANSFORM_FEEDBACK_BUFFER_START, "buffer start");
    ADD(TRANSFORM_FEEDBACK_BUFFER_SIZE, "buffer size");

    domain = DOMAIN_GLSL_TYPE;
    ADD(FLOAT, "float");
    ADD(FLOAT_VEC2, "vec2");
    ADD(FLOAT_VEC3, "vec3");
    ADD(FLOAT_VEC4, "vec4");
    ADD(DOUBLE, "double");
    ADD(DOUBLE_VEC2, "dvec2");
    ADD(DOUBLE_VEC3, "dvec3");
    ADD(DOUBLE_VEC4, "dvec4");
    ADD(INT, "int");
    ADD(INT_VEC2, "ivec2");
    ADD(INT_VEC3, "ivec3");
    ADD(INT_VEC4, "ivec4");
    ADD(UNSIGNED_INT, "uint");
    ADD(UNSIGNED_INT_VEC2, "uvec2");
    ADD(UNSIGNED_INT_VEC3, "uvec3");
    ADD(UNSIGNED_INT_VEC4, "uvec4");
    ADD(BOOL, "bool");
    ADD(BOOL_VEC2, "bvec2");
    ADD(BOOL_VEC3, "bvec3");
    ADD(BOOL_VEC4, "bvec4");
    ADD(FLOAT_MAT2, "mat2");
    ADD(FLOAT_MAT3, "mat3");
    ADD(FLOAT_MAT4, "mat4");
    ADD(FLOAT_MAT2x3, "mat2x3");
    ADD(FLOAT_MAT2x4, "mat2x4");
    ADD(FLOAT_MAT3x2, "mat3x2");
    ADD(FLOAT_MAT3x4, "mat3x4");
    ADD(FLOAT_MAT4x2, "mat4x2");
    ADD(FLOAT_MAT4x3, "mat4x3");
    ADD(DOUBLE_MAT2, "dmat2");
    ADD(DOUBLE_MAT3, "dmat3");
    ADD(DOUBLE_MAT4, "dmat4");
    ADD(DOUBLE_MAT2x3, "dmat2x3");
    ADD(DOUBLE_MAT2x4, "dmat2x4");
    ADD(DOUBLE_MAT3x2, "dmat3x2");
    ADD(DOUBLE_MAT3x4, "dmat3x4");
    ADD(DOUBLE_MAT4x2, "dmat4x2");
    ADD(DOUBLE_MAT4x3, "dmat4x3");
    ADD(SAMPLER_1D, "sampler1D");
    ADD(SAMPLER_2D, "sampler2D");
    ADD(SAMPLER_3D, "sampler3D");
    ADD(SAMPLER_CUBE, "samplerCube");
    ADD(SAMPLER_1D_SHADOW, "sampler1DShadow");
    ADD(SAMPLER_2D_SHADOW, "sampler2DShadow");
    ADD(SAMPLER_1D_ARRAY, "sampler1DArray");
    ADD(SAMPLER_2D_ARRAY, "sampler2DArray");
    ADD(SAMPLER_1D_ARRAY_SHADOW, "sampler1DArrayShadow");
    ADD(SAMPLER_2D_ARRAY_SHADOW, "sampler2DArrayShadow");
    ADD(SAMPLER_2D_MULTISAMPLE, "sampler2DMS");
    ADD(SAMPLER_2D_MULTISAMPLE_ARRAY, "sampler2DMSArray");
    ADD(SAMPLER_CUBE_SHADOW, "samplerCubeShadow");
    ADD(SAMPLER_BUFFER, "samplerBuffer");
    ADD(SAMPLER_2D_RECT, "sampler2DRect");
    ADD(SAMPLER_2D_RECT_SHADOW, "sampler2DRectShadow");
    ADD(INT_SAMPLER_1D, "isampler1D");
    ADD(INT_SAMPLER_2D, "isampler2D");
    ADD(INT_SAMPLER_3D, "isampler3D");
    ADD(INT_SAMPLER_CUBE, "isamplerCube");
    ADD(INT_SAMPLER_1D_ARRAY, "isampler1DArray");
    ADD(INT_SAMPLER_2D_ARRAY, "isampler2DArray");
    ADD(INT_SAMPLER_2D_MULTISAMPLE, "isampler2DMS");
    ADD(INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "isampler2DMSArray");
    ADD(INT_SAMPLER_BUFFER, "isamplerBuffer");
    ADD(INT_SAMPLER_2D_RECT, "isampler2DRect");
    ADD(UNSIGNED_INT_SAMPLER_1D, "usampler1D");
    ADD(UNSIGNED_INT_SAMPLER_2D, "usampler2D");
    ADD(UNSIGNED_INT_SAMPLER_3D, "usampler3D");
    ADD(UNSIGNED_INT_SAMPLER_CUBE, "usamplerCube");
    ADD(UNSIGNED_INT_SAMPLER_1D_ARRAY, "usampler1DArray");
    ADD(UNSIGNED_INT_SAMPLER_2D_ARRAY, "usampler2DArray");
    ADD(UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, "usampler2DMS");
    ADD(UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray");
    ADD(UNSIGNED_INT_SAMPLER_BUFFER, "usamplerBuffer");
    ADD(UNSIGNED_INT_SAMPLER_2D_RECT, "usampler2DRect");
    ADD(IMAGE_1D, "image1D");
    ADD(IMAGE_2D, "image2D");
    ADD(IMAGE_3D, "image3D");
    ADD(IMAGE_2D_RECT, "image2DRect");
    ADD(IMAGE_CUBE, "imageCube");
    ADD(IMAGE_BUFFER, "imageBuffer");
    ADD(IMAGE_1D_ARRAY, "image1DArray");
    ADD(IMAGE_2D_ARRAY, "image2DArray");
    ADD(IMAGE_2D_MULTISAMPLE, "image2DMS");
    ADD(IMAGE_2D_MULTISAMPLE_ARRAY, "image2DMSArray");
    ADD(INT_IMAGE_1D, "iimage1D");
    ADD(INT_IMAGE_2D, "iimage2D");
    ADD(INT_IMAGE_3D, "iimage3D");
    ADD(INT_IMAGE_2D_RECT, "iimage2DRect");
    ADD(INT_IMAGE_CUBE, "iimageCube");
    ADD(INT_IMAGE_BUFFER, "iimageBuffer");
    ADD(INT_IMAGE_1D_ARRAY, "iimage1DArray");
    ADD(INT_IMAGE_2D_ARRAY, "iimage2DArray");
    ADD(INT_IMAGE_2D_MULTISAMPLE, "iimage2DMS");
    ADD(INT_IMAGE_2D_MULTISAMPLE_ARRAY, "iimage2DMSArray");
    ADD(UNSIGNED_INT_IMAGE_1D, "uimage1D");
    ADD(UNSIGNED_INT_IMAGE_2D, "uimage2D");
    ADD(UNSIGNED_INT_IMAGE_3D, "uimage3D");
    ADD(UNSIGNED_INT_IMAGE_2D_RECT, "uimage2DRect");
    ADD(UNSIGNED_INT_IMAGE_CUBE, "uimageCube");
    ADD(UNSIGNED_INT_IMAGE_BUFFER, "uimageBuffer");
    ADD(UNSIGNED_INT_IMAGE_1D_ARRAY, "uimage1DArray");
    ADD(UNSIGNED_INT_IMAGE_2D_ARRAY, "uimage2DArray");
    ADD(UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, "uimage2DMS");
    ADD(UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, "uimage2DMSArray");
    ADD(UNSIGNED_INT_ATOMIC_COUNTER, "atomic_uint");

    domain = DOMAIN_RENDERBUFFER_TARGET;
    ADD(RENDERBUFFER, "renderbuffer");

    domain = DOMAIN_RENDERBUFFER_PNAME;
    ADD(RENDERBUFFER_WIDTH, "width");
    ADD(RENDERBUFFER_HEIGHT, "height");
    ADD(RENDERBUFFER_INTERNAL_FORMAT, "internal format");
    ADD(RENDERBUFFER_SAMPLES, "samples");
    ADD(RENDERBUFFER_RED_SIZE, "red size");
    ADD(RENDERBUFFER_GREEN_SIZE, "green size");
    ADD(RENDERBUFFER_BLUE_SIZE, "blue size");
    ADD(RENDERBUFFER_ALPHA_SIZE, "alpha size");
    ADD(RENDERBUFFER_DEPTH_SIZE, "depth size");
    ADD(RENDERBUFFER_STENCIL_SIZE, "stencil size");

    domain = DOMAIN_INTERNAL_FORMAT;
    ADD(NONE,  "none");
    /* base int. formats */
    ADD(STENCIL_INDEX,  "stencil index");
    ADD(DEPTH_COMPONENT,  "depth component");
    ADD(DEPTH_STENCIL, "depth stencil");
    ADD(RED, "red");
    ADD(RG,  "rg");
    ADD(RGB,  "rgb");
    ADD(RGBA,  "rgba");
    /* sized int. formats */
    ADD(R8, "r8");
    ADD(R8_SNORM, "r8 snorm");
    ADD(R16, "r16");
    ADD(R16_SNORM, "r16 snorm");
    ADD(RG8, "rg8");
    ADD(RG8_SNORM, "rg8 snorm");
    ADD(RG16, "rg16");
    ADD(RG16_SNORM, "rg16 snorm");
    ADD(R3_G3_B2, "r3 g3 b2");
    ADD(RGB4, "rgb4");
    ADD(RGB5, "rgb5");
    ADD(RGB8, "rgb8");
    ADD(RGB8_SNORM, "rgb8 snorm");
    ADD(RGB10, "rgb10");
    ADD(RGB12, "rgb12");
    ADD(RGB16_SNORM, "rgb16 snorm");
    ADD(RGBA2, "rgba2");
    ADD(RGBA4, "rgba4");
    ADD(RGB5_A1, "rgb5 a1");
    ADD(RGBA8, "rgba8");
    ADD(RGBA8_SNORM, "rgba8 snorm");
    ADD(RGB10_A2, "rgb10 a2");
    ADD(RGB10_A2UI, "rgb10 a2ui");
    ADD(RGBA12, "rgba12");
    ADD(RGBA16, "rgba16");
    ADD(SRGB, "srgb");
    ADD(SRGB8, "srgb8");
    ADD(SRGB8_ALPHA8, "srgb8 alpha8");
    ADD(R16F, "r16f");
    ADD(RG16F, "rg16f");
    ADD(RGB16F, "rgb16f");
    ADD(RGBA16F, "rgba16f");
    ADD(R32F, "r32f");
    ADD(RG32F, "rg32f");
    ADD(RGB32F, "rgb32f");
    ADD(RGBA32F, "rgba32f");
    ADD(R11F_G11F_B10F, "r11f g11f b10f");
    ADD(RGB9_E5, "rgb9 e5");
    ADD(R8I, "r8i");
    ADD(R8UI, "r8ui");
    ADD(R16I, "r16i");
    ADD(R16UI, "r16ui");
    ADD(R32I, "r32i");
    ADD(R32UI, "r32ui");
    ADD(RG8I, "rg8i");
    ADD(RG8UI, "rg8ui");
    ADD(RG16I, "rg16i");
    ADD(RG16UI, "rg16ui");
    ADD(RG32I, "rg32i");
    ADD(RG32UI, "rg32ui");
    ADD(RGB8I, "rgb8i");
    ADD(RGB8UI, "rgb8ui");
    ADD(RGB16I, "rgb16i");
    ADD(RGB16UI, "rgb16ui");
    ADD(RGB32I, "rgb32i");
    ADD(RGB32UI, "rgb32ui");
    ADD(RGBA8I, "rgba8i");
    ADD(RGBA8UI, "rgba8ui");
    ADD(RGBA16I, "rgba16i");
    ADD(RGBA16UI, "rgba16ui");
    ADD(RGBA32I, "rgba32i");
    ADD(RGBA32UI, "rgba32ui");
    /* sized depth and stencil int. formats */
    ADD(DEPTH_COMPONENT16, "depth component16");
    ADD(DEPTH_COMPONENT24, "depth component24");
    ADD(DEPTH_COMPONENT32, "depth component32");
    ADD(DEPTH_COMPONENT32F, "depth component32f");
    ADD(DEPTH24_STENCIL8, "depth24 stencil8");
    ADD(DEPTH32F_STENCIL8, "depth32f stencil8");
    ADD(STENCIL_INDEX8, "stencil index8");
    /* compressed */
    ADD(COMPRESSED_RED, "compressed red");
    ADD(COMPRESSED_RG, "compressed rg");
    ADD(COMPRESSED_RGB, "compressed rgb");
    ADD(COMPRESSED_RGBA, "compressed rgba");
    ADD(COMPRESSED_SRGB, "compressed srgb");
    ADD(COMPRESSED_SRGB_ALPHA, "compressed srgb alpha");
    ADD(COMPRESSED_RED_RGTC1, "compressed red rgtc1");
    ADD(COMPRESSED_SIGNED_RED_RGTC1, "compressed signed red rgtc1");
    ADD(COMPRESSED_RG_RGTC2, "compressed rg rgtc2");
    ADD(COMPRESSED_SIGNED_RG_RGTC2, "compressed signed rg rgtc2");
    ADD(COMPRESSED_RGBA_BPTC_UNORM, "compressed rgba bptc unorm");
    ADD(COMPRESSED_SRGB_ALPHA_BPTC_UNORM, "compressed srgb alpha bptc unorm");
    ADD(COMPRESSED_RGB_BPTC_SIGNED_FLOAT, "compressed rgb bptc signed float");
    ADD(COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, "compressed rgb bptc unsigned float");
    /* S3TC compressed */
    ADD(COMPRESSED_RGB_S3TC_DXT1_EXT, "compressed rgb s3tc dxt1 ext");
    ADD(COMPRESSED_SRGB_S3TC_DXT1_EXT, "compressed srgb s3tc dxt1 ext");
    ADD(COMPRESSED_RGBA_S3TC_DXT1_EXT, "compressed rgba s3tc dxt1 ext");
    ADD(COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, "compressed srgb alpha s3tc dxt1 ext");
    ADD(COMPRESSED_RGBA_S3TC_DXT3_EXT, "compressed rgba s3tc dxt3 ext");
    ADD(COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, "compressed srgb alpha s3tc dxt3 ext");
    ADD(COMPRESSED_RGBA_S3TC_DXT5_EXT, "compressed rgba s3tc dxt5 ext");
    ADD(COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, "compressed srgb alpha s3tc dxt5 ext");
#if 0
    ADD(COMPRESSED_RGB8_ETC2, "compressed rgb8 etc2");
    ADD(COMPRESSED_SRGB8_ETC2, "compressed srgb8 etc2");
    ADD(COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, "compressed rgb8 punchthrough alpha1 etc2");
    ADD(COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, "compressed srgb8 punchthrough alpha1 etc2");
    ADD(COMPRESSED_RGBA8_ETC2_EAC, "compressed rgba8 etc2 eac");
    ADD(COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, "compressed srgb8 alpha8 etc2 eac");
    ADD(COMPRESSED_R11_EAC, "compressed r11 eac");
    ADD(COMPRESSED_SIGNED_R11_EAC, "compressed signed r11 eac");
    ADD(COMPRESSED_RG11_EAC, "compressed rg11 eac");
    ADD(COMPRESSED_SIGNED_RG11_EAC, "compressed signed rg11 eac");
    ADD(COMPRESSED_RGBA_BPTC_UNORM_ARB, "compressed rgba bptc unorm arb");
    ADD(COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB, "compressed srgb alpha bptc unorm arb");
    ADD(COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB, "compressed rgb bptc signed float arb");
    ADD(COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, "compressed rgb bptc unsigned float arb");
    ADD(COMPRESSED_RGBA_ASTC_4x4_KHR, "compressed rgba astc 4x4 khr");
    ADD(COMPRESSED_RGBA_ASTC_5x4_KHR, "compressed rgba astc 5x4 khr");
    ADD(COMPRESSED_RGBA_ASTC_5x5_KHR, "compressed rgba astc 5x5 khr");
    ADD(COMPRESSED_RGBA_ASTC_6x5_KHR, "compressed rgba astc 6x5 khr");
    ADD(COMPRESSED_RGBA_ASTC_6x6_KHR, "compressed rgba astc 6x6 khr");
    ADD(COMPRESSED_RGBA_ASTC_8x5_KHR, "compressed rgba astc 8x5 khr");
    ADD(COMPRESSED_RGBA_ASTC_8x6_KHR, "compressed rgba astc 8x6 khr");
    ADD(COMPRESSED_RGBA_ASTC_8x8_KHR, "compressed rgba astc 8x8 khr");
    ADD(COMPRESSED_RGBA_ASTC_10x5_KHR, "compressed rgba astc 10x5 khr");
    ADD(COMPRESSED_RGBA_ASTC_10x6_KHR, "compressed rgba astc 10x6 khr");
    ADD(COMPRESSED_RGBA_ASTC_10x8_KHR, "compressed rgba astc 10x8 khr");
    ADD(COMPRESSED_RGBA_ASTC_10x10_KHR, "compressed rgba astc 10x10 khr");
    ADD(COMPRESSED_RGBA_ASTC_12x10_KHR, "compressed rgba astc 12x10 khr");
    ADD(COMPRESSED_RGBA_ASTC_12x12_KHR, "compressed rgba astc 12x12 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, "compressed srgb8 alpha8 astc 4x4 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, "compressed srgb8 alpha8 astc 5x4 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, "compressed srgb8 alpha8 astc 5x5 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, "compressed srgb8 alpha8 astc 6x5 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, "compressed srgb8 alpha8 astc 6x6 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, "compressed srgb8 alpha8 astc 8x5 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, "compressed srgb8 alpha8 astc 8x6 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, "compressed srgb8 alpha8 astc 8x8 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, "compressed srgb8 alpha8 astc 10x5 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, "compressed srgb8 alpha8 astc 10x6 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, "compressed srgb8 alpha8 astc 10x8 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, "compressed srgb8 alpha8 astc 10x10 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, "compressed srgb8 alpha8 astc 12x10 khr");
    ADD(COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, "compressed srgb8 alpha8 astc 12x12 khr");
#endif

    domain = DOMAIN_FORMAT;
    ADD(NONE, "none");
    ADD(STENCIL_INDEX, "stencil index");
    ADD(DEPTH_COMPONENT, "depth component");
    ADD(DEPTH_STENCIL, "depth stencil");
    ADD(RED, "red");
    ADD(GREEN, "green");
    ADD(BLUE, "blue");
    ADD(RG, "rg");
    ADD(RGB, "rgb");
    ADD(RGBA, "rgba");
    ADD(BGR, "bgr");
    ADD(BGRA, "bgra");
    ADD(RED_INTEGER, "red integer");
    ADD(GREEN_INTEGER, "green integer");
    ADD(BLUE_INTEGER,  "blue integer");
    ADD(RG_INTEGER, "rg integer");
    ADD(RGB_INTEGER, "rgb integer");
    ADD(RGBA_INTEGER, "rgba integer");
    ADD(BGR_INTEGER, "bgr integer");
    ADD(BGRA_INTEGER, "bgra integer");

    domain = DOMAIN_TEXTURE_TARGET;
    ADD(TEXTURE_1D, "1d");
    ADD(TEXTURE_2D, "2d");
    ADD(TEXTURE_3D, "3d");
    ADD(TEXTURE_1D_ARRAY, "1d array");
    ADD(TEXTURE_2D_ARRAY, "2d array");
    ADD(TEXTURE_RECTANGLE, "rectangle");
    ADD(TEXTURE_CUBE_MAP, "cube map");
    ADD(TEXTURE_CUBE_MAP_ARRAY, "cube map array");
    ADD(TEXTURE_2D_MULTISAMPLE, "2d multisample");
    ADD(TEXTURE_2D_MULTISAMPLE_ARRAY, "2d multisample array");
    ADD(TEXTURE_CUBE_MAP_POSITIVE_X,  "cube map positive x");
    ADD(TEXTURE_CUBE_MAP_POSITIVE_Y,  "cube map positive y");
    ADD(TEXTURE_CUBE_MAP_POSITIVE_Z,  "cube map positive z");
    ADD(TEXTURE_CUBE_MAP_NEGATIVE_X,  "cube map negative x");
    ADD(TEXTURE_CUBE_MAP_NEGATIVE_Y,  "cube map negative y");
    ADD(TEXTURE_CUBE_MAP_NEGATIVE_Z,  "cube map negative z");
    ADD(PROXY_TEXTURE_1D, "proxy 1d");
    ADD(PROXY_TEXTURE_2D, "proxy 2d");
    ADD(PROXY_TEXTURE_3D, "proxy 3d");
    ADD(PROXY_TEXTURE_1D_ARRAY, "proxy 1d array");
    ADD(PROXY_TEXTURE_2D_ARRAY, "proxy 2d array");
    ADD(PROXY_TEXTURE_RECTANGLE, "proxy rectangle");
    ADD(PROXY_TEXTURE_CUBE_MAP, "proxy cube map");
    ADD(PROXY_TEXTURE_CUBE_MAP_ARRAY, "proxy cube map array");
    ADD(PROXY_TEXTURE_2D_MULTISAMPLE, "proxy 2d multisample");
    ADD(PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY, "proxy 2d multisample array");
    ADD(TEXTURE_BUFFER, "buffer");
    ADD(RENDERBUFFER, /* for GetInternalFormat() only */ "renderbuffer");

    domain = DOMAIN_TEXTURE_PNAME;
    ADD(DEPTH_STENCIL_TEXTURE_MODE, "depth stencil mode");
    ADD(TEXTURE_BASE_LEVEL, "base level");
    ADD(TEXTURE_COMPARE_FUNC, "compare func");
    ADD(TEXTURE_COMPARE_MODE, "compare mode");
    ADD(TEXTURE_LOD_BIAS, "lod bias");
    ADD(TEXTURE_MIN_FILTER, "min filter");
    ADD(TEXTURE_MAG_FILTER, "mag filter");
    ADD(TEXTURE_MIN_LOD, "min lod");
    ADD(TEXTURE_MAX_LOD, "max lod");
    ADD(TEXTURE_MAX_LEVEL, "max level");
    ADD(TEXTURE_SWIZZLE_R, "swizzle r");
    ADD(TEXTURE_SWIZZLE_G, "swizzle g");
    ADD(TEXTURE_SWIZZLE_B, "swizzle b");
    ADD(TEXTURE_SWIZZLE_A, "swizzle a");
    ADD(TEXTURE_WRAP_S, "wrap s");
    ADD(TEXTURE_WRAP_T, "wrap t");
    ADD(TEXTURE_WRAP_R, "wrap r");
    ADD(TEXTURE_BORDER_COLOR, "border color");
    ADD(TEXTURE_SWIZZLE_RGBA, "swizzle rgba");
    ADD(TEXTURE_MAX_ANISOTROPY, "max anisotropy"); //GL_VERSION_4_6
    /* get only: */
    ADD(IMAGE_FORMAT_COMPATIBILITY_TYPE,  "image format compatibility type");
    ADD(TEXTURE_IMMUTABLE_FORMAT, "immutable format");
    ADD(TEXTURE_IMMUTABLE_LEVELS,  "immutable levels");
    ADD(TEXTURE_VIEW_MIN_LEVEL,  "view min level");
    ADD(TEXTURE_VIEW_NUM_LEVELS,  "view num levels");
    ADD(TEXTURE_VIEW_MIN_LAYER,  "view min layer");
    ADD(TEXTURE_VIEW_NUM_LAYERS, "view num layers");
    ADD(TEXTURE_TARGET, "target");

    domain = DOMAIN_DEPTH_STENCIL;
    ADD(DEPTH_COMPONENT, "depth");
    ADD(STENCIL_INDEX, "stencil");

    domain = DOMAIN_COMPARE_FUNC;
    ADD(NEVER, "never");
    ADD(LESS, "less");
    ADD(EQUAL, "equal");
    ADD(LEQUAL, "lequal");
    ADD(GREATER, "greater");
    ADD(NOTEQUAL, "notequal");
    ADD(GEQUAL, "gequal");
    ADD(ALWAYS, "always");

    domain = DOMAIN_COMPARE_MODE;
    ADD(NONE, "none");
    ADD(COMPARE_REF_TO_TEXTURE, "compare ref to texture");

    domain = DOMAIN_RGBA;
    ADD(RED, "red");
    ADD(GREEN, "green");
    ADD(BLUE, "blue");
    ADD(ALPHA, "alpha");
    ADD(ZERO, "zero");
    ADD(ONE, "one");

    domain = DOMAIN_WRAP;
    ADD(CLAMP_TO_EDGE, "clamp to edge");
    ADD(REPEAT, "repeat");
    ADD(CLAMP_TO_BORDER, "clamp to border");
    ADD(MIRRORED_REPEAT, "mirrored repeat");
    ADD(MIRROR_CLAMP_TO_EDGE, "mirror clamp to edge");

    domain = DOMAIN_MAG_FILTER;
    ADD(NEAREST, "nearest");
    ADD(LINEAR, "linear");

    domain = DOMAIN_MIN_FILTER;
    ADD(NEAREST, "nearest");
    ADD(LINEAR, "linear");
    ADD(NEAREST_MIPMAP_NEAREST, "nearest mipmap nearest");
    ADD(NEAREST_MIPMAP_LINEAR, "nearest mipmap linear");
    ADD(LINEAR_MIPMAP_NEAREST, "linear mipmap nearest");
    ADD(LINEAR_MIPMAP_LINEAR, "linear mipmap linear");

    domain = DOMAIN_IMAGE_FORMAT_COMPATIBILITY;
    ADD(IMAGE_FORMAT_COMPATIBILITY_BY_SIZE, "by size");
    ADD(IMAGE_FORMAT_COMPATIBILITY_BY_CLASS, "by class");
    ADD(NONE, "none");

    domain = DOMAIN_LEVEL_PNAME;
    ADD(TEXTURE_WIDTH, "width");
    ADD(TEXTURE_HEIGHT, "height");
    ADD(TEXTURE_DEPTH, "depth");
    ADD(TEXTURE_FIXED_SAMPLE_LOCATIONS, "fixed sample locations");
    ADD(TEXTURE_INTERNAL_FORMAT, "internal format");
    ADD(TEXTURE_SHARED_SIZE, "shared size");
    ADD(TEXTURE_COMPRESSED, "compressed");
    ADD(TEXTURE_COMPRESSED_IMAGE_SIZE, "compressed image size");
    ADD(TEXTURE_SAMPLES, "samples");
    ADD(TEXTURE_BUFFER_OFFSET, "buffer offset");
    ADD(TEXTURE_BUFFER_SIZE, "buffer size");
    ADD(TEXTURE_RED_SIZE, "red size");
    ADD(TEXTURE_GREEN_SIZE, "green size");
    ADD(TEXTURE_BLUE_SIZE, "blue size");
    ADD(TEXTURE_ALPHA_SIZE, "alpha size");
    ADD(TEXTURE_DEPTH_SIZE, "depth size");
    ADD(TEXTURE_RED_TYPE, "red type");
    ADD(TEXTURE_GREEN_TYPE, "green type");
    ADD(TEXTURE_BLUE_TYPE, "blue type");
    ADD(TEXTURE_ALPHA_TYPE, "alpha type");
    ADD(TEXTURE_DEPTH_TYPE, "depth type");

    domain = DOMAIN_STENCIL_FUNC;
    ADD(NEVER, "never");
    ADD(ALWAYS, "always");
    ADD(LESS, "less");
    ADD(GREATER, "greater");
    ADD(EQUAL, "equal");
    ADD(LEQUAL, "lequal");
    ADD(GEQUAL, "gequal");
    ADD(NOTEQUAL, "notequal");

    domain = DOMAIN_FACE;
    ADD(FRONT, "front");
    ADD(BACK, "back");
    ADD(FRONT_AND_BACK, "front and back");

    domain = DOMAIN_STENCIL_OP;
    ADD(KEEP, "keep");
    ADD(ZERO,  "zero");
    ADD(REPLACE,  "replace");
    ADD(INCR,  "incr");
    ADD(INCR_WRAP,  "incr wrap");
    ADD(DECR,  "decr");
    ADD(DECR_WRAP, "decr wrap");
    ADD(INVERT, "invert");

    domain = DOMAIN_BLEND_MODE;
    ADD(MIN,  "min");
    ADD(MAX, "max");
    ADD(FUNC_ADD,  "add");
    ADD(FUNC_SUBTRACT,  "subtract");
    ADD(FUNC_REVERSE_SUBTRACT, "reverse subtract");

    domain = DOMAIN_BLEND_FACTOR;
    ADD(ZERO, "zero");
    ADD(ONE, "one");
    ADD(SRC_COLOR, "src color");
    ADD(ONE_MINUS_SRC_COLOR, "one minus src color");
    ADD(DST_COLOR, "dst color");
    ADD(ONE_MINUS_DST_COLOR, "one minus dst color");
    ADD(SRC_ALPHA, "src alpha");
    ADD(ONE_MINUS_SRC_ALPHA, "one minus src alpha");
    ADD(DST_ALPHA, "dst alpha");
    ADD(ONE_MINUS_DST_ALPHA, "one minus dst alpha");
    ADD(CONSTANT_COLOR, "constant color");
    ADD(ONE_MINUS_CONSTANT_COLOR, "one minus constant color");
    ADD(CONSTANT_ALPHA, "constant alpha");
    ADD(ONE_MINUS_CONSTANT_ALPHA, "one minus constant alpha");
    ADD(SRC_ALPHA_SATURATE, "src alpha saturate");
    ADD(SRC1_COLOR, "src1 color");
    ADD(ONE_MINUS_SRC1_COLOR, "one minus src1 color");
    ADD(SRC1_ALPHA,  "src1 alpha");
    ADD(ONE_MINUS_SRC1_ALPHA, "one minus src1 alpha");

    domain = DOMAIN_LOGIC_OP;
    ADD(CLEAR,  "clear");
    ADD(AND,  "and");
    ADD(AND_REVERSE,  "and reverse");
    ADD(COPY, "copy");
    ADD(AND_INVERTED,  "and inverted");
    ADD(NOOP,  "noop");
    ADD(XOR,  "xor");
    ADD(OR,  "or");
    ADD(NOR,  "nor");
    ADD(EQUIV,  "equiv");
    ADD(OR_REVERSE,  "or reverse");
    ADD(INVERT,  "invert");
    ADD(COPY_INVERTED,  "copy inverted");
    ADD(OR_INVERTED, "or inverted");
    ADD(NAND,  "nand");
    ADD(SET,  "set");

    domain = DOMAIN_FRAMEBUFFER_TARGET;
    ADD(DRAW_FRAMEBUFFER, "draw");
    ADD(READ_FRAMEBUFFER, "read");
    ADD(FRAMEBUFFER, "draw read");

    domain = DOMAIN_FRAMEBUFFER_PNAME;
    ADD(FRAMEBUFFER_DEFAULT_WIDTH, "default width");
    ADD(FRAMEBUFFER_DEFAULT_HEIGHT, "default height");
    ADD(FRAMEBUFFER_DEFAULT_LAYERS, "default layers");
    ADD(FRAMEBUFFER_DEFAULT_SAMPLES, "default samples");
    ADD(FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, "default fixed sample locations");

    domain = DOMAIN_FRAMEBUFFER_STATUS;
    ADD(FRAMEBUFFER_COMPLETE, "complete");
    ADD(FRAMEBUFFER_UNDEFINED, "undefined");
    ADD(FRAMEBUFFER_INCOMPLETE_ATTACHMENT, "incomplete attachment");
    ADD(FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, "incomplete missing attachment");
    ADD(FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, "incomplete draw buffer");
    ADD(FRAMEBUFFER_INCOMPLETE_READ_BUFFER, "incomplete read buffer");
    ADD(FRAMEBUFFER_UNSUPPORTED, "unsupported");
    ADD(FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, "incomplete multisample");
    ADD(FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS, "incomplete layer targets");

    domain = DOMAIN_FRAMEBUFFER_GPNAME;
    ADD(FRAMEBUFFER_DEFAULT_WIDTH, "default width");
    ADD(FRAMEBUFFER_DEFAULT_HEIGHT, "default height");
    ADD(FRAMEBUFFER_DEFAULT_LAYERS, "default layers");
    ADD(FRAMEBUFFER_DEFAULT_SAMPLES, "default samples");
    ADD(FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, "default fixed sample locations");
    ADD(DOUBLEBUFFER, "doublebuffer");
    ADD(IMPLEMENTATION_COLOR_READ_FORMAT, "implementation color read format");
    ADD(IMPLEMENTATION_COLOR_READ_TYPE, "implementation color read type");
    ADD(SAMPLES, "samples");
    ADD(SAMPLE_BUFFERS, "sample buffers");
    ADD(STEREO, "stereo");

    domain = DOMAIN_FRAMEBUFFER_APNAME;
    ADD(FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, "object type");
    ADD(FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, "object name");
    ADD(FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, "component type");
    ADD(FRAMEBUFFER_ATTACHMENT_RED_SIZE, "red size");
    ADD(FRAMEBUFFER_ATTACHMENT_GREEN_SIZE, "green size");
    ADD(FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,  "blue size");
    ADD(FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,  "alpha size");
    ADD(FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, "depth size");
    ADD(FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,  "stencil size");
    ADD(FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, "color encoding");
    ADD(FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER, "texture layer");
    ADD(FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, "texture level");
    ADD(FRAMEBUFFER_ATTACHMENT_LAYERED,  "layered");
    ADD(FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE, "texture cube map face");

    domain = DOMAIN_OBJECT_TYPE;
    ADD(NONE, "none");
    ADD(FRAMEBUFFER_DEFAULT, "framebuffer default");
    ADD(TEXTURE, "texture");
    ADD(RENDERBUFFER, "framebuffer");

    domain = DOMAIN_COMPONENT_TYPE;
    ADD(NONE, "none");
    ADD(SIGNED_NORMALIZED, "snorm");
    ADD(UNSIGNED_NORMALIZED, "unorm");
    ADD(FLOAT, "float");
    ADD(INT, "int");
    ADD(UNSIGNED_INT, "uint");

    domain = DOMAIN_COLOR_ENCODING;
    ADD(LINEAR, "linear");
    ADD(SRGB, "srgb");

    domain = DOMAIN_DRAW_MODE;
    ADD(POINTS,  "points");
    ADD(PATCHES, "patches");
    ADD(LINE_STRIP,  "line strip");
    ADD(LINE_LOOP,  "line loop");
    ADD(TRIANGLE_STRIP,  "triangle strip");
    ADD(TRIANGLE_FAN,  "triangle fan");
    ADD(LINES,  "lines");
    ADD(LINES_ADJACENCY,  "lines adjacency");
    ADD(TRIANGLES,  "triangles");
    ADD(TRIANGLES_ADJACENCY, "triangles adjacency");
    ADD(LINE_STRIP_ADJACENCY,  "line strip adjacency");
    ADD(TRIANGLE_STRIP_ADJACENCY, "triangle strip adjacency");

    domain = DOMAIN_DRAW_TYPE;
    ADD(UNSIGNED_BYTE, "ubyte");
    ADD(UNSIGNED_SHORT, "ushort");
    ADD(UNSIGNED_INT, "uint");

    domain = DOMAIN_CONDITIONAL_RENDER_MODE;
    ADD(QUERY_WAIT, "query wait");
    ADD(QUERY_NO_WAIT, "query no wait");
    ADD(QUERY_BY_REGION_WAIT, "query by region wait");
    ADD(QUERY_BY_REGION_NO_WAIT, "query by region no wait");
    ADD(QUERY_WAIT_INVERTED, "query wait inverted");
    ADD(QUERY_NO_WAIT_INVERTED, "query no wait inverted");
    ADD(QUERY_BY_REGION_WAIT_INVERTED, "query by region wait inverted");
    ADD(QUERY_BY_REGION_NO_WAIT_INVERTED, "query by region no wait inverted");

    domain = DOMAIN_FILTER;
    ADD(NEAREST, "nearest");
    ADD(LINEAR, "linear");

    domain = DOMAIN_CLAMP_TARGET;
    ADD(CLAMP_READ_COLOR, "clamp read color");

    domain = DOMAIN_CLAMP;
    ADD(TRUE, "true");
    ADD(FALSE, "false");
    ADD(FIXED_ONLY, "fixed only");

    domain = DOMAIN_IMAGE_TARGET;
    ADD(RENDERBUFFER, "renderbuffer");
    ADD(TEXTURE_1D, "1d");
    ADD(TEXTURE_2D, "2d");
    ADD(TEXTURE_3D, "3d");
    ADD(TEXTURE_1D_ARRAY, "1d array");
    ADD(TEXTURE_2D_ARRAY, "2d array");
    ADD(TEXTURE_RECTANGLE, "rectangle");
    ADD(TEXTURE_CUBE_MAP, "cube map");
    ADD(TEXTURE_CUBE_MAP_ARRAY, "cube map array");
    ADD(TEXTURE_BUFFER,  "buffer");
    ADD(TEXTURE_2D_MULTISAMPLE, "2d multisample");
    ADD(TEXTURE_2D_MULTISAMPLE_ARRAY, "2d multisample array");

    domain = DOMAIN_PIXEL_STORE_PNAME;
    ADD(UNPACK_SWAP_BYTES,  "unpack swap bytes");
    ADD(UNPACK_LSB_FIRST,  "unpack lsb first");
    ADD(UNPACK_ROW_LENGTH,  "unpack row length");
    ADD(UNPACK_SKIP_ROWS,  "unpack skip rows");
    ADD(UNPACK_SKIP_PIXELS,  "unpack skip pixels");
    ADD(UNPACK_ALIGNMENT, "unpack alignment");
    ADD(UNPACK_IMAGE_HEIGHT, "unpack image height");
    ADD(UNPACK_SKIP_IMAGES,  "unpack skip images");
    ADD(UNPACK_COMPRESSED_BLOCK_WIDTH,  "unpack compressed block width");
    ADD(UNPACK_COMPRESSED_BLOCK_HEIGHT,  "unpack compressed block height");
    ADD(UNPACK_COMPRESSED_BLOCK_DEPTH,  "unpack compressed block depth");
    ADD(UNPACK_COMPRESSED_BLOCK_SIZE,  "unpack compressed block size");
    ADD(PACK_SWAP_BYTES,  "pack swap bytes");
    ADD(PACK_LSB_FIRST,  "pack lsb first");
    ADD(PACK_ROW_LENGTH,  "pack row length");
    ADD(PACK_SKIP_ROWS,  "pack skip rows");
    ADD(PACK_SKIP_PIXELS,  "pack skip pixels");
    ADD(PACK_ALIGNMENT, "pack alignment");
    ADD(PACK_IMAGE_HEIGHT,  "pack image height");
    ADD(PACK_SKIP_IMAGES,  "pack skip images");
    ADD(PACK_COMPRESSED_BLOCK_WIDTH,  "pack compressed block width");
    ADD(PACK_COMPRESSED_BLOCK_HEIGHT,  "pack compressed block height");
    ADD(PACK_COMPRESSED_BLOCK_DEPTH,  "pack compressed block depth");
    ADD(PACK_COMPRESSED_BLOCK_SIZE,  "pack compressed block size");

    domain = DOMAIN_BUFFER_TARGET;
    ADD(ARRAY_BUFFER, "array");
    ADD(UNIFORM_BUFFER, "uniform");
    ADD(ATOMIC_COUNTER_BUFFER, "atomic counter");
    ADD(QUERY_BUFFER, "query");
    ADD(COPY_READ_BUFFER, "copy read");
    ADD(COPY_WRITE_BUFFER, "copy write");
    ADD(DISPATCH_INDIRECT_BUFFER, "dispatch indirect");
    ADD(DRAW_INDIRECT_BUFFER, "draw indirect");
    ADD(ELEMENT_ARRAY_BUFFER, "element array");
    ADD(TEXTURE_BUFFER, "texture");
    ADD(PIXEL_PACK_BUFFER, "pixel pack");
    ADD(PIXEL_UNPACK_BUFFER, "pixel unpack");
    ADD(SHADER_STORAGE_BUFFER, "shader storage");
    ADD(TRANSFORM_FEEDBACK_BUFFER, "transform feedback");
    ADD(PARAMETER_BUFFER, "parameter"); //GL_VERSION_4_6

    domain = DOMAIN_BUFFER_RANGE_TARGET;
    ADD(ATOMIC_COUNTER_BUFFER, "atomic counter");
    ADD(SHADER_STORAGE_BUFFER, "shader storage");
    ADD(UNIFORM_BUFFER, "uniform");
    ADD(TRANSFORM_FEEDBACK_BUFFER, "transform feedback");

    domain = DOMAIN_BUFFER_USAGE;
    ADD(STREAM_DRAW,  "stream draw");
    ADD(STREAM_READ,  "stream read");
    ADD(STREAM_COPY,  "stream copy");
    ADD(STATIC_DRAW,  "static draw");
    ADD(STATIC_READ,  "static read");
    ADD(STATIC_COPY,  "static copy");
    ADD(DYNAMIC_DRAW,  "dynamic draw");
    ADD(DYNAMIC_READ,  "dynamic read");
    ADD(DYNAMIC_COPY, "dynamic copy");

    domain = DOMAIN_BUFFER_PNAME;
    ADD(BUFFER_SIZE, "size");
    ADD(BUFFER_USAGE, "usage");
    ADD(BUFFER_ACCESS, "access");
    ADD(BUFFER_ACCESS_FLAGS, "access flags");
    ADD(BUFFER_IMMUTABLE_STORAGE, "immutable storage");
    ADD(BUFFER_MAPPED, "mapped");
    ADD(BUFFER_MAP_POINTER, "map pointer");
    ADD(BUFFER_MAP_OFFSET, "map offset");
    ADD(BUFFER_MAP_LENGTH, "map length");
    ADD(BUFFER_STORAGE_FLAGS, "storage flags");

    domain = DOMAIN_BUFFER_ACCESS;
    ADD(READ_ONLY, "read only");
    ADD(WRITE_ONLY, "write only");
    ADD(READ_WRITE, "read write");

    domain = DOMAIN_SHADER_PNAME;
    ADD(SHADER_TYPE, "type");
    ADD(DELETE_STATUS, "delete status");
    ADD(COMPILE_STATUS, "compile status");
    ADD(INFO_LOG_LENGTH, "info log length");
    ADD(SHADER_SOURCE_LENGTH, "source length");
    ADD(SPIR_V_BINARY, "spir v binary"); //GL_VERSION_4_6

    domain = DOMAIN_PRECISION_FORMAT;
    ADD(LOW_FLOAT, "low float");
    ADD(MEDIUM_FLOAT, "medium float");
    ADD(HIGH_FLOAT,  "high float");
    ADD(LOW_INT,  "low int");
    ADD(MEDIUM_INT,  "medium int");
    ADD(HIGH_INT, "high int");

    domain = DOMAIN_CONDITION;
    ADD(SYNC_GPU_COMMANDS_COMPLETE, "gpu commands complete");

    domain = DOMAIN_SYNC_STATUS;
    ADD(ALREADY_SIGNALED, "already signaled");
    ADD(TIMEOUT_EXPIRED, "timeout expired");
    ADD(CONDITION_SATISFIED, "condition satisfied");
    ADD(WAIT_FAILED, "wait failed");

    domain = DOMAIN_SYNC_PNAME;
    ADD(OBJECT_TYPE, "type");
    ADD(SYNC_STATUS, "status");
    ADD(SYNC_CONDITION, "condition");
    ADD(SYNC_FLAGS, "flags");

    domain = DOMAIN_INTERFACE;
    ADD(UNIFORM, "uniform");
    ADD(UNIFORM_BLOCK, "uniform block");
    ADD(ATOMIC_COUNTER_BUFFER, "atomic counter buffer");
    ADD(PROGRAM_INPUT, "program input");
    ADD(PROGRAM_OUTPUT, "program output");
    ADD(VERTEX_SUBROUTINE, "vertex subroutine");
    ADD(TESS_CONTROL_SUBROUTINE, "tess control subroutine");
    ADD(TESS_EVALUATION_SUBROUTINE, "tess evaluation subroutine");
    ADD(GEOMETRY_SUBROUTINE, "geometry subroutine");
    ADD(FRAGMENT_SUBROUTINE, "fragment subroutine");
    ADD(COMPUTE_SUBROUTINE, "compute subroutine");
    ADD(VERTEX_SUBROUTINE_UNIFORM, "vertex subroutine uniform");
    ADD(TESS_CONTROL_SUBROUTINE_UNIFORM, "tess control subroutine uniform");
    ADD(TESS_EVALUATION_SUBROUTINE_UNIFORM, "tess evaluation subroutine uniform");
    ADD(GEOMETRY_SUBROUTINE_UNIFORM, "geometry subroutine uniform");
    ADD(FRAGMENT_SUBROUTINE_UNIFORM, "fragment subroutine uniform");
    ADD(COMPUTE_SUBROUTINE_UNIFORM, "compute subroutine uniform");
    ADD(TRANSFORM_FEEDBACK_VARYING, "transform feedback varying");
    ADD(BUFFER_VARIABLE, "buffer variable");
    ADD(SHADER_STORAGE_BLOCK, "shader storage block");
    ADD(TRANSFORM_FEEDBACK_BUFFER, "transform feedback buffer");

    domain = DOMAIN_INTERFACE_PNAME;
    ADD(ACTIVE_RESOURCES, "active resources");
    ADD(MAX_NAME_LENGTH, "max name length");
    ADD(MAX_NUM_ACTIVE_VARIABLES, "max num active variables");
    ADD(MAX_NUM_COMPATIBLE_SUBROUTINES, "max num compatible subroutines");

    domain = DOMAIN_RESOURCE_PROPERTY;
    ADD(ACTIVE_VARIABLES, "active variables");
    ADD(ARRAY_SIZE, "array size");
    ADD(ARRAY_STRIDE, "array stride");
    ADD(ATOMIC_COUNTER_BUFFER_INDEX, "atomic counter buffer index");
    ADD(BLOCK_INDEX, "block index");
    ADD(BUFFER_BINDING,  "buffer binding"); 
    ADD(BUFFER_DATA_SIZE, "buffer data size");
    ADD(IS_PER_PATCH, "is per patch");
    ADD(IS_ROW_MAJOR, "is row major");
    ADD(LOCATION, "location");
    ADD(LOCATION_COMPONENT, "location component");
    ADD(LOCATION_INDEX, "location index");
    ADD(MATRIX_STRIDE, "matrix stride");
    ADD(NAME_LENGTH, "name length");
    ADD(NUM_ACTIVE_VARIABLES, "num active variables");
    ADD(OFFSET, "offset");
    ADD(REFERENCED_BY_VERTEX_SHADER, "referenced by vertex shader");
    ADD(REFERENCED_BY_TESS_CONTROL_SHADER, "referenced by tess control shader");
    ADD(REFERENCED_BY_TESS_EVALUATION_SHADER, "referenced by tess evaluation shader");
    ADD(REFERENCED_BY_GEOMETRY_SHADER, "referenced by geometry shader");
    ADD(REFERENCED_BY_FRAGMENT_SHADER,  "referenced by fragment shader"); 
    ADD(REFERENCED_BY_COMPUTE_SHADER,  "referenced by compute shader"); 
    ADD(TOP_LEVEL_ARRAY_SIZE, "top level array size");
    ADD(TOP_LEVEL_ARRAY_STRIDE, "top level array stride");
    ADD(TRANSFORM_FEEDBACK_BUFFER_INDEX, "transform feedback buffer index");
    ADD(TRANSFORM_FEEDBACK_BUFFER_STRIDE, "transform feedback buffer stride");
    ADD(TYPE, "type");

    domain = DOMAIN_SAMPLER_PNAME;
    ADD(TEXTURE_WRAP_S,  "wrap s"); 
    ADD(TEXTURE_WRAP_T,  "wrap t"); 
    ADD(TEXTURE_WRAP_R,  "wrap r"); 
    ADD(TEXTURE_MAX_ANISOTROPY, "max anisotropy"); //GL_VERSION_4_6
    ADD(TEXTURE_MIN_FILTER,  "min filter"); 
    ADD(TEXTURE_MAG_FILTER,  "mag filter"); 
    ADD(TEXTURE_BORDER_COLOR,  "border color"); 
    ADD(TEXTURE_MIN_LOD,  "min lod"); 
    ADD(TEXTURE_MAX_LOD,  "max lod"); 
    ADD(TEXTURE_LOD_BIAS, "lod bias");
    ADD(TEXTURE_COMPARE_MODE,  "compare mode"); 
    ADD(TEXTURE_COMPARE_FUNC, "compare func");

    domain = DOMAIN_CAPABILITY;
    ADD(BLEND, "blend");
    ADD(CLIP_DISTANCE0, "clip distance 0");
    ADD(CLIP_DISTANCE1, "clip distance 1");
    ADD(CLIP_DISTANCE2, "clip distance 2");
    ADD(CLIP_DISTANCE3, "clip distance 3");
    ADD(CLIP_DISTANCE4, "clip distance 4");
    ADD(CLIP_DISTANCE5, "clip distance 5");
    ADD(CLIP_DISTANCE6, "clip distance 6");
    ADD(CLIP_DISTANCE7, "clip distance 7");
    ADD(COLOR_LOGIC_OP, "color logic op");
    ADD(CULL_FACE, "cull face");
    ADD(DEBUG_OUTPUT, "debug output");
    ADD(DEBUG_OUTPUT_SYNCHRONOUS, "debug output synchronous");
    ADD(DEPTH_CLAMP, "depth clamp");
    ADD(DEPTH_TEST, "depth test");
    ADD(DITHER, "dither");
    ADD(FRAMEBUFFER_SRGB, "framebuffer srgb");
    ADD(LINE_SMOOTH, "line smooth");
    ADD(MULTISAMPLE, "multisample");
    ADD(POLYGON_OFFSET_FILL, "polygon offset fill");
    ADD(POLYGON_OFFSET_LINE, "polygon offset line");
    ADD(POLYGON_OFFSET_POINT, "polygon offset point");
    ADD(POLYGON_OFFSET_CLAMP, "polygon offset clamp"); //GL_VERSION_4_6
    ADD(POLYGON_SMOOTH, "polygon smooth");
    ADD(PRIMITIVE_RESTART, "primitive restart");
    ADD(PRIMITIVE_RESTART_FIXED_INDEX, "primitive restart fixed index");
    ADD(RASTERIZER_DISCARD, "rasterizer discard");
    ADD(SAMPLE_ALPHA_TO_COVERAGE, "sample alpha to coverage");
    ADD(SAMPLE_ALPHA_TO_ONE, "sample alpha to one");
    ADD(SAMPLE_COVERAGE, "sample coverage");
    ADD(SAMPLE_SHADING, "sample shading");
    ADD(SAMPLE_MASK, "sample mask");
    ADD(SCISSOR_TEST, "scissor test");
    ADD(STENCIL_TEST, "stencil test");
    ADD(TEXTURE_CUBE_MAP_SEAMLESS, "texture cube map seamless");
    ADD(PROGRAM_POINT_SIZE, "program point size");

    domain = DOMAIN_PATCH_PNAME;
    ADD(PATCH_VERTICES,  "vertices");
    ADD(PATCH_DEFAULT_OUTER_LEVEL,  "default outer level"); 
    ADD(PATCH_DEFAULT_INNER_LEVEL, "default inner level");

    domain = DOMAIN_HINT_TARGET;
    ADD(LINE_SMOOTH_HINT, "line smooth");
    ADD(POLYGON_SMOOTH_HINT, "polygon smooth");
    ADD(TEXTURE_COMPRESSION_HINT, "texture compression");
    ADD(FRAGMENT_SHADER_DERIVATIVE_HINT, "fragment shader derivative");

    domain = DOMAIN_HINT_MODE;
    ADD(FASTEST, "fastest");
    ADD(NICEST, "nicest");
    ADD(DONT_CARE, "don't care");

    domain = DOMAIN_QUERY_TARGET;
    ADD(TIMESTAMP, "timestamp");
    ADD(ANY_SAMPLES_PASSED,  "any samples passed"); 
    ADD(ANY_SAMPLES_PASSED_CONSERVATIVE,  "any samples passed conservative"); 
    ADD(PRIMITIVES_GENERATED,  "primitives generated"); 
    ADD(SAMPLES_PASSED,  "samples passed"); 
    ADD(TIME_ELAPSED, "time elapsed");
    ADD(TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, "transform feedback primitives written");
    //GL_VERSION_4_6:
    ADD(VERTICES_SUBMITTED, "vertices submitted");
    ADD(PRIMITIVES_SUBMITTED, "primitives submitted");
    ADD(VERTEX_SHADER_INVOCATIONS, "vertex shader invocations");
    ADD(TESS_CONTROL_SHADER_PATCHES, "tess control shader patches");
    ADD(TESS_EVALUATION_SHADER_INVOCATIONS, "tess evaluation shader invocations");
    ADD(GEOMETRY_SHADER_PRIMITIVES_EMITTED, "geometry shader primitives emitted");
    ADD(FRAGMENT_SHADER_INVOCATIONS, "fragment shader invocations");
    ADD(COMPUTE_SHADER_INVOCATIONS, "compute shader invocations");
    ADD(GEOMETRY_SHADER_INVOCATIONS, "geometry shader invocations");
    ADD(CLIPPING_INPUT_PRIMITIVES, "clipping input primitives");
    ADD(CLIPPING_OUTPUT_PRIMITIVES, "clipping output primitives");
    ADD(TRANSFORM_FEEDBACK_OVERFLOW, "transform feedback overflow");
    ADD(TRANSFORM_FEEDBACK_STREAM_OVERFLOW, "transform feedback stream overflow");

    domain = DOMAIN_QUERY_PNAME;
    ADD(CURRENT_QUERY, "current query");
    ADD(QUERY_COUNTER_BITS, "query counter bits");

#if 0
    domain = DOMAIN_QUERY_OBJECT_PNAME;
    ADD(TARGET, "target");
    ADD(RESULT_NO_WAIT, "result no wait");
    ADD(RESULT_AVAILABLE, "result available");
#endif

    domain = DOMAIN_STRING_NAME;
    ADD(RENDERER, "renderer");
    ADD(VENDOR, "vendor");
    ADD(VERSION, "version");
    ADD(EXTENSIONS, "extensions");
    ADD(SHADING_LANGUAGE_VERSION, "shading language version");

    domain = DOMAIN_UNIFORM_PNAME;
    ADD(UNIFORM_TYPE, "type");
    ADD(UNIFORM_SIZE, "size");
    ADD(UNIFORM_NAME_LENGTH, "name length");
    ADD(UNIFORM_BLOCK_INDEX, "block index");
    ADD(UNIFORM_OFFSET, "offset");
    ADD(UNIFORM_ARRAY_STRIDE, "array stride");
    ADD(UNIFORM_MATRIX_STRIDE, "matrix stride");
    ADD(UNIFORM_IS_ROW_MAJOR, "is row major");
    ADD(UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX, "atomic counter buffer index");

    domain = DOMAIN_BLOCK_PNAME;
    ADD(UNIFORM_BLOCK_BINDING, "binding");
    ADD(UNIFORM_BLOCK_DATA_SIZE,  "data size"); 
    ADD(UNIFORM_BLOCK_NAME_LENGTH, "name length");
    ADD(UNIFORM_BLOCK_ACTIVE_UNIFORMS, "active uniforms");
    ADD(UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, "active uniform indices");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, "referenced by vertex shader");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER, "referenced by tess control shader");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER, "referenced by tess evaluation shader");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER, "referenced by geometry shader");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, "referenced by fragment shader");
    ADD(UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER, "referenced by compute shader");

    domain = DOMAIN_ACB_PNAME;
    ADD(ATOMIC_COUNTER_BUFFER_BINDING, "binding");
    ADD(ATOMIC_COUNTER_BUFFER_DATA_SIZE, "data size");
    ADD(ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS, "active atomic counters");
    ADD(ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES, "active atomic counter indices");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER, "referenced by vertex shader");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER, "referenced by tess control shader");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER, "referenced by tess evaluation shader");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER, "referenced by geometry shader");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER, "referenced by fragment shader");
    ADD(ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER, "referenced by compute shader");

    domain = DOMAIN_BUFFER;
    ADD(NONE, "none");
    ADD(FRONT_LEFT, "front left");
    ADD(FRONT_RIGHT, "front right");
    ADD(BACK_LEFT, "back left");
    ADD(BACK_RIGHT, "back right");
    ADD(FRONT, "front");
    ADD(BACK, "back");
    ADD(LEFT, "left");
    ADD(RIGHT, "right");
    ADD(FRONT_AND_BACK, "front and back");
    ADD(COLOR_ATTACHMENT0, "color attachment 0");
    ADD(COLOR_ATTACHMENT1, "color attachment 1");
    ADD(COLOR_ATTACHMENT2, "color attachment 2");
    ADD(COLOR_ATTACHMENT3, "color attachment 3");
    ADD(COLOR_ATTACHMENT4, "color attachment 4");
    ADD(COLOR_ATTACHMENT5, "color attachment 5");
    ADD(COLOR_ATTACHMENT6, "color attachment 6");
    ADD(COLOR_ATTACHMENT7, "color attachment 7");
    ADD(COLOR_ATTACHMENT8, "color attachment 8");
    ADD(COLOR_ATTACHMENT9, "color attachment 9");
    ADD(COLOR_ATTACHMENT10, "color attachment 10");
    ADD(COLOR_ATTACHMENT11, "color attachment 11");
    ADD(COLOR_ATTACHMENT12, "color attachment 12");
    ADD(COLOR_ATTACHMENT13, "color attachment 13");
    ADD(COLOR_ATTACHMENT14, "color attachment 14");
    ADD(COLOR_ATTACHMENT15, "color attachment 15");
    /* for attachments (see framebuffer.c): */
    ADD(COLOR, "color");
    ADD(DEPTH, "depth");
    ADD(STENCIL, "stencil");
    ADD(DEPTH_ATTACHMENT, "depth attachment");
    ADD(STENCIL_ATTACHMENT, "stencil attachment");
    ADD(DEPTH_STENCIL_ATTACHMENT, "depth stencil attachment");

    domain = DOMAIN_CLEAR_BUFFER;
    ADD(COLOR, "color");
    ADD(DEPTH, "depth");
    ADD(STENCIL, "stencil");

    domain = DOMAIN_PROGRAM_PARAMETER_PNAME;
    ADD(PROGRAM_SEPARABLE, "separable");
    ADD(PROGRAM_BINARY_RETRIEVABLE_HINT, "binary retrievable hint");

    domain = DOMAIN_GEOMETRY_INPUT_TYPE;
    ADD(POINTS, "points");
    ADD(LINES, "lines");
    ADD(LINES_ADJACENCY, "lines adjacency");
    ADD(TRIANGLES, "triangles");
    ADD(TRIANGLES_ADJACENCY, "triangles adjacency");

    domain = DOMAIN_GEOMETRY_OUTPUT_TYPE;
    ADD(POINTS, "points");
    ADD(LINE_STRIP, "line strip");
    ADD(TRIANGLE_STRIP, "triangle");

    domain = DOMAIN_TESS_GEN_MODE;
    ADD(QUADS, "quads");
    ADD(TRIANGLES, "triangles");
    ADD(ISOLINES, "isolines");

    domain = DOMAIN_TESS_GEN_SPACING;
    ADD(EQUAL, "equal");
    ADD(FRACTIONAL_EVEN, "fractional even");
    ADD(FRACTIONAL_ODD, "fractional odd");

    domain = DOMAIN_TESS_GEN_VERTEX_ORDER;
    ADD(CCW, "ccw");
    ADD(CW, "cw");

    domain = DOMAIN_BUFFER_MODE;
    ADD(SEPARATE_ATTRIBS, "separate attribs");
    ADD(INTERLEAVED_ATTRIBS, "interleaved attribs");

    domain = DOMAIN_DEBUG_SOURCE;
    ADD(DONT_CARE, "don't care");
    ADD(DEBUG_SOURCE_API, "api");
    ADD(DEBUG_SOURCE_SHADER_COMPILER, "shader compiler");
    ADD(DEBUG_SOURCE_WINDOW_SYSTEM, "window system");
    ADD(DEBUG_SOURCE_THIRD_PARTY, "third party");
    ADD(DEBUG_SOURCE_APPLICATION, "application");
    ADD(DEBUG_SOURCE_OTHER, "other");

    domain = DOMAIN_DEBUG_TYPE;
    ADD(DONT_CARE, "don't care");
    ADD(DEBUG_TYPE_ERROR, "error");
    ADD(DEBUG_TYPE_DEPRECATED_BEHAVIOR, "deprecated behavior");
    ADD(DEBUG_TYPE_UNDEFINED_BEHAVIOR, "undefined behavior");
    ADD(DEBUG_TYPE_PERFORMANCE, "performance");
    ADD(DEBUG_TYPE_PORTABILITY, "portability");
    ADD(DEBUG_TYPE_MARKER, "marker");
    ADD(DEBUG_TYPE_PUSH_GROUP, "push group");
    ADD(DEBUG_TYPE_POP_GROUP, "pop group");
    ADD(DEBUG_TYPE_OTHER, "other");

    domain = DOMAIN_DEBUG_SEVERITY;
    ADD(DONT_CARE, "don't care");
    ADD(DEBUG_SEVERITY_HIGH, "high");
    ADD(DEBUG_SEVERITY_MEDIUM, "medium");
    ADD(DEBUG_SEVERITY_LOW, "low");
    ADD(DEBUG_SEVERITY_NOTIFICATION, "notification");

    domain = DOMAIN_DEBUG_IDENTIFIER;
    ADD(BUFFER,  "buffer"); 
    ADD(FRAMEBUFFER, "framebuffer");
    ADD(RENDERBUFFER,  "renderbuffer"); 
    ADD(PROGRAM_PIPELINE,  "program pipeline"); 
    ADD(PROGRAM,  "program"); 
    ADD(QUERY,  "query"); 
    ADD(SAMPLER,  "sampler"); 
    ADD(SHADER,  "shader"); 
    ADD(TEXTURE,  "texture"); 
    ADD(TRANSFORM_FEEDBACK,  "transform feedback"); 
    ADD(VERTEX_ARRAY,  "vertex array"); 

    domain = DOMAIN_TEXTURE_ACCESS;
    ADD(READ_ONLY, "read only");
    ADD(WRITE_ONLY, "write only");
    ADD(READ_WRITE, "read write");

    domain = DOMAIN_PNAME;
    ADD(CONTEXT_FLAGS, "context flags");
    ADD(MAJOR_VERSION, "major version");
    ADD(MAX_SERVER_WAIT_TIMEOUT, "max server wait timeout");
    ADD(MIN_MAP_BUFFER_ALIGNMENT, "min map buffer alignment");
    ADD(MINOR_VERSION, "minor version");
    ADD(NUM_EXTENSIONS, "num extensions");
    ADD(NUM_SPIR_V_EXTENSIONS, "num spir v extensions"); //GL_VERSION_4_6
    ADD(NUM_SHADING_LANGUAGE_VERSIONS, "num shading language versions");
    ADD(TIMESTAMP, "timestamp");
    /* Buffer Binding State */
    ADD(ARRAY_BUFFER_BINDING, "array buffer binding");
    ADD(COPY_READ_BUFFER_BINDING, "copy read buffer binding");
    ADD(COPY_WRITE_BUFFER_BINDING, "copy write buffer binding");
    ADD(DRAW_INDIRECT_BUFFER_BINDING, "draw indirect buffer binding");
    ADD(ELEMENT_ARRAY_BUFFER_BINDING, "element array buffer binding");
    ADD(PARAMETER_BUFFER_BINDING, "parameter buffer binding"); //GL_VERSION_4_6
    ADD(QUERY_BUFFER_BINDING, "query buffer binding");
    ADD(TEXTURE_BUFFER_BINDING, "texture buffer binding");
    ADD(VERTEX_ARRAY_BINDING, "vertex array binding");
    ADD(MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, "max atomic counter buffer bindings");
    ADD(ATOMIC_COUNTER_BUFFER_BINDING, "atomic counter buffer binding");
    ADD(ATOMIC_COUNTER_BUFFER_START, "atomic counter buffer start");
    ADD(ATOMIC_COUNTER_BUFFER_SIZE, "atomic counter buffer size");
    ADD(MAX_SHADER_STORAGE_BUFFER_BINDINGS, "max shader storage buffer bindings");
    ADD(SHADER_STORAGE_BUFFER_BINDING, "shader storage buffer binding");
    ADD(SHADER_STORAGE_BUFFER_START, "shader storage buffer start");
    ADD(SHADER_STORAGE_BUFFER_SIZE, "shader storage buffer size");
    ADD(MAX_TRANSFORM_FEEDBACK_BUFFERS, "max transform feedback buffers"); /* for GL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS */
//  ADD(MAX_TRANSFORM_FEEDBACK_BUFFERS, "max transform feedback buffer bindings"); /* alias */
    ADD(TRANSFORM_FEEDBACK_BUFFER_BINDING, "transform feedback buffer binding");
    ADD(TRANSFORM_FEEDBACK_BUFFER_START, "transform feedback buffer start");
    ADD(TRANSFORM_FEEDBACK_BUFFER_SIZE, "transform feedback buffer size");
    ADD(MAX_UNIFORM_BUFFER_BINDINGS, "max uniform buffer bindings");
    ADD(UNIFORM_BUFFER_BINDING, "uniform buffer binding");
    ADD(UNIFORM_BUFFER_START, "uniform buffer start");
    ADD(UNIFORM_BUFFER_SIZE, "uniform buffer size");
    ADD(MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, "max transform feedback interleaved components");
    ADD(MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, "max transform feedback separate attribs");
    ADD(MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, "max transform feedback separate components");
    ADD(TRANSFORM_FEEDBACK_PAUSED, "transform feedback paused");
    ADD(TRANSFORM_FEEDBACK_ACTIVE, "transform feedback active");
    /* Debug Output State */
    ADD(DEBUG_GROUP_STACK_DEPTH, "debug group stack depth");
    ADD(DEBUG_LOGGED_MESSAGES, "debug logged messages");
    ADD(DEBUG_NEXT_LOGGED_MESSAGE_LENGTH, "debug next logged message length");
    ADD(DEBUG_OUTPUT, "debug output");
    ADD(DEBUG_OUTPUT_SYNCHRONOUS, "debug output synchronous");
    ADD(MAX_DEBUG_LOGGED_MESSAGES, "max debug logged messages");
    ADD(MAX_DEBUG_MESSAGE_LENGTH, "max debug message length");
    ADD(MAX_DEBUG_GROUP_STACK_DEPTH, "max debug group stack depth");
    ADD(MAX_LABEL_LENGTH, "max label length");
    /* Framebuffers */
    ADD(COLOR_CLEAR_VALUE, "color clear value");
    ADD(COLOR_WRITEMASK, "color writemask");
    ADD(DEPTH_CLEAR_VALUE, "depth clear value");
    ADD(DEPTH_FUNC, "depth func");
    ADD(DEPTH_TEST, "depth test");
    ADD(DEPTH_WRITEMASK, "depth writemask");
    ADD(DOUBLEBUFFER, "doublebuffer");
    ADD(DRAW_BUFFER, "draw buffer");
    ADD(DRAW_BUFFER0, "draw buffer 0");
    ADD(DRAW_BUFFER1, "draw buffer 1");
    ADD(DRAW_BUFFER2, "draw buffer 2");
    ADD(DRAW_BUFFER3, "draw buffer 3");
    ADD(DRAW_BUFFER4, "draw buffer 4");
    ADD(DRAW_BUFFER5, "draw buffer 5");
    ADD(DRAW_BUFFER6, "draw buffer 6");
    ADD(DRAW_BUFFER7, "draw buffer 7");
    ADD(DRAW_BUFFER8, "draw buffer 8");
    ADD(DRAW_BUFFER9, "draw buffer 9");
    ADD(DRAW_BUFFER10, "draw buffer 10");
    ADD(DRAW_BUFFER11, "draw buffer 11");
    ADD(DRAW_BUFFER12, "draw buffer 12");
    ADD(DRAW_BUFFER13, "draw buffer 13");
    ADD(DRAW_BUFFER14, "draw buffer 14");
    ADD(DRAW_BUFFER15, "draw buffer 15");
    ADD(DRAW_FRAMEBUFFER_BINDING, "draw framebuffer binding");
    ADD(MAX_COLOR_ATTACHMENTS, "max color attachments");
    ADD(MAX_COLOR_TEXTURE_SAMPLES, "max color texture samples");
    ADD(MAX_DEPTH_TEXTURE_SAMPLES, "max depth texture samples");
    ADD(MAX_DRAW_BUFFERS, "max draw buffers");
    ADD(MAX_DUAL_SOURCE_DRAW_BUFFERS, "max dual source draw buffers");
    ADD(MAX_FRAMEBUFFER_HEIGHT, "max framebuffer height");
    ADD(MAX_FRAMEBUFFER_LAYERS, "max framebuffer layers");
    ADD(MAX_FRAMEBUFFER_SAMPLES, "max framebuffer samples");
    ADD(MAX_FRAMEBUFFER_WIDTH, "max framebuffer width");
    ADD(MAX_INTEGER_SAMPLES, "max integer samples");
    ADD(MAX_SAMPLES, "max samples");
    ADD(READ_BUFFER, "read buffer");
    ADD(READ_FRAMEBUFFER_BINDING, "read framebuffer binding");
    ADD(RENDERBUFFER_BINDING, "renderbuffer binding");
    ADD(STENCIL_BACK_FAIL, "stencil back fail");
    ADD(STENCIL_BACK_FUNC, "stencil back func");
    ADD(STENCIL_BACK_PASS_DEPTH_FAIL, "stencil back pass depth fail");
    ADD(STENCIL_BACK_PASS_DEPTH_PASS, "stencil back pass depth pass");
    ADD(STENCIL_BACK_REF, "stencil back ref");
    ADD(STENCIL_BACK_VALUE_MASK, "stencil back value mask");
    ADD(STENCIL_BACK_WRITEMASK, "stencil back writemask");
    ADD(STENCIL_CLEAR_VALUE, "stencil clear value");
    ADD(STENCIL_FAIL, "stencil fail");
    ADD(STENCIL_FUNC, "stencil func");
    ADD(STENCIL_PASS_DEPTH_FAIL, "stencil pass depth fail");
    ADD(STENCIL_PASS_DEPTH_PASS, "stencil pass depth pass");
    ADD(STENCIL_REF, "stencil ref");
    ADD(STENCIL_TEST, "stencil test");
    ADD(STENCIL_VALUE_MASK, "stencil value mask");
    ADD(STENCIL_WRITEMASK, "stencil writemask");
    ADD(STEREO, "stereo");
    /* Hints */
    ADD(FRAGMENT_SHADER_DERIVATIVE_HINT, "fragment shader derivative hint");
    ADD(LINE_SMOOTH_HINT, "line smooth hint");
    ADD(POLYGON_SMOOTH_HINT, "polygon smooth hint");
    ADD(TEXTURE_COMPRESSION_HINT, "texture compression hint");
    /* Image State */
    ADD(IMAGE_BINDING_ACCESS, "image binding access");
    ADD(IMAGE_BINDING_FORMAT, "image binding format");
    ADD(IMAGE_BINDING_NAME, "image binding name");
    ADD(IMAGE_BINDING_LAYER, "image binding layer");
    ADD(IMAGE_BINDING_LAYERED, "image binding layered");
    ADD(IMAGE_BINDING_LEVEL, "image binding level");
    /* Multisampling */
    ADD(MAX_SAMPLE_MASK_WORDS, "max sample mask words");
    ADD(MULTISAMPLE, "multisample");
    ADD(SAMPLE_ALPHA_TO_COVERAGE, "sample alpha to coverage");
    ADD(SAMPLE_ALPHA_TO_ONE, "sample alpha to one");
    ADD(SAMPLE_BUFFERS, "sample buffers");
    ADD(SAMPLE_COVERAGE, "sample coverage");
    ADD(SAMPLE_COVERAGE_INVERT, "sample coverage invert");
    ADD(SAMPLE_COVERAGE_VALUE, "sample coverage value");
    ADD(SAMPLE_MASK, "sample mask");
    ADD(SAMPLE_MASK_VALUE, "sample mask value");
    ADD(SAMPLES, "samples");
    /* Pixel Operations */
    ADD(BLEND, "blend");
    ADD(BLEND_COLOR, "blend color");
    ADD(BLEND_DST_ALPHA, "blend dst alpha");
    ADD(BLEND_DST_RGB, "blend dst rgb");
    ADD(BLEND_EQUATION_RGB, "blend equation rgb");
    ADD(BLEND_EQUATION_ALPHA, "blend equation alpha");
    ADD(BLEND_SRC_ALPHA, "blend src alpha");
    ADD(BLEND_SRC_RGB, "blend src rgb");
    ADD(COLOR_LOGIC_OP, "color logic op");
    ADD(DITHER, "dither");
    ADD(LOGIC_OP_MODE, "logic op mode");
    ADD(SCISSOR_BOX, "scissor box");
    ADD(SCISSOR_TEST, "scissor test");
    /* Pixel Transfer Operations */
    ADD(CLAMP_READ_COLOR, "clamp read color");
    ADD(IMPLEMENTATION_COLOR_READ_FORMAT, "implementation color read format");
    ADD(IMPLEMENTATION_COLOR_READ_TYPE, "implementation color read type");
    ADD(PACK_ALIGNMENT, "pack alignment");
    ADD(PACK_COMPRESSED_BLOCK_DEPTH, "pack compressed block depth");
    ADD(PACK_COMPRESSED_BLOCK_HEIGHT, "pack compressed block height");
    ADD(PACK_COMPRESSED_BLOCK_SIZE, "pack compressed block size");
    ADD(PACK_COMPRESSED_BLOCK_WIDTH, "pack compressed block width");
    ADD(PACK_IMAGE_HEIGHT, "pack image height");
    ADD(PACK_LSB_FIRST, "pack lsb first");
    ADD(PACK_ROW_LENGTH, "pack row length");
    ADD(PACK_SKIP_IMAGES, "pack skip images");
    ADD(PACK_SKIP_PIXELS, "pack skip pixels");
    ADD(PACK_SKIP_ROWS, "pack skip rows");
    ADD(PACK_SWAP_BYTES, "pack swap bytes");
    ADD(PIXEL_PACK_BUFFER_BINDING, "pixel pack buffer binding");
    ADD(PIXEL_UNPACK_BUFFER_BINDING, "pixel unpack buffer binding");
    ADD(UNPACK_ALIGNMENT, "unpack alignment");
    ADD(UNPACK_COMPRESSED_BLOCK_DEPTH, "unpack compressed block depth");
    ADD(UNPACK_COMPRESSED_BLOCK_HEIGHT, "unpack compressed block height");
    ADD(UNPACK_COMPRESSED_BLOCK_SIZE, "unpack compressed block size");
    ADD(UNPACK_COMPRESSED_BLOCK_WIDTH, "unpack compressed block width");
    ADD(UNPACK_IMAGE_HEIGHT, "unpack image height");
    ADD(UNPACK_LSB_FIRST, "unpack lsb first");
    ADD(UNPACK_ROW_LENGTH, "unpack row length");
    ADD(UNPACK_SKIP_IMAGES, "unpack skip images");
    ADD(UNPACK_SKIP_PIXELS, "unpack skip pixels");
    ADD(UNPACK_SKIP_ROWS, "unpack skip rows");
    ADD(UNPACK_SWAP_BYTES, "unpack swap bytes");
    /* Programs */
    ADD(CURRENT_PROGRAM,  "current program"); 
    ADD(MAX_ATOMIC_COUNTER_BUFFER_SIZE, "max atomic counter buffer size");
    ADD(MAX_COMBINED_SHADER_OUTPUT_RESOURCES,  "max combined shader output resources"); 
    ADD(MAX_COMBINED_SHADER_STORAGE_BLOCKS,  "max combined shader storage blocks"); 
    ADD(MAX_IMAGE_SAMPLES,  "max image samples"); 
    ADD(MAX_IMAGE_UNITS,  "max image units"); 
    ADD(MAX_PROGRAM_TEXEL_OFFSET,  "max program texel offset"); 
    ADD(MAX_SHADER_STORAGE_BLOCK_SIZE,  "max shader storage block size"); 
    ADD(MAX_SUBROUTINES,  "max subroutines"); 
    ADD(MAX_SUBROUTINE_UNIFORM_LOCATIONS,  "max subroutine uniform locations"); 
    ADD(MAX_UNIFORM_BLOCK_SIZE,  "max uniform block size"); 
    ADD(MAX_UNIFORM_LOCATIONS,  "max uniform locations"); 
    ADD(MAX_VARYING_VECTORS,  "max varying vectors"); 
    ADD(MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,  "max vertex attrib relative offset"); 
    ADD(MAX_VERTEX_ATTRIB_BINDINGS,  "max vertex attrib bindings"); 
    ADD(MAX_VERTEX_ATTRIB_STRIDE,  "max vertex attrib stride"); 
    ADD(MIN_PROGRAM_TEXEL_OFFSET,  "min program texel offset"); 
    ADD(NUM_PROGRAM_BINARY_FORMATS,  "num program binary formats"); 
    ADD(NUM_SHADER_BINARY_FORMATS,  "num shader binary formats"); 
    ADD(PROGRAM_BINARY_FORMATS,  "program binary formats"); 
    ADD(PROGRAM_PIPELINE_BINDING,  "program pipeline binding"); 
    ADD(SHADER_BINARY_FORMATS,  "shader binary formats"); 
    ADD(SHADER_COMPILER, "shader compiler");
    ADD(SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,  "shader storage buffer offset alignment"); 
    ADD(UNIFORM_BUFFER_OFFSET_ALIGNMENT,  "uniform buffer offset alignment"); 
    /* Provoking Vertices */
    ADD(VIEWPORT_INDEX_PROVOKING_VERTEX, "viewport index provoking vertex");
    ADD(LAYER_PROVOKING_VERTEX, "layer provoking vertex");
    ADD(PROVOKING_VERTEX, "provoking vertex");
    /* Rasterization */
    ADD(ALIASED_LINE_WIDTH_RANGE, "aliased line width range");
    ADD(CULL_FACE,  "cull face"); 
    ADD(CULL_FACE_MODE,  "cull face mode"); 
    ADD(FRONT_FACE,  "front face"); 
    ADD(LINE_SMOOTH,  "line smooth"); 
    ADD(LINE_WIDTH,  "line width"); 
    ADD(POINT_FADE_THRESHOLD_SIZE,  "point fade threshold size"); 
    ADD(POINT_SIZE,  "point size"); 
    ADD(POINT_SIZE_GRANULARITY,  "point size granularity"); 
    ADD(POINT_SIZE_RANGE,  "point size range"); 
    ADD(POINT_SPRITE_COORD_ORIGIN,  "point sprite coord origin"); 
    ADD(POLYGON_MODE,  "polygon mode"); 
    ADD(POLYGON_OFFSET_FACTOR, "polygon offset factor");
    ADD(POLYGON_OFFSET_FILL,  "polygon offset fill"); 
    ADD(POLYGON_OFFSET_LINE,  "polygon offset line"); 
    ADD(POLYGON_OFFSET_POINT,  "polygon offset point"); 
    ADD(POLYGON_OFFSET_CLAMP, "polygon offset clamp"); //GL_VERSION_4_6
    ADD(POLYGON_OFFSET_UNITS, "polygon offset units");
    ADD(POLYGON_SMOOTH,  "polygon smooth"); 
    ADD(PROGRAM_POINT_SIZE,  "program point size"); 
    ADD(RASTERIZER_DISCARD,  "rasterizer discard"); 
    ADD(SMOOTH_LINE_WIDTH_RANGE,  "smooth line width range"); 
    ADD(SMOOTH_LINE_WIDTH_GRANULARITY,  "smooth line width granularity"); 
    ADD(SUBPIXEL_BITS,  "subpixel bits"); 
    /* Shader Execution */
    ADD(FRAGMENT_INTERPOLATION_OFFSET_BITS, "fragment interpolation offset bits");
    ADD(MAX_FRAGMENT_INTERPOLATION_OFFSET, "max fragment interpolation offset");
    ADD(MIN_FRAGMENT_INTERPOLATION_OFFSET, "min fragment interpolation offset");
    /* Shaders */
    ADD(MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, "max combined atomic counter buffers");
    ADD(MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, "max compute atomic counter buffers");
    ADD(MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, "max fragment atomic counter buffers");
    ADD(MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS, "max geometry atomic counter buffers");
    ADD(MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS, "max tess control atomic counter buffers");
    ADD(MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS, "max tess evaluation atomic counter buffers");
    ADD(MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, "max vertex atomic counter buffers");
    ADD(MAX_COMBINED_ATOMIC_COUNTERS, "max combined atomic counters");
    ADD(MAX_COMPUTE_ATOMIC_COUNTERS, "max compute atomic counters");
    ADD(MAX_FRAGMENT_ATOMIC_COUNTERS, "max fragment atomic counters");
    ADD(MAX_GEOMETRY_ATOMIC_COUNTERS, "max geometry atomic counters");
    ADD(MAX_TESS_CONTROL_ATOMIC_COUNTERS, "max tess control atomic counters");
    ADD(MAX_TESS_EVALUATION_ATOMIC_COUNTERS, "max tess evaluation atomic counters");
    ADD(MAX_VERTEX_ATOMIC_COUNTERS, "max vertex atomic counters");
    ADD(MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, "max combined compute uniform components");
    ADD(MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, "max combined fragment uniform components");
    ADD(MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, "max combined geometry uniform components");
    ADD(MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS, "max combined tess control uniform components");
    ADD(MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS, "max combined tess evaluation uniform components");
    ADD(MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, "max combined vertex uniform components");
    ADD(MAX_COMBINED_IMAGE_UNIFORMS, "max combined image uniforms");
    ADD(MAX_COMPUTE_IMAGE_UNIFORMS, "max compute image uniforms");
    ADD(MAX_FRAGMENT_IMAGE_UNIFORMS, "max fragment image uniforms");
    ADD(MAX_GEOMETRY_IMAGE_UNIFORMS, "max geometry image uniforms");
    ADD(MAX_TESS_CONTROL_IMAGE_UNIFORMS, "max tess control image uniforms");
    ADD(MAX_TESS_EVALUATION_IMAGE_UNIFORMS, "max tess evaluation image uniforms");
    ADD(MAX_VERTEX_IMAGE_UNIFORMS, "max vertex image uniforms");
    ADD(MAX_COMPUTE_SHADER_STORAGE_BLOCKS, "max compute shader storage blocks");
    ADD(MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, "max fragment shader storage blocks");
    ADD(MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, "max geometry shader storage blocks");
    ADD(MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, "max tess control shader storage blocks");
    ADD(MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, "max tess evaluation shader storage blocks");
    ADD(MAX_VERTEX_SHADER_STORAGE_BLOCKS, "max vertex shader storage blocks");
    ADD(MAX_COMPUTE_UNIFORM_COMPONENTS, "max compute uniform components");
    ADD(MAX_FRAGMENT_UNIFORM_COMPONENTS, "max fragment uniform components");
    ADD(MAX_GEOMETRY_UNIFORM_COMPONENTS, "max geometry uniform components");
    ADD(MAX_TESS_CONTROL_UNIFORM_COMPONENTS, "max tess control uniform components");
    ADD(MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, "max tess evaluation uniform components");
    ADD(MAX_VERTEX_UNIFORM_COMPONENTS, "max vertex uniform components");
    ADD(MAX_TEXTURE_IMAGE_UNITS, "max texture image units");
    ADD(MAX_COMBINED_TEXTURE_IMAGE_UNITS, "max combined texture image units");
    ADD(MAX_COMPUTE_TEXTURE_IMAGE_UNITS, "max compute texture image units");
    ADD(MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, "max geometry texture image units");
    ADD(MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, "max tess control texture image units");
    ADD(MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, "max tess evaluation texture image units");
    ADD(MAX_VERTEX_TEXTURE_IMAGE_UNITS, "max vertex texture image units");
    ADD(MAX_COMBINED_UNIFORM_BLOCKS, "max combined uniform blocks");
    ADD(MAX_COMPUTE_UNIFORM_BLOCKS, "max compute uniform blocks");
    ADD(MAX_FRAGMENT_UNIFORM_BLOCKS, "max fragment uniform blocks");
    ADD(MAX_GEOMETRY_UNIFORM_BLOCKS, "max geometry uniform blocks");
    ADD(MAX_TESS_CONTROL_UNIFORM_BLOCKS, "max tess control uniform blocks");
    ADD(MAX_TESS_EVALUATION_UNIFORM_BLOCKS, "max tess evaluation uniform blocks");
    ADD(MAX_VERTEX_UNIFORM_BLOCKS, "max vertex uniform blocks");
    /* Compute Shaders */
    ADD(DISPATCH_INDIRECT_BUFFER_BINDING, "dispatch indirect buffer binding");
    ADD(MAX_COMPUTE_SHARED_MEMORY_SIZE, "max compute shared memory size");
    ADD(MAX_COMPUTE_WORK_GROUP_INVOCATIONS, "max compute work group invocations");
    ADD(MAX_COMPUTE_WORK_GROUP_COUNT, "max compute work group count");
    ADD(MAX_COMPUTE_WORK_GROUP_SIZE, "max compute work group size");
    /* Fragment Shaders */
    ADD(MAX_FRAGMENT_INPUT_COMPONENTS, "max fragment input components");
    ADD(MAX_FRAGMENT_UNIFORM_VECTORS, "max fragment uniform vectors");
    ADD(MAX_PROGRAM_TEXTURE_GATHER_OFFSET, "max program texture gather offset");
    ADD(MIN_PROGRAM_TEXTURE_GATHER_OFFSET, "min program texture gather offset");
    /* Geometry Shaders */
    ADD(MAX_GEOMETRY_INPUT_COMPONENTS, "max geometry input components");
    ADD(MAX_GEOMETRY_OUTPUT_COMPONENTS, "max geometry output components");
    ADD(MAX_GEOMETRY_OUTPUT_VERTICES, "max geometry output vertices");
    ADD(MAX_GEOMETRY_SHADER_INVOCATIONS, "max geometry shader invocations");
    ADD(MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, "max geometry total output components");
    ADD(MAX_VERTEX_STREAMS, "max vertex streams");
    /* Tessellation Control Shaders */
    ADD(MAX_PATCH_VERTICES, "max patch vertices");
    ADD(MAX_TESS_CONTROL_INPUT_COMPONENTS, "max tess control input components");
    ADD(MAX_TESS_CONTROL_OUTPUT_COMPONENTS, "max tess control output components");
    ADD(MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, "max tess control total output components");
    ADD(MAX_TESS_GEN_LEVEL, "max tess gen level");
    ADD(MAX_TESS_PATCH_COMPONENTS, "max tess patch components");
    ADD(PATCH_DEFAULT_INNER_LEVEL, "patch default inner level");
    ADD(PATCH_DEFAULT_OUTER_LEVEL, "patch default outer level");
    ADD(PATCH_VERTICES, "patch vertices");
    /* Tessellation Evaluation Shaders */
    ADD(MAX_TESS_EVALUATION_INPUT_COMPONENTS, "max tess evaluation input components");
    ADD(MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, "max tess evaluation output components");
    /* Vertex Shaders */
    ADD(MAX_VERTEX_ATTRIBS, "max vertex attribs");
    ADD(MAX_VERTEX_OUTPUT_COMPONENTS, "max vertex output components");
    ADD(MAX_VERTEX_UNIFORM_VECTORS, "max vertex uniform vectors");
    /* Textures */
    ADD(ACTIVE_TEXTURE, "active texture");
    ADD(COMPRESSED_TEXTURE_FORMATS, "compressed texture formats");
    ADD(MAX_3D_TEXTURE_SIZE, "max 3d texture size");
    ADD(MAX_ARRAY_TEXTURE_LAYERS, "max array texture layers");
    ADD(MAX_CUBE_MAP_TEXTURE_SIZE, "max cube map texture size");
    ADD(MAX_RECTANGLE_TEXTURE_SIZE, "max rectangle texture size");
    ADD(MAX_RENDERBUFFER_SIZE, "max renderbuffer size");
    ADD(MAX_TEXTURE_BUFFER_SIZE, "max texture buffer size");
    ADD(MAX_TEXTURE_LOD_BIAS, "max texture lod bias");
    ADD(MAX_TEXTURE_SIZE, "max texture size");
    ADD(MAX_TEXTURE_MAX_ANISOTROPY, "max texture max anisotropy"); //GL_VERSION_4_6
    ADD(NUM_COMPRESSED_TEXTURE_FORMATS, "num compressed texture formats");
    ADD(SAMPLER_BINDING, "sampler binding");
    ADD(TEXTURE_BINDING_1D, "texture binding 1d");
    ADD(TEXTURE_BINDING_1D_ARRAY, "texture binding 1d array");
    ADD(TEXTURE_BINDING_2D, "texture binding 2d");
    ADD(TEXTURE_BINDING_2D_ARRAY, "texture binding 2d array");
    ADD(TEXTURE_BINDING_2D_MULTISAMPLE, "texture binding 2d multisample");
    ADD(TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, "texture binding 2d multisample array");
    ADD(TEXTURE_BINDING_3D, "texture binding 3d");
    ADD(TEXTURE_BINDING_BUFFER, "texture binding buffer");
    ADD(TEXTURE_BINDING_CUBE_MAP, "texture binding cube map");
    ADD(TEXTURE_BINDING_RECTANGLE, "texture binding rectangle");
    ADD(TEXTURE_BUFFER_OFFSET_ALIGNMENT, "texture buffer offset alignment");
    ADD(TEXTURE_CUBE_MAP_SEAMLESS, "texture cube map seamless");
    /* Transformation State */
    ADD(CLIP_DISTANCE0, "clip distance 0");
    ADD(CLIP_DISTANCE1, "clip distance 1");
    ADD(CLIP_DISTANCE2, "clip distance 2");
    ADD(CLIP_DISTANCE3, "clip distance 3");
    ADD(CLIP_DISTANCE4, "clip distance 4");
    ADD(CLIP_DISTANCE5, "clip distance 5");
    ADD(CLIP_DISTANCE6, "clip distance 6");
    ADD(CLIP_DISTANCE7, "clip distance 7");
    ADD(DEPTH_CLAMP, "depth clamp");
    ADD(DEPTH_RANGE, "depth range");
    ADD(MAX_CLIP_DISTANCES, "max clip distances");
    ADD(MAX_VIEWPORT_DIMS, "max viewport dims");
    ADD(MAX_VIEWPORTS, "max viewports");
    ADD(TRANSFORM_FEEDBACK_BINDING, "transform feedback binding");
    ADD(VIEWPORT, "viewport");
    ADD(VIEWPORT_BOUNDS_RANGE, "viewport bounds range");
    ADD(VIEWPORT_SUBPIXEL_BITS, "viewport subpixel bits");
    /* Vertex Arrays */
    ADD(MAX_ELEMENT_INDEX, "max element index");
    ADD(MAX_ELEMENTS_INDICES, "max elements indices");
    ADD(MAX_ELEMENTS_VERTICES, "max elements vertices");
    ADD(PRIMITIVE_RESTART, "primitive restart");
    ADD(PRIMITIVE_RESTART_FIXED_INDEX, "primitive restart fixed index");
    ADD(PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED, "primitive restart for patches supported");
    ADD(PRIMITIVE_RESTART_INDEX, "primitive restart index");
    ADD(VERTEX_BINDING_DIVISOR, "vertex binding divisor");
    ADD(VERTEX_BINDING_OFFSET, "vertex binding offset");
    ADD(VERTEX_BINDING_STRIDE, "vertex binding stride");
    ADD(MAX_VARYING_FLOATS, "max varying floats");

    domain = DOMAIN_RESET_NOTIFICATION_STRATEGY;
    ADD(NO_RESET_NOTIFICATION, "no reset notification");
    ADD(LOSE_CONTEXT_ON_RESET, "lose context on reset");

    domain = DOMAIN_GRAPHICS_RESET_STATUS;
    ADD(NO_ERROR, "no error");
    ADD(GUILTY_CONTEXT_RESET, "guilty context reset");
    ADD(INNOCENT_CONTEXT_RESET, "innocent context reset");
    ADD(UNKNOWN_CONTEXT_RESET, "unknown context reset");

    domain = DOMAIN_POLYGON_MODE_FACE;
    ADD(FRONT_AND_BACK, "front and back");

    domain = DOMAIN_POLYGON_MODE_MODE;
    ADD(POINT, "point");
    ADD(LINE, "line");
    ADD(FILL, "fill");

    domain = DOMAIN_FRONT_FACE_MODE;
    ADD(CCW, "ccw");
    ADD(CW, "cw");

    domain = DOMAIN_CULL_FACE_MODE;
    ADD(FRONT, "front");
    ADD(BACK, "back");
    ADD(FRONT_AND_BACK, "front and back");

    domain = DOMAIN_MULTISAMPLE_PNAME;
    ADD(SAMPLE_POSITION, "sample position");

    domain = DOMAIN_POINT_PNAME;
    ADD(POINT_FADE_THRESHOLD_SIZE, "fade threshold size");
    ADD(POINT_SPRITE_COORD_ORIGIN, "sprite coord origin");

    domain = DOMAIN_INTFORMAT_PNAME;
    ADD(CLEAR_BUFFER,  "clear buffer"); 
    ADD(CLEAR_TEXTURE, "clear texture");
    ADD(COLOR_ENCODING, "color encoding");
    ADD(COLOR_COMPONENTS, "color components");
    ADD(COLOR_RENDERABLE,  "color renderable"); 
    ADD(COMPUTE_TEXTURE,  "compute texture"); 
    ADD(DEPTH_COMPONENTS, "depth components");
    ADD(DEPTH_RENDERABLE, "depth renderable");
    ADD(FILTER, "filter");
    ADD(FRAMEBUFFER_BLEND, "framebuffer blend");
    ADD(FRAMEBUFFER_RENDERABLE, "framebuffer renderable");
    ADD(FRAMEBUFFER_RENDERABLE_LAYERED, "framebuffer renderable layered");
    ADD(FRAGMENT_TEXTURE,  "fragment texture"); 
    ADD(GEOMETRY_TEXTURE,  "geometry texture"); 
    ADD(GET_TEXTURE_IMAGE_FORMAT, "get texture image format");
    ADD(GET_TEXTURE_IMAGE_TYPE, "get texture image type");
    ADD(IMAGE_COMPATIBILITY_CLASS, "image compatibility class");
    ADD(IMAGE_PIXEL_FORMAT, "image pixel format");
    ADD(IMAGE_PIXEL_TYPE, "image pixel type");
    ADD(IMAGE_FORMAT_COMPATIBILITY_TYPE, "image format compatibility type");
    ADD(IMAGE_TEXEL_SIZE, "image texel size");
    ADD(INTERNALFORMAT_PREFERRED, "internalformat preferred");
    ADD(INTERNALFORMAT_SUPPORTED, "internalformat supported");
    ADD(INTERNALFORMAT_RED_SIZE, "internalformat red size");
    ADD(INTERNALFORMAT_GREEN_SIZE, "internalformat green size");
    ADD(INTERNALFORMAT_ALPHA_SIZE,  "internalformat alpha size"); 
    ADD(INTERNALFORMAT_DEPTH_SIZE,  "internalformat depth size"); 
    ADD(INTERNALFORMAT_STENCIL_SIZE,  "internalformat stencil size"); 
    ADD(INTERNALFORMAT_BLUE_SIZE,  "internalformat blue size"); 
    ADD(INTERNALFORMAT_SHARED_SIZE, "internalformat shared size");
    ADD(INTERNALFORMAT_RED_TYPE, "internalformat red type");
    ADD(INTERNALFORMAT_GREEN_TYPE,  "internalformat green type"); 
    ADD(INTERNALFORMAT_BLUE_TYPE,  "internalformat blue type"); 
    ADD(INTERNALFORMAT_ALPHA_TYPE,  "internalformat alpha type"); 
    ADD(INTERNALFORMAT_DEPTH_TYPE,  "internalformat depth type"); 
    ADD(INTERNALFORMAT_STENCIL_TYPE, "internalformat stencil type");
    ADD(MANUAL_GENERATE_MIPMAP, "manual generate mipmap");
    ADD(GENERATE_MIPMAP, "generate mipmap");
    ADD(AUTO_GENERATE_MIPMAP, "auto generate mipmap");
    ADD(MIPMAP, "mipmap");
    ADD(MAX_COMBINED_DIMENSIONS, "max combined dimensions");
    ADD(MAX_WIDTH, "max width");
    ADD(MAX_HEIGHT, "max height");
    ADD(MAX_DEPTH,  "max depth"); 
    ADD(MAX_LAYERS, "max layers");
    ADD(NUM_SAMPLE_COUNTS,  "num sample counts"); 
    ADD(READ_PIXELS, "read pixels");
    ADD(READ_PIXELS_FORMAT, "read pixels format");
    ADD(READ_PIXELS_TYPE, "read pixels type");
    ADD(SAMPLES,  "samples"); 
    ADD(SHADER_IMAGE_ATOMIC, "shader image atomic");
    ADD(SHADER_IMAGE_LOAD,  "shader image load"); 
    ADD(SHADER_IMAGE_STORE, "shader image store");
    ADD(SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST, "simultaneous texture and depth test");
    ADD(SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE, "simultaneous texture and depth write");
    ADD(SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST, "simultaneous texture and stencil test");
    ADD(SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE, "simultaneous texture and stencil write");
    ADD(SRGB_READ,  "srgb read"); 
    ADD(SRGB_WRITE, "srgb write");
    ADD(STENCIL_COMPONENTS, "stencil components");
    ADD(STENCIL_RENDERABLE, "stencil renderable");
    ADD(TESS_CONTROL_TEXTURE, "tess control texture");
    ADD(TESS_EVALUATION_TEXTURE, "tess evaluation texture");
    ADD(TEXTURE_COMPRESSED, "texture compressed");
    ADD(TEXTURE_COMPRESSED_BLOCK_SIZE, "texture compressed block size");
    ADD(TEXTURE_COMPRESSED_BLOCK_WIDTH,  "texture compressed block width"); 
    ADD(TEXTURE_COMPRESSED_BLOCK_HEIGHT, "texture compressed block height");
    ADD(TEXTURE_GATHER, "texture gather");
    ADD(TEXTURE_GATHER_SHADOW, "texture gather shadow");
    ADD(TEXTURE_IMAGE_FORMAT, "texture image format");
    ADD(TEXTURE_IMAGE_TYPE, "texture image type");
    ADD(TEXTURE_SHADOW,  "texture shadow"); 
    ADD(TEXTURE_VIEW,  "texture view"); 
    ADD(VERTEX_TEXTURE,  "vertex texture"); 
    ADD(VIEW_COMPATIBILITY_CLASS, "view compatibility class");

    domain = DOMAIN_SUPPORTED_OP;
    ADD(NONE, "none");
    ADD(CAVEAT_SUPPORT, "caveat");
    ADD(FULL_SUPPORT, "full");

    domain = DOMAIN_IMAGE_COMPATIBILITY_CLASS;
    ADD(NONE, "none");
    ADD(IMAGE_CLASS_4_X_32, "4 x 32");
    ADD(IMAGE_CLASS_2_X_32, "2 x 32");
    ADD(IMAGE_CLASS_1_X_32, "1 x 32");
    ADD(IMAGE_CLASS_4_X_16, "4 x 16");
    ADD(IMAGE_CLASS_2_X_16, "2 x 16");
    ADD(IMAGE_CLASS_1_X_16, "1 x 16");
    ADD(IMAGE_CLASS_4_X_8, "4 x 8");
    ADD(IMAGE_CLASS_2_X_8, "2 x 8");
    ADD(IMAGE_CLASS_1_X_8, "1 x 8");
    ADD(IMAGE_CLASS_11_11_10, "11 11 10");
    ADD(IMAGE_CLASS_10_10_10_2, "10 10 10 2");

    domain = DOMAIN_IMAGE_FORMAT_COMP_TYPE;
    ADD(NONE, "none");
    ADD(IMAGE_FORMAT_COMPATIBILITY_BY_SIZE, "by size");
    ADD(IMAGE_FORMAT_COMPATIBILITY_BY_CLASS, "by class");

    domain = DOMAIN_VIEW_COMPATIBILITY_TYPE;
    ADD(VIEW_CLASS_128_BITS, "128 bits");
    ADD(VIEW_CLASS_96_BITS, "96 bits");
    ADD(VIEW_CLASS_64_BITS, "64 bits");
    ADD(VIEW_CLASS_48_BITS, "48 bits");
    ADD(VIEW_CLASS_32_BITS, "32 bits");
    ADD(VIEW_CLASS_24_BITS, "24 bits");
    ADD(VIEW_CLASS_16_BITS, "16 bits");
    ADD(VIEW_CLASS_8_BITS, "8 bits");
    ADD(VIEW_CLASS_S3TC_DXT1_RGB, "s3tc dxt1 rgb");
    ADD(VIEW_CLASS_S3TC_DXT1_RGBA, "s3tc dxt1 rgba");
    ADD(VIEW_CLASS_S3TC_DXT3_RGBA, "s3tc dxt3 rgba");
    ADD(VIEW_CLASS_S3TC_DXT5_RGBA, "s3tc dxt5 rgba");
    ADD(VIEW_CLASS_RGTC1_RED, "rgtc1 red");
    ADD(VIEW_CLASS_RGTC2_RG, "rgtc2 rg");
    ADD(VIEW_CLASS_BPTC_UNORM, "bptc unorm");
    ADD(VIEW_CLASS_BPTC_FLOAT, "bptc float");

    domain = DOMAIN_SHADER_BINARY_FORMAT; //GL_VERSION_4_6
    ADD(SHADER_BINARY_FORMAT_SPIR_V, "spir v");

#undef ADD

#define ADD(what, s) do {                               \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, NONGL_##what, s);                 \
} while(0)

    domain = DOMAIN_UNIFORM_TYPE;
    ADD(BOOLEAN, "bool");
    ADD(INT, "int");
    ADD(UNSIGNED_INT, "uint");
    ADD(FLOAT, "float");
    ADD(DOUBLE, "double");

    domain = DOMAIN_MATRIX_DIMENSIONS;
    ADD(MATRIX_2x2, "2x2");
    ADD(MATRIX_3x3, "3x3");
    ADD(MATRIX_4x4, "4x4");
    ADD(MATRIX_2x3, "2x3");
    ADD(MATRIX_3x2, "3x2");
    ADD(MATRIX_2x4, "2x4");
    ADD(MATRIX_4x2, "4x2");
    ADD(MATRIX_3x4, "3x4");
    ADD(MATRIX_4x3, "4x3");

#undef ADD
    }

