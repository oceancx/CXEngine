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

#define rec_t struct rec_s
struct rec_s {
    RB_ENTRY(rec_s) Entry;
    /* (otype, oname) = search key */
    uint32_t otype; /* object type */
    uint32_t oname;   /* object name (assigned by OpenGL) */
};

/* compare function */
static int cmp_code(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->otype != rec2->otype)
        return (rec1->otype < rec2->otype ? -1 : rec1->otype > rec2->otype);
    return (rec1->oname < rec2->oname ? -1 : rec1->oname > rec2->oname);
    } 

static RB_HEAD(ObjTree, rec_s) ObjHead = RB_INITIALIZER(&ObjHead);
RB_PROTOTYPE_STATIC(ObjTree, rec_s, Entry, cmp_code) 
RB_GENERATE_STATIC(ObjTree, rec_s, Entry, cmp_code) 

static rec_t *removeeee(rec_t *rec) 
    { return RB_REMOVE(ObjTree, &ObjHead, rec); }
static rec_t *insert(rec_t *rec) 
    { return RB_INSERT(ObjTree, &ObjHead, rec); }
static rec_t *search(uint32_t otype, uint32_t oname) 
    { rec_t tmp; tmp.otype = otype; tmp.oname = oname; return RB_FIND(ObjTree, &ObjHead, &tmp); }
static rec_t *first(uint32_t otype, uint32_t oname) 
    { rec_t tmp; tmp.otype = otype; tmp.oname = oname; return RB_NFIND(ObjTree, &ObjHead, &tmp); }
#if 0
static rec_t *next(rec_t *rec)
    { return RB_NEXT(ObjTree, &ObjHead, rec); }
static rec_t *prev(rec_t *rec)
    { return RB_PREV(ObjTree, &ObjHead, rec); }
static rec_t *min(void)
    { return RB_MIN(ObjTree, &ObjHead); }
static rec_t *max(void)
    { return RB_MAX(ObjTree, &ObjHead); }
static rec_t *root(void)
    { return RB_ROOT(&ObjHead); }
#endif
 
static int trace_objects = 0;
static int track_objects = 1;

static char *tracename(uint32_t otype)
    {
    switch(otype)
        {
        case OTYPE_BUFFER: return "moongl_buffer";
        case OTYPE_VERTEX_ARRAY: return "moongl_vertex_array";
        case OTYPE_SAMPLER: return "moongl_sampler";
        case OTYPE_TEXTURE: return "moongl_texture";
        case OTYPE_FRAMEBUFFER: return "moongl_framebuffer";
        case OTYPE_RENDERBUFFER: return "moongl_renderbuffer";
        case OTYPE_PROGRAM: return "moongl_program";
        case OTYPE_SHADER: return "moongl_shader";
        case OTYPE_PROGRAM_PIPELINE: return "moongl_program_pipeline";
        case OTYPE_TRANSFORM_FEEDBACK: return "moongl_transform_feedback";
        case OTYPE_QUERY: return "moongl_query";
        default:
            break;
        }
    return "???";
    }

int object_new(lua_State *L, uint32_t otype, uint32_t oname)
    {
    rec_t *rec;
    if(trace_objects)
        printf("create %s %d\n", tracename(otype), oname);

    if(track_objects==0)
        return 0;
    if(search(otype, oname))
        {
        /* duplicate value: this should mean that we are working with
         * multiple concurrent GL contexts, so we stop tracking objects
         * (we wouldn't be able to delete them at exit anyway, since
         * we have no control on the current context).
         */
        track_objects = 0;
        return 0; // unexpected(L);
        }

    if((rec = (rec_t*)Malloc(L, sizeof(rec_t))) == NULL) 
        return luaL_error(L, errstring(ERR_MEMORY));

    memset(rec, 0, sizeof(rec_t));
    rec->otype = otype;
    rec->oname = oname;
    insert(rec);

    return 0;
    }

void object_free(lua_State *L, uint32_t otype, uint32_t oname)
    {
    rec_t *rec;
    GLuint name = oname;    
    if(trace_objects)
        printf("delete %s %d\n", tracename(otype), oname);
    if(track_objects)
        {
        rec = search(otype, oname);
        if(rec)
            { removeeee(rec); Free(L, rec); }
        }

    switch(otype)
        {
        case OTYPE_BUFFER: glDeleteBuffers(1, &name); break;
        case OTYPE_VERTEX_ARRAY: glDeleteVertexArrays(1, &name); break;
        case OTYPE_SAMPLER: glDeleteSamplers(1, &name); break;
        case OTYPE_TEXTURE: glDeleteTextures(1, &name); break;
        case OTYPE_FRAMEBUFFER: glDeleteFramebuffers(1, &name); break;
        case OTYPE_RENDERBUFFER: glDeleteRenderbuffers(1, &name); break;
        case OTYPE_PROGRAM: glDeleteProgram(name); break;
        case OTYPE_SHADER: glDeleteShader(name); break;
        case OTYPE_PROGRAM_PIPELINE: glDeleteProgramPipelines(1, &name); break;
        case OTYPE_TRANSFORM_FEEDBACK: glDeleteTransformFeedbacks(1, &name); break;
        case OTYPE_QUERY: glDeleteQueries(1, &name); break;
        default:
            unexpected(L);
        }
    CheckError(L);
    }

void object_free_all(lua_State *L)
    {
    rec_t *rec;
    if(track_objects)
        {
        while((rec = first(0, 0)))
            object_free(L, rec->otype, rec->oname);
        }
    else /* just remove any object inserted before object tracking was disabled */
        {
        while((rec = first(0, 0)))
            { removeeee(rec); Free(L, rec); }
        }
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/


static int TraceObjects(lua_State *L)
    {
    trace_objects = checkboolean(L, 1);
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "trace_objects", TraceObjects },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_object(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

