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

#ifndef wranglerDEFINED
#define wranglerDEFINED

/*------------------------------------------------------------------------------*
 | Wrangler hat ....                                                            |
 *------------------------------------------------------------------------------*/

#if 0
/*------------------------------------------------------------------------------*
 | GL_VERSION_3_3                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_3_3
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#define GL_SAMPLER_BINDING                0x8919
#define GL_RGB10_A2UI                     0x906F
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#define GL_INT_2_10_10_10_REV             0x8D9F
#define glBindFragDataLocationIndexed(prog,colorNumber,index,name)  notavailable(L,prog,colorNumber,index,name)
#define glGetFragDataIndex(prog,name)   notavailable(L,prog,name)
#define glGenSamplers(cnt,samplers) notavailable(L,cnt,samplers)
#define glDeleteSamplers(cnt,samplers)  notavailable(L,cnt,samplers)
#define glIsSampler(sampler)    notavailable(L,sampler)
#define glBindSampler(unit,sampler) notavailable(L,unit,sampler)
#define glSamplerParameteri(sampler,pname,param)    notavailable(L,sampler,pname,param)
#define glSamplerParameteriv(sampler,pname,param)   notavailable(L,sampler,pname,param)
#define glSamplerParameterf(sampler,pname,param)    notavailable(L,sampler,pname,param)
#define glSamplerParameterfv(sampler,pname,param)   notavailable(L,sampler,pname,param)
#define glSamplerParameterIiv(sampler,pname,param)  notavailable(L,sampler,pname,param)
#define glSamplerParameterIuiv(sampler,pname,param) notavailable(L,sampler,pname,param)
#define glGetSamplerParameteriv(sampler,pname,params)   notavailable(L,sampler,pname,params)
#define glGetSamplerParameterIiv(sampler,pname,params)  notavailable(L,sampler,pname,params)
#define glGetSamplerParameterfv(sampler,pname,params)   notavailable(L,sampler,pname,params)
#define glGetSamplerParameterIuiv(sampler,pname,params) notavailable(L,sampler,pname,params)
#define glQueryCounter(id,target)   notavailable(L,id,target)
#define glGetQueryObjecti64v(id,pname,params)   notavailable(L,id,pname,params)
#define glGetQueryObjectui64v(id,pname,params)  notavailable(L,id,pname,params)
#define glVertexAttribDivisor(index,divisor)    notavailable(L,index,divisor)
#define glVertexAttribP1ui(index,type,normalized,val)   notavailable(L,index,type,normalized,val)
#define glVertexAttribP1uiv(index,type,normalized,val)  notavailable(L,index,type,normalized,val)
#define glVertexAttribP2ui(index,type,normalized,val)   notavailable(L,index,type,normalized,val)
#define glVertexAttribP2uiv(index,type,normalized,val)  notavailable(L,index,type,normalized,val)
#define glVertexAttribP3ui(index,type,normalized,val)   notavailable(L,index,type,normalized,val)
#define glVertexAttribP3uiv(index,type,normalized,val)  notavailable(L,index,type,normalized,val)
#define glVertexAttribP4ui(index,type,normalized,val)   notavailable(L,index,type,normalized,val)
#define glVertexAttribP4uiv(index,type,normalized,val)  notavailable(L,index,type,normalized,val)

#endif /* GL_VERSION_3_3 */
#endif

/*------------------------------------------------------------------------------*
 | GL_VERSION_4_0                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_0
#define GL_SAMPLE_SHADING                 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE       0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY   0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY         0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW  0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY     0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#define GL_GEOMETRY_SHADER_INVOCATIONS    0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_MAX_VERTEX_STREAMS             0x8E71
#define GL_DOUBLE_VEC2                    0x8FFC
#define GL_DOUBLE_VEC3                    0x8FFD
#define GL_DOUBLE_VEC4                    0x8FFE
#define GL_DOUBLE_MAT2                    0x8F46
#define GL_DOUBLE_MAT3                    0x8F47
#define GL_DOUBLE_MAT4                    0x8F48
#define GL_DOUBLE_MAT2x3                  0x8F49
#define GL_DOUBLE_MAT2x4                  0x8F4A
#define GL_DOUBLE_MAT3x2                  0x8F4B
#define GL_DOUBLE_MAT3x4                  0x8F4C
#define GL_DOUBLE_MAT4x2                  0x8F4D
#define GL_DOUBLE_MAT4x3                  0x8F4E
#define GL_ACTIVE_SUBROUTINES             0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS     0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH   0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES                0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES     0x8E4A
#define GL_COMPATIBLE_SUBROUTINES         0x8E4B
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL      0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL      0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES   0x8E75
#define GL_TESS_GEN_MODE                  0x8E76
#define GL_TESS_GEN_SPACING               0x8E77
#define GL_TESS_GEN_VERTEX_ORDER          0x8E78
#define GL_TESS_GEN_POINT_MODE            0x8E79
#define GL_ISOLINES                       0x8E7A
#define GL_FRACTIONAL_ODD                 0x8E7B
#define GL_FRACTIONAL_EVEN                0x8E7C
#define GL_MAX_PATCH_VERTICES             0x8E7D
#define GL_MAX_TESS_GEN_LEVEL             0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS      0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING     0x8E25
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#define glMinSampleShading(val) notavailable(L,val)
#define glBlendEquationi(buf,mode)  notavailable(L,buf,mode)
#define glBlendEquationSeparatei(buf,modeRGB,modeAlpha) notavailable(L,buf,modeRGB,modeAlpha)
#define glBlendFunci(buf,src,dst)   notavailable(L,buf,src,dst)
#define glBlendFuncSeparatei(buf,srcRGB,dstRGB,srcAlpha,dstAlpha)   notavailable(L,buf,srcRGB,dstRGB,srcAlpha,dstAlpha)
#define glDrawArraysIndirect(mode,indirect) notavailable(L,mode,indirect)
#define glDrawElementsIndirect(mode,type,indirect)  notavailable(L,mode,type,indirect)
#define glUniform1d(loc,x)  notavailable(L,loc,x)
#define glUniform2d(loc,x,y)    notavailable(L,loc,x,y)
#define glUniform3d(loc,x,y,z)  notavailable(L,loc,x,y,z)
#define glUniform4d(loc,x,y,z,w)    notavailable(L,loc,x,y,z,w)
#define glUniform1dv(loc,cnt,val)   notavailable(L,loc,cnt,val)
#define glUniform2dv(loc,cnt,val)   notavailable(L,loc,cnt,val)
#define glUniform3dv(loc,cnt,val)   notavailable(L,loc,cnt,val)
#define glUniform4dv(loc,cnt,val)   notavailable(L,loc,cnt,val)
#define glUniformMatrix2dv(loc,cnt,trans,val)   notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix3dv(loc,cnt,trans,val)   notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix4dv(loc,cnt,trans,val)   notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix2x3dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix2x4dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix3x2dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix3x4dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix4x2dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glUniformMatrix4x3dv(loc,cnt,trans,val) notavailable(L,loc,cnt,trans,val)
#define glGetUniformdv(prog,loc,params) notavailable(L,prog,loc,params)
#define glGetSubroutineUniformLocation(prog,shadertype,name)    notavailable(L,prog,shadertype,name)
#define glGetSubroutineIndex(prog,shadertype,name)  notavailable(L,prog,shadertype,name)
#define glGetActiveSubroutineUniformiv(prog,shadertype,index,pname,vals)    notavailable(L,prog,shadertype,index,pname,vals)
#define glGetActiveSubroutineUniformName(prog,shadertype,index,bufsize,length,name) notavailable(L,prog,shadertype,index,bufsize,length,name)
#define glGetActiveSubroutineName(prog,shadertype,index,bufsize,length,name)    notavailable(L,prog,shadertype,index,bufsize,length,name)
#define glUniformSubroutinesuiv(shadertype,cnt,indices) notavailable(L,shadertype,cnt,indices)
#define glGetUniformSubroutineuiv(shadertype,loc,params)    notavailable(L,shadertype,loc,params)
#define glGetProgramStageiv(prog,shadertype,pname,vals) notavailable(L,prog,shadertype,pname,vals)
#define glPatchParameteri(pname,val)    notavailable(L,pname,val)
#define glPatchParameterfv(pname,vals)  notavailable(L,pname,vals)
#define glBindTransformFeedback(target,id)  notavailable(L,target,id)
#define glDeleteTransformFeedbacks(n,ids)   notavailable(L,n,ids)
#define glGenTransformFeedbacks(n,ids)  notavailable(L,n,ids)
#define glIsTransformFeedback(id)   notavailable(L,id)
#define glPauseTransformFeedback()  notavailable(L)
#define glResumeTransformFeedback() notavailable(L)
#define glResumeTransformFeedback() notavailable(L)
#define glResumeTransformFeedback() notavailable(L)
#define glResumeTransformFeedback() notavailable(L)
#define glDrawTransformFeedback(mode,id)    notavailable(L,mode,id)
#define glDrawTransformFeedbackStream(mode,id,stream)   notavailable(L,mode,id,stream)
#define glBeginQueryIndexed(target,index,id)    notavailable(L,target,index,id)
#define glEndQueryIndexed(target,index) notavailable(L,target,index)
#define glGetQueryIndexediv(target,index,pname,params)  notavailable(L,target,index,pname,params)
#endif /* GL_VERSION_4_0 */

/*------------------------------------------------------------------------------*
 | GL_VERSION_4_1                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_1
#define GL_FIXED                          0x140C
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_LOW_FLOAT                      0x8DF0
#define GL_MEDIUM_FLOAT                   0x8DF1
#define GL_HIGH_FLOAT                     0x8DF2
#define GL_LOW_INT                        0x8DF3
#define GL_MEDIUM_INT                     0x8DF4
#define GL_HIGH_INT                       0x8DF5
#define GL_SHADER_COMPILER                0x8DFA
#define GL_SHADER_BINARY_FORMATS          0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS      0x8DF9
#define GL_MAX_VERTEX_UNIFORM_VECTORS     0x8DFB
#define GL_MAX_VARYING_VECTORS            0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS   0x8DFD
#define GL_RGB565                         0x8D62
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH          0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS     0x87FE
#define GL_PROGRAM_BINARY_FORMATS         0x87FF
#define GL_VERTEX_SHADER_BIT              0x00000001
#define GL_FRAGMENT_SHADER_BIT            0x00000002
#define GL_GEOMETRY_SHADER_BIT            0x00000004
#define GL_TESS_CONTROL_SHADER_BIT        0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT     0x00000010
#define GL_ALL_SHADER_BITS                0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE              0x8258
#define GL_ACTIVE_PROGRAM                 0x8259
#define GL_PROGRAM_PIPELINE_BINDING       0x825A
#define GL_MAX_VIEWPORTS                  0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS         0x825C
#define GL_VIEWPORT_BOUNDS_RANGE          0x825D
#define GL_LAYER_PROVOKING_VERTEX         0x825E
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_UNDEFINED_VERTEX               0x8260
#define glReleaseShaderCompiler()   notavailable(L)
#define glShaderBinary(cnt,shaders,binaryformat,binary,length)  notavailable(L,cnt,shaders,binaryformat,binary,length)
#define glGetShaderPrecisionFormat(shadertype,precisiontype,range,precision)    notavailable(L,shadertype,precisiontype,range,precision)
#define glDepthRangef(n,f)  notavailable(L,n,f)
#define glClearDepthf(d)    notavailable(L,d)
#define glGetProgramBinary(prog,bufsz,length,binaryFormat,binary)   notavailable(L,prog,bufsz,length,binaryFormat,binary)
#define glProgramBinary(prog,binaryFormat,binary,length)    notavailable(L,prog,binaryFormat,binary,length)
#define glProgramParameteri(prog,pname,val) notavailable(L,prog,pname,val)
#define glUseProgramStages(pipeline,stages,prog)    notavailable(L,pipeline,stages,prog)
#define glActiveShaderProgram(pipeline,prog)    notavailable(L,pipeline,prog)
#define glCreateShaderProgramv(type,cnt,strings)    notavailable(L,type,cnt,strings)
#define glBindProgramPipeline(pipeline) notavailable(L,pipeline)
#define glDeleteProgramPipelines(n,pipelines)   notavailable(L,n,pipelines)
#define glGenProgramPipelines(n,pipelines)  notavailable(L,n,pipelines)
#define glIsProgramPipeline(pipeline)   notavailable(L,pipeline)
#define glGetProgramPipelineiv(pipeline,pname,params)   notavailable(L,pipeline,pname,params)
#define glProgramUniform1i(prog,loc,v0) notavailable(L,prog,loc,v0)
#define glProgramUniform1iv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform1f(prog,loc,v0) notavailable(L,prog,loc,v0)
#define glProgramUniform1fv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform1d(prog,loc,v0) notavailable(L,prog,loc,v0)
#define glProgramUniform1dv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform1ui(prog,loc,v0)    notavailable(L,prog,loc,v0)
#define glProgramUniform1uiv(prog,loc,cnt,val)  notavailable(L,prog,loc,cnt,val)
#define glProgramUniform2i(prog,loc,v0,v1)  notavailable(L,prog,loc,v0,v1)
#define glProgramUniform2iv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform2f(prog,loc,v0,v1)  notavailable(L,prog,loc,v0,v1)
#define glProgramUniform2fv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform2d(prog,loc,v0,v1)  notavailable(L,prog,loc,v0,v1)
#define glProgramUniform2dv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform2ui(prog,loc,v0,v1) notavailable(L,prog,loc,v0,v1)
#define glProgramUniform2uiv(prog,loc,cnt,val)  notavailable(L,prog,loc,cnt,val)
#define glProgramUniform3i(prog,loc,v0,v1,v2)   notavailable(L,prog,loc,v0,v1,v2)
#define glProgramUniform3iv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform3f(prog,loc,v0,v1,v2)   notavailable(L,prog,loc,v0,v1,v2)
#define glProgramUniform3fv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform3d(prog,loc,v0,v1,v2)   notavailable(L,prog,loc,v0,v1,v2)
#define glProgramUniform3dv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform3ui(prog,loc,v0,v1,v2)  notavailable(L,prog,loc,v0,v1,v2)
#define glProgramUniform3uiv(prog,loc,cnt,val)  notavailable(L,prog,loc,cnt,val)
#define glProgramUniform4i(prog,loc,v0,v1,v2,v3)    notavailable(L,prog,loc,v0,v1,v2,v3)
#define glProgramUniform4iv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform4f(prog,loc,v0,v1,v2,v3)    notavailable(L,prog,loc,v0,v1,v2,v3)
#define glProgramUniform4fv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform4d(prog,loc,v0,v1,v2,v3)    notavailable(L,prog,loc,v0,v1,v2,v3)
#define glProgramUniform4dv(prog,loc,cnt,val)   notavailable(L,prog,loc,cnt,val)
#define glProgramUniform4ui(prog,loc,v0,v1,v2,v3)   notavailable(L,prog,loc,v0,v1,v2,v3)
#define glProgramUniform4uiv(prog,loc,cnt,val)  notavailable(L,prog,loc,cnt,val)
#define glProgramUniformMatrix2fv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3fv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4fv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix2dv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3dv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4dv(prog,loc,cnt,trans,val)   notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix2x3fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3x2fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix2x4fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4x2fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3x4fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4x3fv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix2x3dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3x2dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix2x4dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4x2dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix3x4dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glProgramUniformMatrix4x3dv(prog,loc,cnt,trans,val) notavailable(L,prog,loc,cnt,trans,val)
#define glGetProgramPipelineInfoLog(pipeline,bufsz,length,infoLog)  notavailable(L,pipeline,bufsz,length,infoLog)
#define glVertexAttribL1d(index,x)  notavailable(L,index,x)
#define glVertexAttribL2d(index,x,y)    notavailable(L,index,x,y)
#define glVertexAttribL3d(index,x,y,z)  notavailable(L,index,x,y,z)
#define glVertexAttribL4d(index,x,y,z,w)    notavailable(L,index,x,y,z,w)
#define glVertexAttribL1dv(index,v) notavailable(L,index,v)
#define glVertexAttribL2dv(index,v) notavailable(L,index,v)
#define glVertexAttribL3dv(index,v) notavailable(L,index,v)
#define glVertexAttribL4dv(index,v) notavailable(L,index,v)
#define glVertexAttribLPointer(index,size,type,stride,pointer)  notavailable(L,index,size,type,stride,pointer)
#define glGetVertexAttribLdv(index,pname,params)    notavailable(L,index,pname,params)
#define glViewportArrayv(first,cnt,v)   notavailable(L,first,cnt,v)
#define glViewportIndexedf(index,x,y,w,h)   notavailable(L,index,x,y,w,h)
#define glViewportIndexedfv(index,v)    notavailable(L,index,v)
#define glScissorArrayv(first,cnt,v)    notavailable(L,first,cnt,v)
#define glScissorIndexed(index,left,bottom,width,height)    notavailable(L,index,left,bottom,width,height)
#define glScissorIndexedv(index,v)  notavailable(L,index,v)
#define glDepthRangeArrayv(first,cnt,v) notavailable(L,first,cnt,v)
#define glDepthRangeIndexed(index,n,f)  notavailable(L,index,n,f)
#define glGetFloati_v(target,index,data)    notavailable(L,target,index,data)
#define glGetDoublei_v(target,index,data)   notavailable(L,target,index,data)
#endif /* GL_VERSION_4_1 */

/*------------------------------------------------------------------------------*
 | GL_VERSION_4_2                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_2
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH  0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH  0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE   0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH    0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT   0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH    0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE     0x912E
#define GL_NUM_SAMPLE_COUNTS              0x9380
#define GL_MIN_MAP_BUFFER_ALIGNMENT       0x90BC
#define GL_ATOMIC_COUNTER_BUFFER          0x92C0
#define GL_ATOMIC_COUNTER_BUFFER_BINDING  0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START    0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE     0x92C3
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS     0x92D2
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS   0x92D5
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS   0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS   0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS  0x92D9
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_UNSIGNED_INT_ATOMIC_COUNTER    0x92DB
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT      0x00000002
#define GL_UNIFORM_BARRIER_BIT            0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT      0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT            0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT       0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT     0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT      0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT        0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT     0x00001000
#define GL_ALL_BARRIER_BITS               0xFFFFFFFF
#define GL_MAX_IMAGE_UNITS                0x8F38
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_IMAGE_BINDING_NAME             0x8F3A
#define GL_IMAGE_BINDING_LEVEL            0x8F3B
#define GL_IMAGE_BINDING_LAYERED          0x8F3C
#define GL_IMAGE_BINDING_LAYER            0x8F3D
#define GL_IMAGE_BINDING_ACCESS           0x8F3E
#define GL_IMAGE_1D                       0x904C
#define GL_IMAGE_2D                       0x904D
#define GL_IMAGE_3D                       0x904E
#define GL_IMAGE_2D_RECT                  0x904F
#define GL_IMAGE_CUBE                     0x9050
#define GL_IMAGE_BUFFER                   0x9051
#define GL_IMAGE_1D_ARRAY                 0x9052
#define GL_IMAGE_2D_ARRAY                 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY           0x9054
#define GL_IMAGE_2D_MULTISAMPLE           0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY     0x9056
#define GL_INT_IMAGE_1D                   0x9057
#define GL_INT_IMAGE_2D                   0x9058
#define GL_INT_IMAGE_3D                   0x9059
#define GL_INT_IMAGE_2D_RECT              0x905A
#define GL_INT_IMAGE_CUBE                 0x905B
#define GL_INT_IMAGE_BUFFER               0x905C
#define GL_INT_IMAGE_1D_ARRAY             0x905D
#define GL_INT_IMAGE_2D_ARRAY             0x905E
#define GL_INT_IMAGE_CUBE_MAP_ARRAY       0x905F
#define GL_INT_IMAGE_2D_MULTISAMPLE       0x9060
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_UNSIGNED_INT_IMAGE_1D          0x9062
#define GL_UNSIGNED_INT_IMAGE_2D          0x9063
#define GL_UNSIGNED_INT_IMAGE_3D          0x9064
#define GL_UNSIGNED_INT_IMAGE_2D_RECT     0x9065
#define GL_UNSIGNED_INT_IMAGE_CUBE        0x9066
#define GL_UNSIGNED_INT_IMAGE_BUFFER      0x9067
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY    0x9068
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY    0x9069
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_IMAGE_SAMPLES              0x906D
#define GL_IMAGE_BINDING_FORMAT           0x906E
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_MAX_VERTEX_IMAGE_UNIFORMS      0x90CA
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS    0x90CD
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS    0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS    0x90CF
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#define GL_TEXTURE_IMMUTABLE_FORMAT       0x912F
#define glDrawArraysInstancedBaseInstance(mode,first,cnt,instcnt,baseinst)  \
                notavailable(L,mode,first,cnt,instcnt,baseinst)
#define glDrawElementsInstancedBaseInstance(mode,cnt,type,indices,instcnt,baseinst) \
                notavailable(L,mode,cnt,type,indices,instcnt,baseinst)
#define glDrawElementsInstancedBaseVertexBaseInstance(mode,cnt,type,indices,instcnt,basevertex,baseinst)    notavailable(L,mode,cnt,type,indices,instcnt,basevertex,baseinst)
#define glGetInternalformativ(target,intformat,pname,bufsz,params)  \
                notavailable(L,target,intformat,pname,bufsz,params)
#define glGetActiveAtomicCounterBufferiv(prog,bufferIndex,pname,params) \
                notavailable(L,prog,bufferIndex,pname,params)
#define glBindImageTexture(unit,txt,lvl,layered,layer,access,format)    \
                notavailable(L,unit,txt,lvl,layered,layer,access,format)
#define glMemoryBarrier(barriers)   \
                notavailable(L,barriers)
#define glTexStorage1D(target,lvls,intformat,width) \
                notavailable(L,target,lvls,intformat,width)
#define glTexStorage2D(target,lvls,intformat,width,height)  \
                notavailable(L,target,lvls,intformat,width,height)
#define glTexStorage3D(target,lvls,intformat,width,height,depth)    \
                notavailable(L,target,lvls,intformat,width,height,depth)
#define glDrawTransformFeedbackInstanced(mode,id,instcnt)   \
                notavailable(L,mode,id,instcnt)
#define glDrawTransformFeedbackStreamInstanced(mode,id,stream,instcnt)  \
                notavailable(L,mode,id,stream,instcnt)
#endif /* GL_VERSION_4_2 */


/*------------------------------------------------------------------------------*
 | GL_VERSION_4_3                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_3
typedef void (*GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

#define GL_NUM_SHADING_LANGUAGE_VERSIONS  0x82E9
#define GL_VERTEX_ATTRIB_ARRAY_LONG       0x874E
#define GL_COMPRESSED_RGB8_ETC2           0x9274
#define GL_COMPRESSED_SRGB8_ETC2          0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_R11_EAC             0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC      0x9271
#define GL_COMPRESSED_RG11_EAC            0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC     0x9273
#define GL_PRIMITIVE_RESTART_FIXED_INDEX  0x8D69
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_MAX_ELEMENT_INDEX              0x8D6B
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS     0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS     0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS    0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE        0x8267
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_DISPATCH_INDIRECT_BUFFER       0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT             0x00000020
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_MAX_UNIFORM_LOCATIONS          0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#define GL_MAX_FRAMEBUFFER_LAYERS         0x9317
#define GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
#define GL_INTERNALFORMAT_SUPPORTED       0x826F
#define GL_INTERNALFORMAT_PREFERRED       0x8270
#define GL_INTERNALFORMAT_RED_SIZE        0x8271
#define GL_INTERNALFORMAT_GREEN_SIZE      0x8272
#define GL_INTERNALFORMAT_BLUE_SIZE       0x8273
#define GL_INTERNALFORMAT_ALPHA_SIZE      0x8274
#define GL_INTERNALFORMAT_DEPTH_SIZE      0x8275
#define GL_INTERNALFORMAT_STENCIL_SIZE    0x8276
#define GL_INTERNALFORMAT_SHARED_SIZE     0x8277
#define GL_INTERNALFORMAT_RED_TYPE        0x8278
#define GL_INTERNALFORMAT_GREEN_TYPE      0x8279
#define GL_INTERNALFORMAT_BLUE_TYPE       0x827A
#define GL_INTERNALFORMAT_ALPHA_TYPE      0x827B
#define GL_INTERNALFORMAT_DEPTH_TYPE      0x827C
#define GL_INTERNALFORMAT_STENCIL_TYPE    0x827D
#define GL_MAX_WIDTH                      0x827E
#define GL_MAX_HEIGHT                     0x827F
#define GL_MAX_DEPTH                      0x8280
#define GL_MAX_LAYERS                     0x8281
#define GL_MAX_COMBINED_DIMENSIONS        0x8282
#define GL_COLOR_COMPONENTS               0x8283
#define GL_DEPTH_COMPONENTS               0x8284
#define GL_STENCIL_COMPONENTS             0x8285
#define GL_COLOR_RENDERABLE               0x8286
#define GL_DEPTH_RENDERABLE               0x8287
#define GL_STENCIL_RENDERABLE             0x8288
#define GL_FRAMEBUFFER_RENDERABLE         0x8289
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_FRAMEBUFFER_BLEND              0x828B
#define GL_READ_PIXELS                    0x828C
#define GL_READ_PIXELS_FORMAT             0x828D
#define GL_READ_PIXELS_TYPE               0x828E
#define GL_TEXTURE_IMAGE_FORMAT           0x828F
#define GL_TEXTURE_IMAGE_TYPE             0x8290
#define GL_GET_TEXTURE_IMAGE_FORMAT       0x8291
#define GL_GET_TEXTURE_IMAGE_TYPE         0x8292
#define GL_MIPMAP                         0x8293
#define GL_MANUAL_GENERATE_MIPMAP         0x8294
#define GL_AUTO_GENERATE_MIPMAP           0x8295
#define GL_COLOR_ENCODING                 0x8296
#define GL_SRGB_READ                      0x8297
#define GL_SRGB_WRITE                     0x8298
#define GL_FILTER                         0x829A
#define GL_VERTEX_TEXTURE                 0x829B
#define GL_TESS_CONTROL_TEXTURE           0x829C
#define GL_TESS_EVALUATION_TEXTURE        0x829D
#define GL_GEOMETRY_TEXTURE               0x829E
#define GL_FRAGMENT_TEXTURE               0x829F
#define GL_COMPUTE_TEXTURE                0x82A0
#define GL_TEXTURE_SHADOW                 0x82A1
#define GL_TEXTURE_GATHER                 0x82A2
#define GL_TEXTURE_GATHER_SHADOW          0x82A3
#define GL_SHADER_IMAGE_LOAD              0x82A4
#define GL_SHADER_IMAGE_STORE             0x82A5
#define GL_SHADER_IMAGE_ATOMIC            0x82A6
#define GL_IMAGE_TEXEL_SIZE               0x82A7
#define GL_IMAGE_COMPATIBILITY_CLASS      0x82A8
#define GL_IMAGE_PIXEL_FORMAT             0x82A9
#define GL_IMAGE_PIXEL_TYPE               0x82AA
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE  0x82B3
#define GL_CLEAR_BUFFER                   0x82B4
#define GL_TEXTURE_VIEW                   0x82B5
#define GL_VIEW_COMPATIBILITY_CLASS       0x82B6
#define GL_FULL_SUPPORT                   0x82B7
#define GL_CAVEAT_SUPPORT                 0x82B8
#define GL_IMAGE_CLASS_4_X_32             0x82B9
#define GL_IMAGE_CLASS_2_X_32             0x82BA
#define GL_IMAGE_CLASS_1_X_32             0x82BB
#define GL_IMAGE_CLASS_4_X_16             0x82BC
#define GL_IMAGE_CLASS_2_X_16             0x82BD
#define GL_IMAGE_CLASS_1_X_16             0x82BE
#define GL_IMAGE_CLASS_4_X_8              0x82BF
#define GL_IMAGE_CLASS_2_X_8              0x82C0
#define GL_IMAGE_CLASS_1_X_8              0x82C1
#define GL_IMAGE_CLASS_11_11_10           0x82C2
#define GL_IMAGE_CLASS_10_10_10_2         0x82C3
#define GL_VIEW_CLASS_128_BITS            0x82C4
#define GL_VIEW_CLASS_96_BITS             0x82C5
#define GL_VIEW_CLASS_64_BITS             0x82C6
#define GL_VIEW_CLASS_48_BITS             0x82C7
#define GL_VIEW_CLASS_32_BITS             0x82C8
#define GL_VIEW_CLASS_24_BITS             0x82C9
#define GL_VIEW_CLASS_16_BITS             0x82CA
#define GL_VIEW_CLASS_8_BITS              0x82CB
#define GL_VIEW_CLASS_S3TC_DXT1_RGB       0x82CC
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA      0x82CD
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA      0x82CE
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA      0x82CF
#define GL_VIEW_CLASS_RGTC1_RED           0x82D0
#define GL_VIEW_CLASS_RGTC2_RG            0x82D1
#define GL_VIEW_CLASS_BPTC_UNORM          0x82D2
#define GL_VIEW_CLASS_BPTC_FLOAT          0x82D3
#define GL_UNIFORM                        0x92E1
#define GL_UNIFORM_BLOCK                  0x92E2
#define GL_PROGRAM_INPUT                  0x92E3
#define GL_PROGRAM_OUTPUT                 0x92E4
#define GL_BUFFER_VARIABLE                0x92E5
#define GL_SHADER_STORAGE_BLOCK           0x92E6
#define GL_VERTEX_SUBROUTINE              0x92E8
#define GL_TESS_CONTROL_SUBROUTINE        0x92E9
#define GL_TESS_EVALUATION_SUBROUTINE     0x92EA
#define GL_GEOMETRY_SUBROUTINE            0x92EB
#define GL_FRAGMENT_SUBROUTINE            0x92EC
#define GL_COMPUTE_SUBROUTINE             0x92ED
#define GL_VERTEX_SUBROUTINE_UNIFORM      0x92EE
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_GEOMETRY_SUBROUTINE_UNIFORM    0x92F1
#define GL_FRAGMENT_SUBROUTINE_UNIFORM    0x92F2
#define GL_COMPUTE_SUBROUTINE_UNIFORM     0x92F3
#define GL_TRANSFORM_FEEDBACK_VARYING     0x92F4
#define GL_ACTIVE_RESOURCES               0x92F5
#define GL_MAX_NAME_LENGTH                0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES       0x92F7
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB
#define GL_OFFSET                         0x92FC
#define GL_BLOCK_INDEX                    0x92FD
#define GL_ARRAY_STRIDE                   0x92FE
#define GL_MATRIX_STRIDE                  0x92FF
#define GL_IS_ROW_MAJOR                   0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX    0x9301
#define GL_BUFFER_BINDING                 0x9302
#define GL_BUFFER_DATA_SIZE               0x9303
#define GL_NUM_ACTIVE_VARIABLES           0x9304
#define GL_ACTIVE_VARIABLES               0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER    0x9306
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_REFERENCED_BY_GEOMETRY_SHADER  0x9309
#define GL_REFERENCED_BY_FRAGMENT_SHADER  0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER   0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE           0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE         0x930D
#define GL_LOCATION                       0x930E
#define GL_LOCATION_INDEX                 0x930F
#define GL_IS_PER_PATCH                   0x92E7
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BUFFER_START    0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE     0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE  0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#define GL_TEXTURE_BUFFER_OFFSET          0x919D
#define GL_TEXTURE_BUFFER_SIZE            0x919E
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#define GL_TEXTURE_VIEW_MIN_LEVEL         0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS        0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER         0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS        0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS       0x82DF
#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS     0x82DA
#define GL_VERTEX_BINDING_BUFFER          0x8F4F

#define glClearBufferData(target,intformat,format,type,data)    \
            notavailable(L,target,intformat,format,type,data)
#define glClearBufferSubData(target,intformat,ofs,size,format,type,data)    \
            notavailable(L,target,intformat,ofs,size,format,type,data)
#define glDispatchCompute(num_groups_x,num_groups_y,num_groups_z)   \
            notavailable(L,num_groups_x,num_groups_y,num_groups_z)
#define glDispatchComputeIndirect(indirect) \
            notavailable(L,indirect)
#define glCopyImageSubData(srcName,srcTarget,srcLevel,srcX,srcY,srcZ,dstName,dstTarget,dstLevel,dstX,dstY,dstZ,srcWidth,srcHeight,srcDepth) \
            notavailable(L,srcName,srcTarget,srcLevel,srcX,srcY,srcZ,dstName,dstTarget,dstLevel,dstX,dstY,dstZ,srcWidth,srcHeight,srcDepth)
#define glFramebufferParameteri(target,pname,param) \
            notavailable(L,target,pname,param)
#define glGetFramebufferParameteriv(target,pname,params)    \
            notavailable(L,target,pname,params)
#define glGetInternalformati64v(target,intformat,pname,bufsz,params)    \
            notavailable(L,target,intformat,pname,bufsz,params)
#define glInvalidateTexSubImage(txt,lvl,xofs,yofs,zofs,width,height,depth)  \
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth)
#define glInvalidateTexImage(txt,lvl)   \
            notavailable(L,txt,lvl)
#define glInvalidateBufferSubData(buffer,ofs,length)    \
            notavailable(L,buffer,ofs,length)
#define glInvalidateBufferData(buffer)  \
            notavailable(L,buffer)
#define glInvalidateFramebuffer(target,numAttachments,attachments)  \
            notavailable(L,target,numAttachments,attachments)
#define glInvalidateSubFramebuffer(target,numAttachments,attachments,x,y,width,height)  \
            notavailable(L,target,numAttachments,attachments,x,y,width,height)
#define glMultiDrawArraysIndirect(mode,indirect,drawcnt,stride) \
            notavailable(L,mode,indirect,drawcnt,stride)
#define glMultiDrawElementsIndirect(mode,type,indirect,drawcnt,stride)  \
            notavailable(L,mode,type,indirect,drawcnt,stride)
#define glGetProgramInterfaceiv(prog,progInterface,pname,params)    \
            notavailable(L,prog,progInterface,pname,params)
#define glGetProgramResourceIndex(prog,progInterface,name)  \
            notavailable(L,prog,progInterface,name)
#define glGetProgramResourceName(prog,progInterface,index,bufsz,length,name)    \
            notavailable(L,prog,progInterface,index,bufsz,length,name)
#define glGetProgramResourceiv(prog,progItf,index,propCount,props,bufsz,length,params)  \
            notavailable(L,prog,progItf,index,propCount,props,bufsz,length,params)
#define glGetProgramResourceLocation(prog,progInterface,name)   \
            notavailable(L,prog,progInterface,name)
#define glGetProgramResourceLocationIndex(prog,progInterface,name)  \
            notavailable(L,prog,progInterface,name)
#define glShaderStorageBlockBinding(prog,storageBlockIndex,storageBlockBinding) \
            notavailable(L,prog,storageBlockIndex,storageBlockBinding)
#define glTexBufferRange(target,intformat,buffer,ofs,size)  \
            notavailable(L,target,intformat,buffer,ofs,size)
#define glTexStorage2DMultisample(target,samples,intformat,width,height,fixsamploc) \
            notavailable(L,target,samples,intformat,width,height,fixsamploc)
#define glTexStorage3DMultisample(target,samples,intformat,width,height,depth,fixsamploc)   \
            notavailable(L,target,samples,intformat,width,height,depth,fixsamploc)
#define glTextureView(txt,target,origtxt,intformat,minlvl,numlvls,minlayer,numlayers)   \
            notavailable(L,txt,target,origtxt,intformat,minlvl,numlvls,minlayer,numlayers)
#define glBindVertexBuffer(bindingindex,buffer,ofs,stride)  \
            notavailable(L,bindingindex,buffer,ofs,stride)
#define glVertexAttribFormat(attribindex,size,type,normalized,relativeofs)  \
            notavailable(L,attribindex,size,type,normalized,relativeofs)
#define glVertexAttribIFormat(attribindex,size,type,relativeofs)    \
            notavailable(L,attribindex,size,type,relativeofs)
#define glVertexAttribLFormat(attribindex,size,type,relativeofs)    \
            notavailable(L,attribindex,size,type,relativeofs)
#define glVertexAttribBinding(attribindex,bindingindex) \
            notavailable(L,attribindex,bindingindex)
#define glVertexBindingDivisor(bindingindex,divisor)    \
            notavailable(L,bindingindex,divisor)
#define glDebugMessageControl(source,type,severity,cnt,ids,enabled) \
            notavailable(L,source,type,severity,cnt,ids,enabled)
#define glDebugMessageInsert(source,type,id,severity,length,buf)    \
            notavailable(L,source,type,id,severity,length,buf)
#define glDebugMessageCallback(callback,userParam)  \
            notavailable(L,callback,userParam)
#define glGetDebugMessageLog(cnt,bufsz,sources,types,ids,severities,lengths,messageLog) \
            notavailable(L,cnt,bufsz,sources,types,ids,severities,lengths,messageLog)
#define glPushDebugGroup(source,id,length,message)  \
            notavailable(L,source,id,length,message)
#define glPopDebugGroup()   \
            notavailable(L)
#define glObjectLabel(identifier,name,length,label) \
            notavailable(L,identifier,name,length,label)
#define glGetObjectLabel(identifier,name,bufsz,length,label)    \
            notavailable(L,identifier,name,bufsz,length,label)
#define glObjectPtrLabel(ptr,length,label)  \
            notavailable(L,ptr,length,label)
#define glGetObjectPtrLabel(ptr,bufsz,length,label) \
            notavailable(L,ptr,bufsz,length,label)
#endif /* GL_VERSION_4_3 */


/*------------------------------------------------------------------------------*
 | GL_VERSION_4_4                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_4

#define GL_MAX_VERTEX_ATTRIB_STRIDE       0x82E5
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_TEXTURE_BUFFER_BINDING         0x8C2A
#define GL_MAP_PERSISTENT_BIT             0x0040
#define GL_MAP_COHERENT_BIT               0x0080
#define GL_DYNAMIC_STORAGE_BIT            0x0100
#define GL_CLIENT_STORAGE_BIT             0x0200
#define GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT 0x00004000
#define GL_BUFFER_IMMUTABLE_STORAGE       0x821F
#define GL_BUFFER_STORAGE_FLAGS           0x8220
#define GL_CLEAR_TEXTURE                  0x9365
#define GL_LOCATION_COMPONENT             0x934A
#define GL_TRANSFORM_FEEDBACK_BUFFER_INDEX 0x934B
#define GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE 0x934C
#define GL_QUERY_BUFFER                   0x9192
#define GL_QUERY_BUFFER_BARRIER_BIT       0x00008000
#define GL_QUERY_BUFFER_BINDING           0x9193
#define GL_QUERY_RESULT_NO_WAIT           0x9194
#define GL_MIRROR_CLAMP_TO_EDGE           0x8743
#define glBufferStorage(target,size,data,flags) \
            notavailable(L,target,size,data,flags)
#define glClearTexImage(txt,lvl,format,type,data)   \
            notavailable(L,txt,lvl,format,type,data)
#define glClearTexSubImage(txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,data)  \
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,data)
#define glBindBuffersBase(target,first,cnt,buffers) \
            notavailable(L,target,first,cnt,buffers)
#define glBindBuffersRange(target,first,cnt,buffers,ofss,sizes)\
            notavailable(L,target,first,cnt,buffers,ofss,sizes)
#define glBindTextures(first,cnt,txts)  \
            notavailable(L,first,cnt,txts)
#define glBindSamplers(first,cnt,samplers)  \
            notavailable(L,first,cnt,samplers)
#define glBindImageTextures(first,cnt,txts) \
            notavailable(L,first,cnt,txts)
#define glBindVertexBuffers(first,cnt,buffers,ofss,strides) \
            notavailable(L,first,cnt,buffers,ofss,strides)
#endif /* GL_VERSION_4_4 */

/*------------------------------------------------------------------------------*
 | GL_VERSION_4_5                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_5

#define GL_CONTEXT_LOST                   0x0507
#define GL_NEGATIVE_ONE_TO_ONE            0x935E
#define GL_ZERO_TO_ONE                    0x935F
#define GL_CLIP_ORIGIN                    0x935C
#define GL_CLIP_DEPTH_MODE                0x935D
#define GL_QUERY_WAIT_INVERTED            0x8E17
#define GL_QUERY_NO_WAIT_INVERTED         0x8E18
#define GL_QUERY_BY_REGION_WAIT_INVERTED  0x8E19
#define GL_QUERY_BY_REGION_NO_WAIT_INVERTED 0x8E1A
#define GL_MAX_CULL_DISTANCES             0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES 0x82FA
#define GL_TEXTURE_TARGET                 0x1006
#define GL_QUERY_TARGET                   0x82EA
#define GL_TEXTURE_BINDING                0x82EB
#define GL_GUILTY_CONTEXT_RESET           0x8253
#define GL_INNOCENT_CONTEXT_RESET         0x8254
#define GL_UNKNOWN_CONTEXT_RESET          0x8255
#define GL_RESET_NOTIFICATION_STRATEGY    0x8256
#define GL_LOSE_CONTEXT_ON_RESET          0x8252
#define GL_NO_RESET_NOTIFICATION          0x8261
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT 0x00000004
#define GL_CONTEXT_RELEASE_BEHAVIOR       0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#define glClipControl(origin,depth) notavailable(L,origin,depth)
#define glCreateTransformFeedbacks(n,ids)   notavailable(L,n,ids)
#define glTransformFeedbackBufferBase(xfb,index,buffer) \
            notavailable(L,xfb,index,buffer)
#define glTransformFeedbackBufferRange(xfb,index,buffer,ptrofs,size)    \
            notavailable(L,xfb,index,buffer,ptrofs,size)
#define glGetTransformFeedbackiv(xfb,pname,param)   \
            notavailable(L,xfb,pname,param)
#define glGetTransformFeedbacki_v(xfb,pname,index,param)    \
            notavailable(L,xfb,pname,index,param)
#define glGetTransformFeedbacki64_v(xfb,pname,index,param)  \
            notavailable(L,xfb,pname,index,param)
#define glCreateBuffers(n,buffers)  \
            notavailable(L,n,buffers)
#define glNamedBufferStorage(buffer,size,data,flags)    \
            notavailable(L,buffer,size,data,flags)
#define glNamedBufferData(buffer,size,data,usage)   \
            notavailable(L,buffer,size,data,usage)
#define glNamedBufferSubData(buffer,ptrofs,size,data)   \
            notavailable(L,buffer,ptrofs,size,data)
#define glCopyNamedBufferSubData(readBuffer,writeBuffer,readOffset,writeOffset,size)    \
            notavailable(L,readBuffer,writeBuffer,readOffset,writeOffset,size)
#define glClearNamedBufferData(buffer,intformat,format,type,data)   \
            notavailable(L,buffer,intformat,format,type,data)
#define glClearNamedBufferSubData(buffer,intformat,ofs,size,format,type,data)   \
            notavailable(L,buffer,intformat,ofs,size,format,type,data)
#define glMapNamedBuffer(buffer,access) NULL, notavailable(L,buffer,access)
#define glMapNamedBufferRange(buffer,ofs,length,access) \
                NULL, notavailable(L,buffer,ofs,length,access)
#define glUnmapNamedBuffer(buffer)  \
            notavailable(L,buffer)
#define glFlushMappedNamedBufferRange(buffer,ofs,length)    \
            notavailable(L,buffer,ofs,length)
#define glGetNamedBufferParameteriv(buffer,pname,params)    \
            notavailable(L,buffer,pname,params)
#define glGetNamedBufferParameteri64v(buffer,pname,params)  \
            notavailable(L,buffer,pname,params)
#define glGetNamedBufferPointerv(buffer,pname,params)   \
            notavailable(L,buffer,pname,params)
#define glGetNamedBufferSubData(buffer,ofs,size,data)   \
            notavailable(L,buffer,ofs,size,data)
#define glCreateFramebuffers(n,framebuffers)    \
            notavailable(L,n,framebuffers)
#define glNamedFramebufferRenderbuffer(framebuffer,attachment,renderbuffertarget,renderbuffer)  \
            notavailable(L,framebuffer,attachment,renderbuffertarget,renderbuffer)
#define glNamedFramebufferParameteri(framebuffer,pname,param)   \
            notavailable(L,framebuffer,pname,param)
#define glNamedFramebufferTexture(framebuffer,attachment,txt,lvl)   \
            notavailable(L,framebuffer,attachment,txt,lvl)
#define glNamedFramebufferTextureLayer(framebuffer,attachment,txt,lvl,layer)    \
            notavailable(L,framebuffer,attachment,txt,lvl,layer)
#define glNamedFramebufferDrawBuffer(framebuffer,buf)   \
            notavailable(L,framebuffer,buf)
#define glNamedFramebufferDrawBuffers(framebuffer,n,bufs)   \
            notavailable(L,framebuffer,n,bufs)
#define glNamedFramebufferReadBuffer(framebuffer,src)   \
            notavailable(L,framebuffer,src)
#define glInvalidateNamedFramebufferData(framebuffer,numAttachments,attachments)    \
            notavailable(L,framebuffer,numAttachments,attachments)
#define glInvalidateNamedFramebufferSubData(framebuffer,numAttachments,attachments,x,y,idth,height) \
            notavailable(L,framebuffer,numAttachments,attachments,x,y,idth,height)
#define glClearNamedFramebufferiv(framebuffer,buffer,drawbuffer,val)    \
            notavailable(L,framebuffer,buffer,drawbuffer,val)
#define glClearNamedFramebufferuiv(framebuffer,buffer,drawbuffer,val)   \
            notavailable(L,framebuffer,buffer,drawbuffer,val)
#define glClearNamedFramebufferfv(framebuffer,buffer,drawbuffer,val)    \
            notavailable(L,framebuffer,buffer,drawbuffer,val)
#define glClearNamedFramebufferfi(framebuffer,buffer,drawbuffer,depth,stencil)  \
            notavailable(L,framebuffer,buffer,drawbuffer,depth,stencil)
#define glBlitNamedFramebuffer(readFramebuffer,drawFramebuffer,srcX0,srcY0,srcX1,srcY1,dstX0,dstY0,dstX1,dstY1,mask,filter) \
            notavailable(L,readFramebuffer,drawFramebuffer,srcX0,srcY0,srcX1,srcY1,dstX0,dstY0,dstX1,dstY1,mask,filter)
#define glCheckNamedFramebufferStatus(framebuffer,target)   \
            notavailable(L,framebuffer,target)
#define glGetNamedFramebufferParameteriv(framebuffer,pname,param)   \
            notavailable(L,framebuffer,pname,param)
#define glGetNamedFramebufferAttachmentParameteriv(framebuffer,attachment,pname,params) \
            notavailable(L,framebuffer,attachment,pname,params)
#define glCreateRenderbuffers(n,renderbuffers)  \
            notavailable(L,n,renderbuffers)
#define glNamedRenderbufferStorage(renderbuffer,intformat,width,height) \
            notavailable(L,renderbuffer,intformat,width,height)
#define glNamedRenderbufferStorageMultisample(renderbuffer,samples,intformat,width,height)  \
            notavailable(L,renderbuffer,samples,intformat,width,height)
#define glGetNamedRenderbufferParameteriv(renderbuffer,pname,params)    \
            notavailable(L,renderbuffer,pname,params)
#define glCreateTextures(target,n,txts) \
            notavailable(L,target,n,txts)
#define glTextureBuffer(txt,intformat,buffer)   \
            notavailable(L,txt,intformat,buffer)
#define glTextureBufferRange(txt,intformat,buffer,ofs,size) \
            notavailable(L,txt,intformat,buffer,ofs,size)
#define glTextureStorage1D(txt,lvls,intformat,width)    \
            notavailable(L,txt,lvls,intformat,width)
#define glTextureStorage2D(txt,lvls,intformat,width,height) \
            notavailable(L,txt,lvls,intformat,width,height)
#define glTextureStorage3D(txt,lvls,intformat,width,height,depth)   \
            notavailable(L,txt,lvls,intformat,width,height,depth)
#define glTextureStorage2DMultisample(txt,samples,intformat,width,height,fixsamploc)    \
            notavailable(L,txt,samples,intformat,width,height,fixsamploc)
#define glTextureStorage3DMultisample(txt,samples,intformat,width,height,depth,fixsamploc)  \
            notavailable(L,txt,samples,intformat,width,height,depth,fixsamploc)
#define glTextureSubImage1D(txt,lvl,xofs,width,format,type,pixels)  \
            notavailable(L,txt,lvl,xofs,width,format,type,pixels)
#define glTextureSubImage2D(txt,lvl,xofs,yofs,width,height,format,type,pixels)  \
            notavailable(L,txt,lvl,xofs,yofs,width,height,format,type,pixels)
#define glTextureSubImage3D(txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,pixels)   \
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,pixels)
#define glCompressedTextureSubImage1D(txt,lvl,xofs,width,format,imgsz,data)\
            notavailable(L,txt,lvl,xofs,width,format,imgsz,data)
#define glCompressedTextureSubImage2D(txt,lvl,xofs,yofs,width,height,format,imgsz,data) \
            notavailable(L,txt,lvl,xofs,yofs,width,height,format,imgsz,data)
#define glCompressedTextureSubImage3D(txt,lvl,xofs,yofs,zofs,width,height,depth,format,imgsz,data)\
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth,format,imgsz,data)
#define glCopyTextureSubImage1D(txt,lvl,xofs,x,y,width) \
            notavailable(L,txt,lvl,xofs,x,y,width)
#define glCopyTextureSubImage2D(txt,lvl,xofs,yofs,x,y,width,height) \
            notavailable(L,txt,lvl,xofs,yofs,x,y,width,height)
#define glCopyTextureSubImage3D(txt,lvl,xofs,yofs,zofs,x,y,width,height)    \
            notavailable(L,txt,lvl,xofs,yofs,zofs,x,y,width,height)
#define glTextureParameterf(txt,pname,param)notavailable(L,txt,pname,param)
#define glTextureParameterfv(txt,pname,param) notavailable(L,txt,pname,param)
#define glTextureParameteri(txt,pname,param) notavailable(L,txt,pname,param)
#define glTextureParameterIiv(txt,pname,params) notavailable(L,txt,pname,params)
#define glTextureParameterIuiv(txt,pname,params) notavailable(L,txt,pname,params)
#define glTextureParameteriv(txt,pname,param) notavailable(L,txt,pname,param)
#define glGenerateTextureMipmap(txt) notavailable(L,txt)
#define glBindTextureUnit(unit,txt) notavailable(L,unit,txt)
#define glGetTextureImage(txt,lvl,format,type,bufsz,pixels) \
            notavailable(L,txt,lvl,format,type,bufsz,pixels)
#define glGetCompressedTextureImage(txt,lvl,bufsz,pixels)   \
            notavailable(L,txt,lvl,bufsz,pixels)
#define glGetTextureLevelParameterfv(txt,lvl,pname,params) notavailable(L,txt,lvl,pname,params)
#define glGetTextureLevelParameteriv(txt,lvl,pname,params) notavailable(L,txt,lvl,pname,params)
#define glGetTextureParameterfv(txt,pname,params) notavailable(L,txt,pname,params)
#define glGetTextureParameterIiv(txt,pname,params)  notavailable(L,txt,pname,params)
#define glGetTextureParameterIuiv(txt,pname,params) notavailable(L,txt,pname,params)
#define glGetTextureParameteriv(txt,pname,params) notavailable(L,txt,pname,params)
#define glCreateVertexArrays(n,arrays)  notavailable(L,n,arrays)
#define glDisableVertexArrayAttrib(vaobj,index) notavailable(L,vaobj,index)
#define glEnableVertexArrayAttrib(vaobj,index) notavailable(L,vaobj,index)
#define glVertexArrayElementBuffer(vaobj,buffer) notavailable(L,vaobj,buffer)
#define glVertexArrayVertexBuffer(vaobj,bindingindex,buffer,ofs,stride) \
            notavailable(L,vaobj,bindingindex,buffer,ofs,stride)
#define glVertexArrayVertexBuffers(vaobj,first,cnt,buffers,ofss,strides)    \
            notavailable(L,vaobj,first,cnt,buffers,ofss,strides)
#define glVertexArrayAttribBinding(vaobj,attribindex,bindingindex)  \
            notavailable(L,vaobj,attribindex,bindingindex)
#define glVertexArrayAttribFormat(vaobj,attribindex,size,type,normalized,relativeofs)   \
            notavailable(L,vaobj,attribindex,size,type,normalized,relativeofs)
#define glVertexArrayAttribIFormat(vaobj,attribindex,size,type,relativeofs) \
            notavailable(L,vaobj,attribindex,size,type,relativeofs)
#define glVertexArrayAttribLFormat(vaobj,attribindex,size,type,relativeofs) \
            notavailable(L,vaobj,attribindex,size,type,relativeofs)
#define glVertexArrayBindingDivisor(vaobj,bindingindex,divisor) \
            notavailable(L,vaobj,bindingindex,divisor)
#define glGetVertexArrayiv(vaobj,pname,param)   notavailable(L,vaobj,pname,param)
#define glGetVertexArrayIndexediv(vaobj,index,pname,param)  notavailable(L,vaobj,index,pname,param)
#define glGetVertexArrayIndexed64iv(vaobj,index,pname,param)    \
            notavailable(L,vaobj,index,pname,param)
#define glCreateSamplers(n,samplers) notavailable(L,n,samplers)
#define glCreateProgramPipelines(n,pipelines)   notavailable(L,n,pipelines)
#define glCreateQueries(target,n,ids)   notavailable(L,target,n,ids)
#define glMemoryBarrierByRegion(barriers)   notavailable(L,barriers)
#define glGetTextureSubImage(txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,bufsz,pixels)\
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth,format,type,bufsz,pixels)
#define glGetCompressedTextureSubImage(txt,lvl,xofs,yofs,zofs,width,height,depth,bufsz,pixels)\
            notavailable(L,txt,lvl,xofs,yofs,zofs,width,height,depth,bufsz,pixels)
#define glGetGraphicsResetStatus()  notavailable(L)
#define glGetnCompressedTexImage(target,lod,bufsz,pixels)   \
            notavailable(L,target,lod,bufsz,pixels)
#define glGetnTexImage(target,lvl,format,type,bufsz,pixels) \
            notavailable(L,target,lvl,format,type,bufsz,pixels)
#define glGetnUniformdv(prog,loc,bufsz,params)  notavailable(L,prog,loc,bufsz,params)
#define glGetnUniformfv(prog,loc,bufsz,params)  notavailable(L,prog,loc,bufsz,params)
#define glGetnUniformiv(prog,loc,bufsz,params)  notavailable(L,prog,loc,bufsz,params)
#define glGetnUniformuiv(prog,loc,bufsz,params) notavailable(L,prog,loc,bufsz,params)
#define glReadnPixels(x,y,width,height,format,type,bufsz,data)  \
            notavailable(L,x,y,width,height,format,type,bufsz,data)
#define glTextureBarrier()  notavailable(L)

#endif /* GL_VERSION_4_5 */

/*------------------------------------------------------------------------------*
 | GL_VERSION_4_5                                                               |
 *------------------------------------------------------------------------------*/

#ifndef GL_VERSION_4_6

#define GL_SHADER_BINARY_FORMAT_SPIR_V    0x9551
#define GL_SPIR_V_BINARY                  0x9552
#define GL_PARAMETER_BUFFER               0x80EE
#define GL_PARAMETER_BUFFER_BINDING       0x80EF
#define GL_CONTEXT_FLAG_NO_ERROR_BIT      0x00000008
#define GL_VERTICES_SUBMITTED             0x82EE
#define GL_PRIMITIVES_SUBMITTED           0x82EF
#define GL_VERTEX_SHADER_INVOCATIONS      0x82F0
#define GL_TESS_CONTROL_SHADER_PATCHES    0x82F1
#define GL_TESS_EVALUATION_SHADER_INVOCATIONS 0x82F2
#define GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED 0x82F3
#define GL_FRAGMENT_SHADER_INVOCATIONS    0x82F4
#define GL_COMPUTE_SHADER_INVOCATIONS     0x82F5
#define GL_CLIPPING_INPUT_PRIMITIVES      0x82F6
#define GL_CLIPPING_OUTPUT_PRIMITIVES     0x82F7
#define GL_POLYGON_OFFSET_CLAMP           0x8E1B
#define GL_SPIR_V_EXTENSIONS              0x9553
#define GL_NUM_SPIR_V_EXTENSIONS          0x9554
#define GL_TEXTURE_MAX_ANISOTROPY         0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY     0x84FF
#define GL_TRANSFORM_FEEDBACK_OVERFLOW    0x82EC
#define GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW 0x82ED

#define glSpecializeShader(shader,pEntryPoint,numSpecializationConstants,pConstantIndex,pConstantValue) \
    notavailable(L,shader,pEntryPoint,numSpecializationConstants,pConstantIndex,pConstantValue)
#define glMultiDrawArraysIndirectCount(mode,indirect,drawcount,maxdrawcount,stride) \
    notavailable(L,mode,indirect,drawcount,maxdrawcount,stride)
#define glMultiDrawElementsIndirectCount(mode,type,indirect,drawcount,maxdrawcount,stride)  \
    notavailable(L,mode,type,indirect,drawcount,maxdrawcount,stride)
#define glPolygonOffsetClamp(factor,units,clamp)    \
    notavailable(L,factor,units,clamp)

#endif /* GL_VERSION_4_6 */


/*@@ MISSING DEFINES on my machine: */
#ifndef GL_COPY_READ_BUFFER_BINDING
#define GL_COPY_READ_BUFFER_BINDING       0x8F36
#endif
#ifndef GL_COPY_WRITE_BUFFER_BINDING
#define GL_COPY_WRITE_BUFFER_BINDING      0x8F37
#endif
#ifndef GL_TEXTURE_BUFFER_BINDING
#define GL_TEXTURE_BUFFER_BINDING         0x8C2A
#endif
#ifndef GL_TRANSFORM_FEEDBACK_PAUSED
#define GL_TRANSFORM_FEEDBACK_PAUSED      0x8E23
#endif
#ifndef GL_TRANSFORM_FEEDBACK_ACTIVE
#define GL_TRANSFORM_FEEDBACK_ACTIVE      0x8E24
#endif
#ifndef GL_CLIP_DISTANCE6
#define GL_CLIP_DISTANCE6                 0x3006
#endif
#ifndef GL_CLIP_DISTANCE7
#define GL_CLIP_DISTANCE7                 0x3007
#endif
#ifndef GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#endif

#endif /*  wranglerDEFINED */
