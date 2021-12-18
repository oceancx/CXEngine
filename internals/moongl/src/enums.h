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

#ifndef enumsDEFINED
#define enumsDEFINED
#include <stdint.h>

/* enums.c */
#define enums_free_all moongl_enums_free_all
void enums_free_all(lua_State *L);
#define enums_test moongl_enums_test
uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err);
#define enums_check moongl_enums_check
uint32_t enums_check(lua_State *L, uint32_t domain, int arg);
#define enums_push moongl_enums_push
int enums_push(lua_State *L, uint32_t domain, uint32_t code);
#define enums_values moongl_enums_values
int enums_values(lua_State *L, uint32_t domain);
#define enums_checklist moongl_enums_checklist
uint32_t* enums_checklist(lua_State *L, uint32_t domain, int arg, uint32_t *count, int *err);
#define enums_freelist moongl_enums_freelist
void enums_freelist(lua_State *L, uint32_t *list);


/* Enum domains */
// #define DOMAIN_                          0 not used
#define DOMAIN_SHADER_TYPE                  1
#define DOMAIN_STAGE_PNAME                  2
#define DOMAIN_SUBROUTINE_PNAME             3
#define DOMAIN_PIPELINE_PNAME               4
#define DOMAIN_TRANSFORM_TARGET             5
#define DOMAIN_VERTEX_PNAME                 6
#define DOMAIN_TYPE                         7
#define DOMAIN_PROGRAM_PNAME                8
#define DOMAIN_PRIM_MODE                    9
#define DOMAIN_PROVOKE_MODE                 10
#define DOMAIN_ORIGIN                       11
#define DOMAIN_DEPTH                        12
#define DOMAIN_TRANSFORM_PNAME              13
#define DOMAIN_GLSL_TYPE                    14
#define DOMAIN_RENDERBUFFER_TARGET          15
#define DOMAIN_RENDERBUFFER_PNAME           16
#define DOMAIN_INTERNAL_FORMAT              17
#define DOMAIN_FORMAT                       18
#define DOMAIN_TEXTURE_TARGET               19
#define DOMAIN_TEXTURE_PNAME                20
#define DOMAIN_DEPTH_STENCIL                21
#define DOMAIN_COMPARE_FUNC                 22
#define DOMAIN_COMPARE_MODE                 23
#define DOMAIN_RGBA                         24
#define DOMAIN_WRAP                         25
#define DOMAIN_MAG_FILTER                   26
#define DOMAIN_MIN_FILTER                   27
#define DOMAIN_IMAGE_FORMAT_COMPATIBILITY   28
#define DOMAIN_LEVEL_PNAME                  29
#define DOMAIN_STENCIL_FUNC                 30
#define DOMAIN_FACE                         31
#define DOMAIN_STENCIL_OP                   32
#define DOMAIN_BLEND_MODE                   33
#define DOMAIN_BLEND_FACTOR                 34
#define DOMAIN_LOGIC_OP                     35
#define DOMAIN_FRAMEBUFFER_TARGET           36
#define DOMAIN_FRAMEBUFFER_PNAME            37
#define DOMAIN_FRAMEBUFFER_STATUS           38
#define DOMAIN_FRAMEBUFFER_GPNAME           39
#define DOMAIN_FRAMEBUFFER_APNAME           40
#define DOMAIN_OBJECT_TYPE                  41
#define DOMAIN_COMPONENT_TYPE               42
#define DOMAIN_COLOR_ENCODING               43
#define DOMAIN_DRAW_MODE                    44
#define DOMAIN_DRAW_TYPE                    45
#define DOMAIN_CONDITIONAL_RENDER_MODE      46
#define DOMAIN_FILTER                       47
#define DOMAIN_CLAMP_TARGET                 48
#define DOMAIN_CLAMP                        49
#define DOMAIN_IMAGE_TARGET                 50
#define DOMAIN_PIXEL_STORE_PNAME            51
#define DOMAIN_UNIFORM_TYPE                 52
#define DOMAIN_MATRIX_DIMENSIONS            53
#define DOMAIN_BUFFER_TARGET                54
#define DOMAIN_BUFFER_RANGE_TARGET          55
#define DOMAIN_BUFFER_USAGE                 56
#define DOMAIN_BUFFER_PNAME                 57
#define DOMAIN_BUFFER_ACCESS                58
#define DOMAIN_SHADER_PNAME                 59
#define DOMAIN_PRECISION_FORMAT             60
#define DOMAIN_CONDITION                    61
#define DOMAIN_SYNC_STATUS                  62
#define DOMAIN_SYNC_PNAME                   63
#define DOMAIN_INTERFACE                    64
#define DOMAIN_INTERFACE_PNAME              65
#define DOMAIN_RESOURCE_PROPERTY            66
#define DOMAIN_SAMPLER_PNAME                67
#define DOMAIN_CAPABILITY                   68
#define DOMAIN_PATCH_PNAME                  69
#define DOMAIN_HINT_TARGET                  70
#define DOMAIN_HINT_MODE                    71
#define DOMAIN_QUERY_TARGET                 72
#define DOMAIN_QUERY_PNAME                  73
#define DOMAIN_QUERY_OBJECT_PNAME           74
#define DOMAIN_STRING_NAME                  75
#define DOMAIN_UNIFORM_PNAME                76
#define DOMAIN_BLOCK_PNAME                  77
#define DOMAIN_ACB_PNAME                    78
#define DOMAIN_BUFFER                       79
#define DOMAIN_CLEAR_BUFFER                 80
#define DOMAIN_PROGRAM_PARAMETER_PNAME      81
#define DOMAIN_GEOMETRY_INPUT_TYPE          82
#define DOMAIN_GEOMETRY_OUTPUT_TYPE         83
#define DOMAIN_TESS_GEN_MODE                84
#define DOMAIN_TESS_GEN_SPACING             85
#define DOMAIN_TESS_GEN_VERTEX_ORDER        86
#define DOMAIN_BUFFER_MODE                  87
#define DOMAIN_DEBUG_SOURCE                 88
#define DOMAIN_DEBUG_TYPE                   89
#define DOMAIN_DEBUG_SEVERITY               90
#define DOMAIN_DEBUG_IDENTIFIER             91
#define DOMAIN_TEXTURE_ACCESS               92
#define DOMAIN_PNAME                        93
#define DOMAIN_RESET_NOTIFICATION_STRATEGY  94
#define DOMAIN_GRAPHICS_RESET_STATUS        95
#define DOMAIN_POLYGON_MODE_FACE            96
#define DOMAIN_POLYGON_MODE_MODE            97
#define DOMAIN_FRONT_FACE_MODE              98
#define DOMAIN_CULL_FACE_MODE               99
#define DOMAIN_MULTISAMPLE_PNAME            100
#define DOMAIN_POINT_PNAME                  101
#define DOMAIN_INTFORMAT_PNAME              102
#define DOMAIN_SUPPORTED_OP                 103
#define DOMAIN_IMAGE_COMPATIBILITY_CLASS    104
#define DOMAIN_IMAGE_FORMAT_COMP_TYPE       105
#define DOMAIN_VIEW_COMPATIBILITY_TYPE      106
#define DOMAIN_SHADER_BINARY_FORMAT         107

/* NONGL defines */
#define NONGL_BOOLEAN 0 /* see uniform.c */
#define NONGL_INT GL_INT
#define NONGL_UNSIGNED_INT GL_UNSIGNED_INT
#define NONGL_FLOAT GL_FLOAT
#define NONGL_DOUBLE GL_DOUBLE

#define NONGL_MATRIX_2x2  0x0202
#define NONGL_MATRIX_3x3  0x0303
#define NONGL_MATRIX_4x4  0x0404
#define NONGL_MATRIX_2x3  0x0203
#define NONGL_MATRIX_3x2  0x0302
#define NONGL_MATRIX_2x4  0x0204
#define NONGL_MATRIX_4x2  0x0402
#define NONGL_MATRIX_3x4  0x0304
#define NONGL_MATRIX_4x3  0x0403


#define testshadertype(L, arg, err) (GLenum)enums_test((L), DOMAIN_SHADER_TYPE, (arg), (err))
#define checkshadertype(L, arg) (GLenum)enums_check((L), DOMAIN_SHADER_TYPE, (arg))
#define pushshadertype(L, val) enums_push((L), DOMAIN_SHADER_TYPE, (uint32_t)(val))
#define valuesshadertype(L) enums_values((L), DOMAIN_SHADER_TYPE)

#define teststagepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_STAGE_PNAME, (arg), (err))
#define checkstagepname(L, arg) (GLenum)enums_check((L), DOMAIN_STAGE_PNAME, (arg))
#define pushstagepname(L, val) enums_push((L), DOMAIN_STAGE_PNAME, (uint32_t)(val))
#define valuesstagepname(L) enums_values((L), DOMAIN_STAGE_PNAME)

#define testsubroutinepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_SUBROUTINE_PNAME, (arg), (err))
#define checksubroutinepname(L, arg) (GLenum)enums_check((L), DOMAIN_SUBROUTINE_PNAME, (arg))
#define pushsubroutinepname(L, val) enums_push((L), DOMAIN_SUBROUTINE_PNAME, (uint32_t)(val))
#define valuessubroutinepname(L) enums_values((L), DOMAIN_SUBROUTINE_PNAME)

#define testpipelinepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PIPELINE_PNAME, (arg), (err))
#define checkpipelinepname(L, arg) (GLenum)enums_check((L), DOMAIN_PIPELINE_PNAME, (arg))
#define pushpipelinepname(L, val) enums_push((L), DOMAIN_PIPELINE_PNAME, (uint32_t)(val))
#define valuespipelinepname(L) enums_values((L), DOMAIN_PIPELINE_PNAME)

#define testtransformtarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_TRANSFORM_TARGET, (arg), (err))
#define checktransformtarget(L, arg) (GLenum)enums_check((L), DOMAIN_TRANSFORM_TARGET, (arg))
#define pushtransformtarget(L, val) enums_push((L), DOMAIN_TRANSFORM_TARGET, (uint32_t)(val))
#define valuestransformtarget(L) enums_values((L), DOMAIN_TRANSFORM_TARGET)

#define testvertexpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_VERTEX_PNAME, (arg), (err))
#define checkvertexpname(L, arg) (GLenum)enums_check((L), DOMAIN_VERTEX_PNAME, (arg))
#define pushvertexpname(L, val) enums_push((L), DOMAIN_VERTEX_PNAME, (uint32_t)(val))
#define valuesvertexpname(L) enums_values((L), DOMAIN_VERTEX_PNAME)

#define testtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_TYPE, (arg), (err))
#define checktype(L, arg) (GLenum)enums_check((L), DOMAIN_TYPE, (arg))
#define pushtype(L, val) enums_push((L), DOMAIN_TYPE, (uint32_t)(val))
#define valuestype(L) enums_values((L), DOMAIN_TYPE)

#define testprogrampname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PROGRAM_PNAME, (arg), (err))
#define checkprogrampname(L, arg) (GLenum)enums_check((L), DOMAIN_PROGRAM_PNAME, (arg))
#define pushprogrampname(L, val) enums_push((L), DOMAIN_PROGRAM_PNAME, (uint32_t)(val))
#define valuesprogrampname(L) enums_values((L), DOMAIN_PROGRAM_PNAME)

#define testprimmode(L, arg, err) (GLenum)enums_test((L), DOMAIN_PRIM_MODE, (arg), (err))
#define checkprimmode(L, arg) (GLenum)enums_check((L), DOMAIN_PRIM_MODE, (arg))
#define pushprimmode(L, val) enums_push((L), DOMAIN_PRIM_MODE, (uint32_t)(val))
#define valuesprimmode(L) enums_values((L), DOMAIN_PRIM_MODE)

#define testprovokemode(L, arg, err) (GLenum)enums_test((L), DOMAIN_PROVOKE_MODE, (arg), (err))
#define checkprovokemode(L, arg) (GLenum)enums_check((L), DOMAIN_PROVOKE_MODE, (arg))
#define pushprovokemode(L, val) enums_push((L), DOMAIN_PROVOKE_MODE, (uint32_t)(val))
#define valuesprovokemode(L) enums_values((L), DOMAIN_PROVOKE_MODE)

#define testorigin(L, arg, err) (GLenum)enums_test((L), DOMAIN_ORIGIN, (arg), (err))
#define checkorigin(L, arg) (GLenum)enums_check((L), DOMAIN_ORIGIN, (arg))
#define pushorigin(L, val) enums_push((L), DOMAIN_ORIGIN, (uint32_t)(val))
#define valuesorigin(L) enums_values((L), DOMAIN_ORIGIN)

#define testdepth(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEPTH, (arg), (err))
#define checkdepth(L, arg) (GLenum)enums_check((L), DOMAIN_DEPTH, (arg))
#define pushdepth(L, val) enums_push((L), DOMAIN_DEPTH, (uint32_t)(val))
#define valuesdepth(L) enums_values((L), DOMAIN_DEPTH)

#define testtransformpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_TRANSFORM_PNAME, (arg), (err))
#define checktransformpname(L, arg) (GLenum)enums_check((L), DOMAIN_TRANSFORM_PNAME, (arg))
#define pushtransformpname(L, val) enums_push((L), DOMAIN_TRANSFORM_PNAME, (uint32_t)(val))
#define valuestransformpname(L) enums_values((L), DOMAIN_TRANSFORM_PNAME)

#define testglsltype(L, arg, err) (GLenum)enums_test((L), DOMAIN_GLSL_TYPE, (arg), (err))
#define checkglsltype(L, arg) (GLenum)enums_check((L), DOMAIN_GLSL_TYPE, (arg))
#define pushglsltype(L, val) enums_push((L), DOMAIN_GLSL_TYPE, (uint32_t)(val))
#define valuesglsltype(L) enums_values((L), DOMAIN_GLSL_TYPE)

#define testrenderbuffertarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_RENDERBUFFER_TARGET, (arg), (err))
#define checkrenderbuffertarget(L, arg) (GLenum)enums_check((L), DOMAIN_RENDERBUFFER_TARGET, (arg))
#define pushrenderbuffertarget(L, val) enums_push((L), DOMAIN_RENDERBUFFER_TARGET, (uint32_t)(val))
#define valuesrenderbuffertarget(L) enums_values((L), DOMAIN_RENDERBUFFER_TARGET)

#define testrenderbufferpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_RENDERBUFFER_PNAME, (arg), (err))
#define checkrenderbufferpname(L, arg) (GLenum)enums_check((L), DOMAIN_RENDERBUFFER_PNAME, (arg))
#define pushrenderbufferpname(L, val) enums_push((L), DOMAIN_RENDERBUFFER_PNAME, (uint32_t)(val))
#define valuesrenderbufferpname(L) enums_values((L), DOMAIN_RENDERBUFFER_PNAME)

#define testinternalformat(L, arg, err) (GLenum)enums_test((L), DOMAIN_INTERNAL_FORMAT, (arg), (err))
#define checkinternalformat(L, arg) (GLenum)enums_check((L), DOMAIN_INTERNAL_FORMAT, (arg))
#define pushinternalformat(L, val) enums_push((L), DOMAIN_INTERNAL_FORMAT, (uint32_t)(val))
#define valuesinternalformat(L) enums_values((L), DOMAIN_INTERNAL_FORMAT)

#define testformat(L, arg, err) (GLenum)enums_test((L), DOMAIN_FORMAT, (arg), (err))
#define checkformat(L, arg) (GLenum)enums_check((L), DOMAIN_FORMAT, (arg))
#define pushformat(L, val) enums_push((L), DOMAIN_FORMAT, (uint32_t)(val))
#define valuesformat(L) enums_values((L), DOMAIN_FORMAT)

#define testtexturetarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_TEXTURE_TARGET, (arg), (err))
#define checktexturetarget(L, arg) (GLenum)enums_check((L), DOMAIN_TEXTURE_TARGET, (arg))
#define pushtexturetarget(L, val) enums_push((L), DOMAIN_TEXTURE_TARGET, (uint32_t)(val))
#define valuestexturetarget(L) enums_values((L), DOMAIN_TEXTURE_TARGET)

#define testtexturepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_TEXTURE_PNAME, (arg), (err))
#define checktexturepname(L, arg) (GLenum)enums_check((L), DOMAIN_TEXTURE_PNAME, (arg))
#define pushtexturepname(L, val) enums_push((L), DOMAIN_TEXTURE_PNAME, (uint32_t)(val))
#define valuestexturepname(L) enums_values((L), DOMAIN_TEXTURE_PNAME)

#define testdepthstencil(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEPTH_STENCIL, (arg), (err))
#define checkdepthstencil(L, arg) (GLenum)enums_check((L), DOMAIN_DEPTH_STENCIL, (arg))
#define pushdepthstencil(L, val) enums_push((L), DOMAIN_DEPTH_STENCIL, (uint32_t)(val))
#define valuesdepthstencil(L) enums_values((L), DOMAIN_DEPTH_STENCIL)

#define testcomparefunc(L, arg, err) (GLenum)enums_test((L), DOMAIN_COMPARE_FUNC, (arg), (err))
#define checkcomparefunc(L, arg) (GLenum)enums_check((L), DOMAIN_COMPARE_FUNC, (arg))
#define pushcomparefunc(L, val) enums_push((L), DOMAIN_COMPARE_FUNC, (uint32_t)(val))
#define valuescomparefunc(L) enums_values((L), DOMAIN_COMPARE_FUNC)

#define testcomparemode(L, arg, err) (GLenum)enums_test((L), DOMAIN_COMPARE_MODE, (arg), (err))
#define checkcomparemode(L, arg) (GLenum)enums_check((L), DOMAIN_COMPARE_MODE, (arg))
#define pushcomparemode(L, val) enums_push((L), DOMAIN_COMPARE_MODE, (uint32_t)(val))
#define valuescomparemode(L) enums_values((L), DOMAIN_COMPARE_MODE)

#define testrgba(L, arg, err) (GLenum)enums_test((L), DOMAIN_RGBA, (arg), (err))
#define checkrgba(L, arg) (GLenum)enums_check((L), DOMAIN_RGBA, (arg))
#define pushrgba(L, val) enums_push((L), DOMAIN_RGBA, (uint32_t)(val))
#define valuesrgba(L) enums_values((L), DOMAIN_RGBA)

#define testwrap(L, arg, err) (GLenum)enums_test((L), DOMAIN_WRAP, (arg), (err))
#define checkwrap(L, arg) (GLenum)enums_check((L), DOMAIN_WRAP, (arg))
#define pushwrap(L, val) enums_push((L), DOMAIN_WRAP, (uint32_t)(val))
#define valueswrap(L) enums_values((L), DOMAIN_WRAP)

#define testmagfilter(L, arg, err) (GLenum)enums_test((L), DOMAIN_MAG_FILTER, (arg), (err))
#define checkmagfilter(L, arg) (GLenum)enums_check((L), DOMAIN_MAG_FILTER, (arg))
#define pushmagfilter(L, val) enums_push((L), DOMAIN_MAG_FILTER, (uint32_t)(val))
#define valuesmagfilter(L) enums_values((L), DOMAIN_MAG_FILTER)

#define testminfilter(L, arg, err) (GLenum)enums_test((L), DOMAIN_MIN_FILTER, (arg), (err))
#define checkminfilter(L, arg) (GLenum)enums_check((L), DOMAIN_MIN_FILTER, (arg))
#define pushminfilter(L, val) enums_push((L), DOMAIN_MIN_FILTER, (uint32_t)(val))
#define valuesminfilter(L) enums_values((L), DOMAIN_MIN_FILTER)

#define testimageformatcompatibility(L, arg, err) (GLenum)enums_test((L), DOMAIN_IMAGE_FORMAT_COMPATIBILITY, (arg), (err))
#define checkimageformatcompatibility(L, arg) (GLenum)enums_check((L), DOMAIN_IMAGE_FORMAT_COMPATIBILITY, (arg))
#define pushimageformatcompatibility(L, val) enums_push((L), DOMAIN_IMAGE_FORMAT_COMPATIBILITY, (uint32_t)(val))
#define valuesimageformatcompatibility(L) enums_values((L), DOMAIN_IMAGE_FORMAT_COMPATIBILITY)

#define testlevelpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_LEVEL_PNAME, (arg), (err))
#define checklevelpname(L, arg) (GLenum)enums_check((L), DOMAIN_LEVEL_PNAME, (arg))
#define pushlevelpname(L, val) enums_push((L), DOMAIN_LEVEL_PNAME, (uint32_t)(val))
#define valueslevelpname(L) enums_values((L), DOMAIN_LEVEL_PNAME)

#define teststencilfunc(L, arg, err) (GLenum)enums_test((L), DOMAIN_STENCIL_FUNC, (arg), (err))
#define checkstencilfunc(L, arg) (GLenum)enums_check((L), DOMAIN_STENCIL_FUNC, (arg))
#define pushstencilfunc(L, val) enums_push((L), DOMAIN_STENCIL_FUNC, (uint32_t)(val))
#define valuesstencilfunc(L) enums_values((L), DOMAIN_STENCIL_FUNC)

#define testface(L, arg, err) (GLenum)enums_test((L), DOMAIN_FACE, (arg), (err))
#define checkface(L, arg) (GLenum)enums_check((L), DOMAIN_FACE, (arg))
#define pushface(L, val) enums_push((L), DOMAIN_FACE, (uint32_t)(val))
#define valuesface(L) enums_values((L), DOMAIN_FACE)

#define teststencilop(L, arg, err) (GLenum)enums_test((L), DOMAIN_STENCIL_OP, (arg), (err))
#define checkstencilop(L, arg) (GLenum)enums_check((L), DOMAIN_STENCIL_OP, (arg))
#define pushstencilop(L, val) enums_push((L), DOMAIN_STENCIL_OP, (uint32_t)(val))
#define valuesstencilop(L) enums_values((L), DOMAIN_STENCIL_OP)

#define testblendmode(L, arg, err) (GLenum)enums_test((L), DOMAIN_BLEND_MODE, (arg), (err))
#define checkblendmode(L, arg) (GLenum)enums_check((L), DOMAIN_BLEND_MODE, (arg))
#define pushblendmode(L, val) enums_push((L), DOMAIN_BLEND_MODE, (uint32_t)(val))
#define valuesblendmode(L) enums_values((L), DOMAIN_BLEND_MODE)

#define testblendfactor(L, arg, err) (GLenum)enums_test((L), DOMAIN_BLEND_FACTOR, (arg), (err))
#define checkblendfactor(L, arg) (GLenum)enums_check((L), DOMAIN_BLEND_FACTOR, (arg))
#define pushblendfactor(L, val) enums_push((L), DOMAIN_BLEND_FACTOR, (uint32_t)(val))
#define valuesblendfactor(L) enums_values((L), DOMAIN_BLEND_FACTOR)

#define testlogicop(L, arg, err) (GLenum)enums_test((L), DOMAIN_LOGIC_OP, (arg), (err))
#define checklogicop(L, arg) (GLenum)enums_check((L), DOMAIN_LOGIC_OP, (arg))
#define pushlogicop(L, val) enums_push((L), DOMAIN_LOGIC_OP, (uint32_t)(val))
#define valueslogicop(L) enums_values((L), DOMAIN_LOGIC_OP)

#define testframebuffertarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRAMEBUFFER_TARGET, (arg), (err))
#define checkframebuffertarget(L, arg) (GLenum)enums_check((L), DOMAIN_FRAMEBUFFER_TARGET, (arg))
#define pushframebuffertarget(L, val) enums_push((L), DOMAIN_FRAMEBUFFER_TARGET, (uint32_t)(val))
#define valuesframebuffertarget(L) enums_values((L), DOMAIN_FRAMEBUFFER_TARGET)

#define testframebufferpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRAMEBUFFER_PNAME, (arg), (err))
#define checkframebufferpname(L, arg) (GLenum)enums_check((L), DOMAIN_FRAMEBUFFER_PNAME, (arg))
#define pushframebufferpname(L, val) enums_push((L), DOMAIN_FRAMEBUFFER_PNAME, (uint32_t)(val))
#define valuesframebufferpname(L) enums_values((L), DOMAIN_FRAMEBUFFER_PNAME)

#define testframebufferstatus(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRAMEBUFFER_STATUS, (arg), (err))
#define checkframebufferstatus(L, arg) (GLenum)enums_check((L), DOMAIN_FRAMEBUFFER_STATUS, (arg))
#define pushframebufferstatus(L, val) enums_push((L), DOMAIN_FRAMEBUFFER_STATUS, (uint32_t)(val))
#define valuesframebufferstatus(L) enums_values((L), DOMAIN_FRAMEBUFFER_STATUS)

#define testframebuffergpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRAMEBUFFER_GPNAME, (arg), (err))
#define checkframebuffergpname(L, arg) (GLenum)enums_check((L), DOMAIN_FRAMEBUFFER_GPNAME, (arg))
#define pushframebuffergpname(L, val) enums_push((L), DOMAIN_FRAMEBUFFER_GPNAME, (uint32_t)(val))
#define valuesframebuffergpname(L) enums_values((L), DOMAIN_FRAMEBUFFER_GPNAME)

#define testframebufferapname(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRAMEBUFFER_APNAME, (arg), (err))
#define checkframebufferapname(L, arg) (GLenum)enums_check((L), DOMAIN_FRAMEBUFFER_APNAME, (arg))
#define pushframebufferapname(L, val) enums_push((L), DOMAIN_FRAMEBUFFER_APNAME, (uint32_t)(val))
#define valuesframebufferapname(L) enums_values((L), DOMAIN_FRAMEBUFFER_APNAME)

#define testobjecttype(L, arg, err) (GLenum)enums_test((L), DOMAIN_OBJECT_TYPE, (arg), (err))
#define checkobjecttype(L, arg) (GLenum)enums_check((L), DOMAIN_OBJECT_TYPE, (arg))
#define pushobjecttype(L, val) enums_push((L), DOMAIN_OBJECT_TYPE, (uint32_t)(val))
#define valuesobjecttype(L) enums_values((L), DOMAIN_OBJECT_TYPE)

#define testcomponenttype(L, arg, err) (GLenum)enums_test((L), DOMAIN_COMPONENT_TYPE, (arg), (err))
#define checkcomponenttype(L, arg) (GLenum)enums_check((L), DOMAIN_COMPONENT_TYPE, (arg))
#define pushcomponenttype(L, val) enums_push((L), DOMAIN_COMPONENT_TYPE, (uint32_t)(val))
#define valuescomponenttype(L) enums_values((L), DOMAIN_COMPONENT_TYPE)

#define testcolorencoding(L, arg, err) (GLenum)enums_test((L), DOMAIN_COLOR_ENCODING, (arg), (err))
#define checkcolorencoding(L, arg) (GLenum)enums_check((L), DOMAIN_COLOR_ENCODING, (arg))
#define pushcolorencoding(L, val) enums_push((L), DOMAIN_COLOR_ENCODING, (uint32_t)(val))
#define valuescolorencoding(L) enums_values((L), DOMAIN_COLOR_ENCODING)

#define testdrawmode(L, arg, err) (GLenum)enums_test((L), DOMAIN_DRAW_MODE, (arg), (err))
#define checkdrawmode(L, arg) (GLenum)enums_check((L), DOMAIN_DRAW_MODE, (arg))
#define pushdrawmode(L, val) enums_push((L), DOMAIN_DRAW_MODE, (uint32_t)(val))
#define valuesdrawmode(L) enums_values((L), DOMAIN_DRAW_MODE)

#define testdrawtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_DRAW_TYPE, (arg), (err))
#define checkdrawtype(L, arg) (GLenum)enums_check((L), DOMAIN_DRAW_TYPE, (arg))
#define pushdrawtype(L, val) enums_push((L), DOMAIN_DRAW_TYPE, (uint32_t)(val))
#define valuesdrawtype(L) enums_values((L), DOMAIN_DRAW_TYPE)

#define testconditionalrendermode(L, arg, err) (GLenum)enums_test((L), DOMAIN_CONDITIONAL_RENDER_MODE, (arg), (err))
#define checkconditionalrendermode(L, arg) (GLenum)enums_check((L), DOMAIN_CONDITIONAL_RENDER_MODE, (arg))
#define pushconditionalrendermode(L, val) enums_push((L), DOMAIN_CONDITIONAL_RENDER_MODE, (uint32_t)(val))
#define valuesconditionalrendermode(L) enums_values((L), DOMAIN_CONDITIONAL_RENDER_MODE)

#define testfilter(L, arg, err) (GLenum)enums_test((L), DOMAIN_FILTER, (arg), (err))
#define checkfilter(L, arg) (GLenum)enums_check((L), DOMAIN_FILTER, (arg))
#define pushfilter(L, val) enums_push((L), DOMAIN_FILTER, (uint32_t)(val))
#define valuesfilter(L) enums_values((L), DOMAIN_FILTER)

#define testclamptarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_CLAMP_TARGET, (arg), (err))
#define checkclamptarget(L, arg) (GLenum)enums_check((L), DOMAIN_CLAMP_TARGET, (arg))
#define pushclamptarget(L, val) enums_push((L), DOMAIN_CLAMP_TARGET, (uint32_t)(val))
#define valuesclamptarget(L) enums_values((L), DOMAIN_CLAMP_TARGET)

#define testclamp(L, arg, err) (GLenum)enums_test((L), DOMAIN_CLAMP, (arg), (err))
#define checkclamp(L, arg) (GLenum)enums_check((L), DOMAIN_CLAMP, (arg))
#define pushclamp(L, val) enums_push((L), DOMAIN_CLAMP, (uint32_t)(val))
#define valuesclamp(L) enums_values((L), DOMAIN_CLAMP)

#define testimagetarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_IMAGE_TARGET, (arg), (err))
#define checkimagetarget(L, arg) (GLenum)enums_check((L), DOMAIN_IMAGE_TARGET, (arg))
#define pushimagetarget(L, val) enums_push((L), DOMAIN_IMAGE_TARGET, (uint32_t)(val))
#define valuesimagetarget(L) enums_values((L), DOMAIN_IMAGE_TARGET)

#define testpixelstorepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PIXEL_STORE_PNAME, (arg), (err))
#define checkpixelstorepname(L, arg) (GLenum)enums_check((L), DOMAIN_PIXEL_STORE_PNAME, (arg))
#define pushpixelstorepname(L, val) enums_push((L), DOMAIN_PIXEL_STORE_PNAME, (uint32_t)(val))
#define valuespixelstorepname(L) enums_values((L), DOMAIN_PIXEL_STORE_PNAME)

#define testuniformtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_UNIFORM_TYPE, (arg), (err))
#define checkuniformtype(L, arg) (GLenum)enums_check((L), DOMAIN_UNIFORM_TYPE, (arg))
#define pushuniformtype(L, val) enums_push((L), DOMAIN_UNIFORM_TYPE, (uint32_t)(val))
#define valuesuniformtype(L) enums_values((L), DOMAIN_UNIFORM_TYPE)

#define testmatrixdimensions(L, arg, err) (GLenum)enums_test((L), DOMAIN_MATRIX_DIMENSIONS, (arg), (err))
#define checkmatrixdimensions(L, arg) (GLenum)enums_check((L), DOMAIN_MATRIX_DIMENSIONS, (arg))
#define pushmatrixdimensions(L, val) enums_push((L), DOMAIN_MATRIX_DIMENSIONS, (uint32_t)(val))
#define valuesmatrixdimensions(L) enums_values((L), DOMAIN_MATRIX_DIMENSIONS)

#define testbuffertarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_TARGET, (arg), (err))
#define checkbuffertarget(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_TARGET, (arg))
#define pushbuffertarget(L, val) enums_push((L), DOMAIN_BUFFER_TARGET, (uint32_t)(val))
#define valuesbuffertarget(L) enums_values((L), DOMAIN_BUFFER_TARGET)

#define testbufferrangetarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_RANGE_TARGET, (arg), (err))
#define checkbufferrangetarget(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_RANGE_TARGET, (arg))
#define pushbufferrangetarget(L, val) enums_push((L), DOMAIN_BUFFER_RANGE_TARGET, (uint32_t)(val))
#define valuesbufferrangetarget(L) enums_values((L), DOMAIN_BUFFER_RANGE_TARGET)

#define testbufferusage(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_USAGE, (arg), (err))
#define checkbufferusage(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_USAGE, (arg))
#define pushbufferusage(L, val) enums_push((L), DOMAIN_BUFFER_USAGE, (uint32_t)(val))
#define valuesbufferusage(L) enums_values((L), DOMAIN_BUFFER_USAGE)

#define testbufferpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_PNAME, (arg), (err))
#define checkbufferpname(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_PNAME, (arg))
#define pushbufferpname(L, val) enums_push((L), DOMAIN_BUFFER_PNAME, (uint32_t)(val))
#define valuesbufferpname(L) enums_values((L), DOMAIN_BUFFER_PNAME)

#define testbufferaccess(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_ACCESS, (arg), (err))
#define checkbufferaccess(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_ACCESS, (arg))
#define pushbufferaccess(L, val) enums_push((L), DOMAIN_BUFFER_ACCESS, (uint32_t)(val))
#define valuesbufferaccess(L) enums_values((L), DOMAIN_BUFFER_ACCESS)

#define testshaderpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_SHADER_PNAME, (arg), (err))
#define checkshaderpname(L, arg) (GLenum)enums_check((L), DOMAIN_SHADER_PNAME, (arg))
#define pushshaderpname(L, val) enums_push((L), DOMAIN_SHADER_PNAME, (uint32_t)(val))
#define valuesshaderpname(L) enums_values((L), DOMAIN_SHADER_PNAME)

#define testprecisionformat(L, arg, err) (GLenum)enums_test((L), DOMAIN_PRECISION_FORMAT, (arg), (err))
#define checkprecisionformat(L, arg) (GLenum)enums_check((L), DOMAIN_PRECISION_FORMAT, (arg))
#define pushprecisionformat(L, val) enums_push((L), DOMAIN_PRECISION_FORMAT, (uint32_t)(val))
#define valuesprecisionformat(L) enums_values((L), DOMAIN_PRECISION_FORMAT)

#define testcondition(L, arg, err) (GLenum)enums_test((L), DOMAIN_CONDITION, (arg), (err))
#define checkcondition(L, arg) (GLenum)enums_check((L), DOMAIN_CONDITION, (arg))
#define pushcondition(L, val) enums_push((L), DOMAIN_CONDITION, (uint32_t)(val))
#define valuescondition(L) enums_values((L), DOMAIN_CONDITION)

#define testsyncstatus(L, arg, err) (GLenum)enums_test((L), DOMAIN_SYNC_STATUS, (arg), (err))
#define checksyncstatus(L, arg) (GLenum)enums_check((L), DOMAIN_SYNC_STATUS, (arg))
#define pushsyncstatus(L, val) enums_push((L), DOMAIN_SYNC_STATUS, (uint32_t)(val))
#define valuessyncstatus(L) enums_values((L), DOMAIN_SYNC_STATUS)

#define testsyncpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_SYNC_PNAME, (arg), (err))
#define checksyncpname(L, arg) (GLenum)enums_check((L), DOMAIN_SYNC_PNAME, (arg))
#define pushsyncpname(L, val) enums_push((L), DOMAIN_SYNC_PNAME, (uint32_t)(val))
#define valuessyncpname(L) enums_values((L), DOMAIN_SYNC_PNAME)

#define testinterface(L, arg, err) (GLenum)enums_test((L), DOMAIN_INTERFACE, (arg), (err))
#define checkinterface(L, arg) (GLenum)enums_check((L), DOMAIN_INTERFACE, (arg))
#define pushinterface(L, val) enums_push((L), DOMAIN_INTERFACE, (uint32_t)(val))
#define valuesinterface(L) enums_values((L), DOMAIN_INTERFACE)

#define testinterfacepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_INTERFACE_PNAME, (arg), (err))
#define checkinterfacepname(L, arg) (GLenum)enums_check((L), DOMAIN_INTERFACE_PNAME, (arg))
#define pushinterfacepname(L, val) enums_push((L), DOMAIN_INTERFACE_PNAME, (uint32_t)(val))
#define valuesinterfacepname(L) enums_values((L), DOMAIN_INTERFACE_PNAME)

#define testresourceproperty(L, arg, err) (GLenum)enums_test((L), DOMAIN_RESOURCE_PROPERTY, (arg), (err))
#define checkresourceproperty(L, arg) (GLenum)enums_check((L), DOMAIN_RESOURCE_PROPERTY, (arg))
#define pushresourceproperty(L, val) enums_push((L), DOMAIN_RESOURCE_PROPERTY, (uint32_t)(val))
#define valuesresourceproperty(L) enums_values((L), DOMAIN_RESOURCE_PROPERTY)

#define testsamplerpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_SAMPLER_PNAME, (arg), (err))
#define checksamplerpname(L, arg) (GLenum)enums_check((L), DOMAIN_SAMPLER_PNAME, (arg))
#define pushsamplerpname(L, val) enums_push((L), DOMAIN_SAMPLER_PNAME, (uint32_t)(val))
#define valuessamplerpname(L) enums_values((L), DOMAIN_SAMPLER_PNAME)

#define testcapability(L, arg, err) (GLenum)enums_test((L), DOMAIN_CAPABILITY, (arg), (err))
#define checkcapability(L, arg) (GLenum)enums_check((L), DOMAIN_CAPABILITY, (arg))
#define pushcapability(L, val) enums_push((L), DOMAIN_CAPABILITY, (uint32_t)(val))
#define valuescapability(L) enums_values((L), DOMAIN_CAPABILITY)

#define testpatchpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PATCH_PNAME, (arg), (err))
#define checkpatchpname(L, arg) (GLenum)enums_check((L), DOMAIN_PATCH_PNAME, (arg))
#define pushpatchpname(L, val) enums_push((L), DOMAIN_PATCH_PNAME, (uint32_t)(val))
#define valuespatchpname(L) enums_values((L), DOMAIN_PATCH_PNAME)

#define testhinttarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_HINT_TARGET, (arg), (err))
#define checkhinttarget(L, arg) (GLenum)enums_check((L), DOMAIN_HINT_TARGET, (arg))
#define pushhinttarget(L, val) enums_push((L), DOMAIN_HINT_TARGET, (uint32_t)(val))
#define valueshinttarget(L) enums_values((L), DOMAIN_HINT_TARGET)

#define testhintmode(L, arg, err) (GLenum)enums_test((L), DOMAIN_HINT_MODE, (arg), (err))
#define checkhintmode(L, arg) (GLenum)enums_check((L), DOMAIN_HINT_MODE, (arg))
#define pushhintmode(L, val) enums_push((L), DOMAIN_HINT_MODE, (uint32_t)(val))
#define valueshintmode(L) enums_values((L), DOMAIN_HINT_MODE)

#define testquerytarget(L, arg, err) (GLenum)enums_test((L), DOMAIN_QUERY_TARGET, (arg), (err))
#define checkquerytarget(L, arg) (GLenum)enums_check((L), DOMAIN_QUERY_TARGET, (arg))
#define pushquerytarget(L, val) enums_push((L), DOMAIN_QUERY_TARGET, (uint32_t)(val))
#define valuesquerytarget(L) enums_values((L), DOMAIN_QUERY_TARGET)

#define testquerypname(L, arg, err) (GLenum)enums_test((L), DOMAIN_QUERY_PNAME, (arg), (err))
#define checkquerypname(L, arg) (GLenum)enums_check((L), DOMAIN_QUERY_PNAME, (arg))
#define pushquerypname(L, val) enums_push((L), DOMAIN_QUERY_PNAME, (uint32_t)(val))
#define valuesquerypname(L) enums_values((L), DOMAIN_QUERY_PNAME)

#define testqueryobjectpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_QUERY_OBJECT_PNAME, (arg), (err))
#define checkqueryobjectpname(L, arg) (GLenum)enums_check((L), DOMAIN_QUERY_OBJECT_PNAME, (arg))
#define pushqueryobjectpname(L, val) enums_push((L), DOMAIN_QUERY_OBJECT_PNAME, (uint32_t)(val))
#define valuesqueryobjectpname(L) enums_values((L), DOMAIN_QUERY_OBJECT_PNAME)

#define teststringname(L, arg, err) (GLenum)enums_test((L), DOMAIN_STRING_NAME, (arg), (err))
#define checkstringname(L, arg) (GLenum)enums_check((L), DOMAIN_STRING_NAME, (arg))
#define pushstringname(L, val) enums_push((L), DOMAIN_STRING_NAME, (uint32_t)(val))
#define valuesstringname(L) enums_values((L), DOMAIN_STRING_NAME)

#define testuniformpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_UNIFORM_PNAME, (arg), (err))
#define checkuniformpname(L, arg) (GLenum)enums_check((L), DOMAIN_UNIFORM_PNAME, (arg))
#define pushuniformpname(L, val) enums_push((L), DOMAIN_UNIFORM_PNAME, (uint32_t)(val))
#define valuesuniformpname(L) enums_values((L), DOMAIN_UNIFORM_PNAME)

#define testblockpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_BLOCK_PNAME, (arg), (err))
#define checkblockpname(L, arg) (GLenum)enums_check((L), DOMAIN_BLOCK_PNAME, (arg))
#define pushblockpname(L, val) enums_push((L), DOMAIN_BLOCK_PNAME, (uint32_t)(val))
#define valuesblockpname(L) enums_values((L), DOMAIN_BLOCK_PNAME)

#define testacbpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_ACB_PNAME, (arg), (err))
#define checkacbpname(L, arg) (GLenum)enums_check((L), DOMAIN_ACB_PNAME, (arg))
#define pushacbpname(L, val) enums_push((L), DOMAIN_ACB_PNAME, (uint32_t)(val))
#define valuesacbpname(L) enums_values((L), DOMAIN_ACB_PNAME)

#define testbuffer(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER, (arg), (err))
#define checkbuffer(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER, (arg))
#define pushbuffer(L, val) enums_push((L), DOMAIN_BUFFER, (uint32_t)(val))
#define valuesbuffer(L) enums_values((L), DOMAIN_BUFFER)

#define testclearbuffer(L, arg, err) (GLenum)enums_test((L), DOMAIN_CLEAR_BUFFER, (arg), (err))
#define checkclearbuffer(L, arg) (GLenum)enums_check((L), DOMAIN_CLEAR_BUFFER, (arg))
#define pushclearbuffer(L, val) enums_push((L), DOMAIN_CLEAR_BUFFER, (uint32_t)(val))
#define valuesclearbuffer(L) enums_values((L), DOMAIN_CLEAR_BUFFER)

#define testprogramparameterpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PROGRAM_PARAMETER_PNAME, (arg), (err))
#define checkprogramparameterpname(L, arg) (GLenum)enums_check((L), DOMAIN_PROGRAM_PARAMETER_PNAME, (arg))
#define pushprogramparameterpname(L, val) enums_push((L), DOMAIN_PROGRAM_PARAMETER_PNAME, (uint32_t)(val))
#define valuesprogramparameterpname(L) enums_values((L), DOMAIN_PROGRAM_PARAMETER_PNAME)

#define testgeometryinputtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_GEOMETRY_INPUT_TYPE, (arg), (err))
#define checkgeometryinputtype(L, arg) (GLenum)enums_check((L), DOMAIN_GEOMETRY_INPUT_TYPE, (arg))
#define pushgeometryinputtype(L, val) enums_push((L), DOMAIN_GEOMETRY_INPUT_TYPE, (uint32_t)(val))
#define valuesgeometryinputtype(L) enums_values((L), DOMAIN_GEOMETRY_INPUT_TYPE)

#define testgeometryoutputtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_GEOMETRY_OUTPUT_TYPE, (arg), (err))
#define checkgeometryoutputtype(L, arg) (GLenum)enums_check((L), DOMAIN_GEOMETRY_OUTPUT_TYPE, (arg))
#define pushgeometryoutputtype(L, val) enums_push((L), DOMAIN_GEOMETRY_OUTPUT_TYPE, (uint32_t)(val))
#define valuesgeometryoutputtype(L) enums_values((L), DOMAIN_GEOMETRY_OUTPUT_TYPE)

#define testtessgenmode(L, arg, err) (GLenum)enums_test((L), DOMAIN_TESS_GEN_MODE, (arg), (err))
#define checktessgenmode(L, arg) (GLenum)enums_check((L), DOMAIN_TESS_GEN_MODE, (arg))
#define pushtessgenmode(L, val) enums_push((L), DOMAIN_TESS_GEN_MODE, (uint32_t)(val))
#define valuestessgenmode(L) enums_values((L), DOMAIN_TESS_GEN_MODE)

#define testtessgenspacing(L, arg, err) (GLenum)enums_test((L), DOMAIN_TESS_GEN_SPACING, (arg), (err))
#define checktessgenspacing(L, arg) (GLenum)enums_check((L), DOMAIN_TESS_GEN_SPACING, (arg))
#define pushtessgenspacing(L, val) enums_push((L), DOMAIN_TESS_GEN_SPACING, (uint32_t)(val))
#define valuestessgenspacing(L) enums_values((L), DOMAIN_TESS_GEN_SPACING)

#define testtessgenvertexorder(L, arg, err) (GLenum)enums_test((L), DOMAIN_TESS_GEN_VERTEX_ORDER, (arg), (err))
#define checktessgenvertexorder(L, arg) (GLenum)enums_check((L), DOMAIN_TESS_GEN_VERTEX_ORDER, (arg))
#define pushtessgenvertexorder(L, val) enums_push((L), DOMAIN_TESS_GEN_VERTEX_ORDER, (uint32_t)(val))
#define valuestessgenvertexorder(L) enums_values((L), DOMAIN_TESS_GEN_VERTEX_ORDER)

#define testbuffermode(L, arg, err) (GLenum)enums_test((L), DOMAIN_BUFFER_MODE, (arg), (err))
#define checkbuffermode(L, arg) (GLenum)enums_check((L), DOMAIN_BUFFER_MODE, (arg))
#define pushbuffermode(L, val) enums_push((L), DOMAIN_BUFFER_MODE, (uint32_t)(val))
#define valuesbuffermode(L) enums_values((L), DOMAIN_BUFFER_MODE)

#define testdebugsource(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEBUG_SOURCE, (arg), (err))
#define checkdebugsource(L, arg) (GLenum)enums_check((L), DOMAIN_DEBUG_SOURCE, (arg))
#define pushdebugsource(L, val) enums_push((L), DOMAIN_DEBUG_SOURCE, (uint32_t)(val))
#define valuesdebugsource(L) enums_values((L), DOMAIN_DEBUG_SOURCE)

#define testdebugtype(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEBUG_TYPE, (arg), (err))
#define checkdebugtype(L, arg) (GLenum)enums_check((L), DOMAIN_DEBUG_TYPE, (arg))
#define pushdebugtype(L, val) enums_push((L), DOMAIN_DEBUG_TYPE, (uint32_t)(val))
#define valuesdebugtype(L) enums_values((L), DOMAIN_DEBUG_TYPE)

#define testdebugseverity(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEBUG_SEVERITY, (arg), (err))
#define checkdebugseverity(L, arg) (GLenum)enums_check((L), DOMAIN_DEBUG_SEVERITY, (arg))
#define pushdebugseverity(L, val) enums_push((L), DOMAIN_DEBUG_SEVERITY, (uint32_t)(val))
#define valuesdebugseverity(L) enums_values((L), DOMAIN_DEBUG_SEVERITY)

#define testdebugidentifier(L, arg, err) (GLenum)enums_test((L), DOMAIN_DEBUG_IDENTIFIER, (arg), (err))
#define checkdebugidentifier(L, arg) (GLenum)enums_check((L), DOMAIN_DEBUG_IDENTIFIER, (arg))
#define pushdebugidentifier(L, val) enums_push((L), DOMAIN_DEBUG_IDENTIFIER, (uint32_t)(val))
#define valuesdebugidentifier(L) enums_values((L), DOMAIN_DEBUG_IDENTIFIER)

#define testtextureaccess(L, arg, err) (GLenum)enums_test((L), DOMAIN_TEXTURE_ACCESS, (arg), (err))
#define checktextureaccess(L, arg) (GLenum)enums_check((L), DOMAIN_TEXTURE_ACCESS, (arg))
#define pushtextureaccess(L, val) enums_push((L), DOMAIN_TEXTURE_ACCESS, (uint32_t)(val))
#define valuestextureaccess(L) enums_values((L), DOMAIN_TEXTURE_ACCESS)

#define testpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_PNAME, (arg), (err))
#define checkpname(L, arg) (GLenum)enums_check((L), DOMAIN_PNAME, (arg))
#define pushpname(L, val) enums_push((L), DOMAIN_PNAME, (uint32_t)(val))
#define valuespname(L) enums_values((L), DOMAIN_PNAME)

#define testresetnotificationstrategy(L, arg, err) (GLenum)enums_test((L), DOMAIN_RESET_NOTIFICATION_STRATEGY, (arg), (err))
#define checkresetnotificationstrategy(L, arg) (GLenum)enums_check((L), DOMAIN_RESET_NOTIFICATION_STRATEGY, (arg))
#define pushresetnotificationstrategy(L, val) enums_push((L), DOMAIN_RESET_NOTIFICATION_STRATEGY, (uint32_t)(val))
#define valuesresetnotificationstrategy(L) enums_values((L), DOMAIN_RESET_NOTIFICATION_STRATEGY)

#define testgraphicsresetstatus(L, arg, err) (GLenum)enums_test((L), DOMAIN_GRAPHICS_RESET_STATUS, (arg), (err))
#define checkgraphicsresetstatus(L, arg) (GLenum)enums_check((L), DOMAIN_GRAPHICS_RESET_STATUS, (arg))
#define pushgraphicsresetstatus(L, val) enums_push((L), DOMAIN_GRAPHICS_RESET_STATUS, (uint32_t)(val))
#define valuesgraphicsresetstatus(L) enums_values((L), DOMAIN_GRAPHICS_RESET_STATUS)

#define testpolygonmodeface(L, arg, err) (GLenum)enums_test((L), DOMAIN_POLYGON_MODE_FACE, (arg), (err))
#define checkpolygonmodeface(L, arg) (GLenum)enums_check((L), DOMAIN_POLYGON_MODE_FACE, (arg))
#define pushpolygonmodeface(L, val) enums_push((L), DOMAIN_POLYGON_MODE_FACE, (uint32_t)(val))
#define valuespolygonmodeface(L) enums_values((L), DOMAIN_POLYGON_MODE_FACE)

#define testpolygonmodemode(L, arg, err) (GLenum)enums_test((L), DOMAIN_POLYGON_MODE_MODE, (arg), (err))
#define checkpolygonmodemode(L, arg) (GLenum)enums_check((L), DOMAIN_POLYGON_MODE_MODE, (arg))
#define pushpolygonmodemode(L, val) enums_push((L), DOMAIN_POLYGON_MODE_MODE, (uint32_t)(val))
#define valuespolygonmodemode(L) enums_values((L), DOMAIN_POLYGON_MODE_MODE)

#define testfrontfacemode(L, arg, err) (GLenum)enums_test((L), DOMAIN_FRONT_FACE_MODE, (arg), (err))
#define checkfrontfacemode(L, arg) (GLenum)enums_check((L), DOMAIN_FRONT_FACE_MODE, (arg))
#define pushfrontfacemode(L, val) enums_push((L), DOMAIN_FRONT_FACE_MODE, (uint32_t)(val))
#define valuesfrontfacemode(L) enums_values((L), DOMAIN_FRONT_FACE_MODE)

#define testcullfacemode(L, arg, err) (GLenum)enums_test((L), DOMAIN_CULL_FACE_MODE, (arg), (err))
#define checkcullfacemode(L, arg) (GLenum)enums_check((L), DOMAIN_CULL_FACE_MODE, (arg))
#define pushcullfacemode(L, val) enums_push((L), DOMAIN_CULL_FACE_MODE, (uint32_t)(val))
#define valuescullfacemode(L) enums_values((L), DOMAIN_CULL_FACE_MODE)

#define testmultisamplepname(L, arg, err) (GLenum)enums_test((L), DOMAIN_MULTISAMPLE_PNAME, (arg), (err))
#define checkmultisamplepname(L, arg) (GLenum)enums_check((L), DOMAIN_MULTISAMPLE_PNAME, (arg))
#define pushmultisamplepname(L, val) enums_push((L), DOMAIN_MULTISAMPLE_PNAME, (uint32_t)(val))
#define valuesmultisamplepname(L) enums_values((L), DOMAIN_MULTISAMPLE_PNAME)

#define testpointpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_POINT_PNAME, (arg), (err))
#define checkpointpname(L, arg) (GLenum)enums_check((L), DOMAIN_POINT_PNAME, (arg))
#define pushpointpname(L, val) enums_push((L), DOMAIN_POINT_PNAME, (uint32_t)(val))
#define valuespointpname(L) enums_values((L), DOMAIN_POINT_PNAME)

#define testintformatpname(L, arg, err) (GLenum)enums_test((L), DOMAIN_INTFORMAT_PNAME, (arg), (err))
#define checkintformatpname(L, arg) (GLenum)enums_check((L), DOMAIN_INTFORMAT_PNAME, (arg))
#define pushintformatpname(L, val) enums_push((L), DOMAIN_INTFORMAT_PNAME, (uint32_t)(val))
#define valuesintformatpname(L) enums_values((L), DOMAIN_INTFORMAT_PNAME)

#define testsupportedop(L, arg, err) (GLenum)enums_test((L), DOMAIN_SUPPORTED_OP, (arg), (err))
#define checksupportedop(L, arg) (GLenum)enums_check((L), DOMAIN_SUPPORTED_OP, (arg))
#define pushsupportedop(L, val) enums_push((L), DOMAIN_SUPPORTED_OP, (uint32_t)(val))
#define valuessupportedop(L) enums_values((L), DOMAIN_SUPPORTED_OP)

#define testimagecompatibilityclass(L, arg, err) (GLenum)enums_test((L), DOMAIN_IMAGE_COMPATIBILITY_CLASS, (arg), (err))
#define checkimagecompatibilityclass(L, arg) (GLenum)enums_check((L), DOMAIN_IMAGE_COMPATIBILITY_CLASS, (arg))
#define pushimagecompatibilityclass(L, val) enums_push((L), DOMAIN_IMAGE_COMPATIBILITY_CLASS, (uint32_t)(val))
#define valuesimagecompatibilityclass(L) enums_values((L), DOMAIN_IMAGE_COMPATIBILITY_CLASS)

#define testimageformatcomptype(L, arg, err) (GLenum)enums_test((L), DOMAIN_IMAGE_FORMAT_COMP_TYPE, (arg), (err))
#define checkimageformatcomptype(L, arg) (GLenum)enums_check((L), DOMAIN_IMAGE_FORMAT_COMP_TYPE, (arg))
#define pushimageformatcomptype(L, val) enums_push((L), DOMAIN_IMAGE_FORMAT_COMP_TYPE, (uint32_t)(val))
#define valuesimageformatcomptype(L) enums_values((L), DOMAIN_IMAGE_FORMAT_COMP_TYPE)

#define testviewcompatibilitytype(L, arg, err) (GLenum)enums_test((L), DOMAIN_VIEW_COMPATIBILITY_TYPE, (arg), (err))
#define checkviewcompatibilitytype(L, arg) (GLenum)enums_check((L), DOMAIN_VIEW_COMPATIBILITY_TYPE, (arg))
#define pushviewcompatibilitytype(L, val) enums_push((L), DOMAIN_VIEW_COMPATIBILITY_TYPE, (uint32_t)(val))
#define valuesviewcompatibilitytype(L) enums_values((L), DOMAIN_VIEW_COMPATIBILITY_TYPE)

#define testshaderbinaryformat(L, arg, err) (GLenum)enums_test((L), DOMAIN_SHADER_BINARY_FORMAT, (arg), (err))
#define checkshaderbinaryformat(L, arg) (GLenum)enums_check((L), DOMAIN_SHADER_BINARY_FORMAT, (arg))
#define pushshaderbinaryformat(L, val) enums_push((L), DOMAIN_SHADER_BINARY_FORMAT, (uint32_t)(val))
#define valuesshaderbinaryformat(L) enums_values((L), DOMAIN_SHADER_BINARY_FORMAT)

#if 0 /* scaffolding 6yy */
#define testxxx(L, arg, err) (GLenum)enums_test((L), DOMAIN_XXX, (arg), (err))
#define checkxxx(L, arg) (GLenum)enums_check((L), DOMAIN_XXX, (arg))
#define pushxxx(L, val) enums_push((L), DOMAIN_XXX, (uint32_t)(val))
#define valuesxxx(L) enums_values((L), DOMAIN_XXX)
    CASE(xxx);

#endif

#endif /* enumsDEFINED */


