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

BITFIELD_STRINGS(StagesCodesStrings) = {
    "all",
    "tess control",
    "tess evaluation", 
    "vertex",
    "geometry", 
    "fragment",
    "compute",
    NULL
};
BITFIELD_CODES(StagesCodesCodes) = {
    GL_ALL_SHADER_BITS,
    GL_TESS_CONTROL_SHADER_BIT,
    GL_TESS_EVALUATION_SHADER_BIT, 
    GL_VERTEX_SHADER_BIT,
    GL_GEOMETRY_SHADER_BIT, 
    GL_FRAGMENT_SHADER_BIT,
    GL_COMPUTE_SHADER_BIT,
};
BITFIELD_T(StagesCodesBitfield, StagesCodesStrings, StagesCodesCodes)
#define CheckStagesCodes(L, arg, mand) bitfieldCheck((L), (arg), (mand), &StagesCodesBitfield)
#define PushStagesCodes(L, code) bitfieldPush((L), (code), &StagesCodesBitfield)



GEN_FUNC(ProgramPipeline, OTYPE_PROGRAM_PIPELINE)
NEW_FUNC(ProgramPipeline, OTYPE_PROGRAM_PIPELINE)
BIND_FUNC(ProgramPipeline)
DELETE_FUNC(ProgramPipeline, OTYPE_PROGRAM_PIPELINE)
IS_FUNC(ProgramPipeline)
CREATE_FUNC(ProgramPipeline, OTYPE_PROGRAM_PIPELINE)

UINT_FUNC(ValidateProgramPipeline)

UINT2_FUNC(ActiveShaderProgram)

static int UseProgramStages(lua_State *L)
    {
    GLuint pipeline = luaL_checkinteger(L, 1);
    GLuint program = luaL_checkinteger(L, 2);
    GLbitfield stages = CheckStagesCodes(L, 3, 1);
    glUseProgramStages(pipeline, stages, program);
    CheckError(L);
    return 0;
    }

static int GetProgramPipelineInfoLog(lua_State *L)
    {
    char *buf;
    GLint bufsz;
    GLuint name = luaL_checkinteger(L, 1);

    glGetProgramPipelineiv(name, GL_INFO_LOG_LENGTH, &bufsz);
    CheckError(L);
    if(bufsz==0) return 0;
    buf = (char*)Malloc(L, bufsz * sizeof(char));
    glGetProgramPipelineInfoLog(name, bufsz, NULL, buf);
    CheckErrorFree(L, buf);
    lua_pushstring(L, buf);
    Free(L, buf);
    return 1;
    }

static int GetInteger(lua_State *L, GLuint pipeline, GLenum pname)
    {
    GLint params; 
    glGetProgramPipelineiv(pipeline, pname, &params);
    CheckError(L);
    lua_pushinteger(L, params); 
    return 1;
    }


static int GetProgramPipeline(lua_State *L)
    {
#define INTEGER return GetInteger(L, pipeline, pname);
    GLuint pipeline = luaL_checkinteger(L, 1);
    GLenum pname = checkpipelinepname(L, 2);
    switch(pname)
        {
        case GL_ACTIVE_PROGRAM: INTEGER
        case GL_VALIDATE_STATUS: INTEGER
        case GL_VERTEX_SHADER: INTEGER
        case GL_GEOMETRY_SHADER: INTEGER
        case GL_FRAGMENT_SHADER: INTEGER
        case GL_COMPUTE_SHADER: INTEGER
        case GL_TESS_CONTROL_SHADER: INTEGER
        case GL_TESS_EVALUATION_SHADER: INTEGER
        case GL_INFO_LOG_LENGTH: INTEGER
        default: return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
#undef INTEGER
    }


static const struct luaL_Reg Functions[] = 
    {
        { "new_program_pipeline", NewProgramPipeline },
        { "gen_program_pipelines", GenProgramPipelines },
        { "bind_program_pipeline", BindProgramPipeline },
        { "create_program_pipelines", CreateProgramPipelines },
        { "delete_program_pipelines", DeleteProgramPipelines },
        { "is_program_pipeline", IsProgramPipeline },
        { "validate_program_pipeline", ValidateProgramPipeline },
        { "active_shader_program", ActiveShaderProgram },
        { "use_program_stages", UseProgramStages },
        { "get_program_pipeline", GetProgramPipeline },
        { "get_program_pipeline_info_log", GetProgramPipelineInfoLog },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_pipeline(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

