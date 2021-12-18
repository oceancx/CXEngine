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
 | sync database                                                                |
 *------------------------------------------------------------------------------*/

#include "tree.h"

#define node_t       moongl_node_t
#define node_s       moongl_node_s

#define moongl_node_t struct moongl_node_s
struct moongl_node_s {
    RB_ENTRY(moongl_node_s) entry;
    int id;     /* search key */
    GLsync  sync;
};

static node_t *node_search(int id); 
static int IdNew(void)
/* returns a free id or -1 if none are found (unlikely) */
    {
    static int idfree = 0;
    int guard = idfree-1;
    if((idfree==-1)||(idfree==0)||(idfree==1))
        { idfree=1; guard=-1; }
    while(1)
        {
        if(idfree==guard) /* we made a complete round */
            return -1;
        if(idfree==-1) idfree=1;
        if(node_search(idfree)==NULL) return idfree++;
        idfree++;
        }
    return -1;
    }

static int cmp(node_t *node1, node_t *node2) /* the compare function */
    { return (node1->id < node2->id ? -1 : node1->id > node2->id); } 

static RB_HEAD(nodetree_s, node_s) Head = RB_INITIALIZER(&Head);

RB_PROTOTYPE_STATIC(nodetree_s, node_s, entry, cmp) 
RB_GENERATE_STATIC(nodetree_s, node_s, entry, cmp) 
 
static node_t *node_remove(node_t *node) 
    { return RB_REMOVE(nodetree_s, &Head, node); }
static node_t *node_insert(node_t *node) 
    { return RB_INSERT(nodetree_s, &Head, node); }
static node_t *node_search(int id) 
    { node_t tmp; tmp.id = id; return RB_FIND(nodetree_s, &Head, &tmp); }
#if 0
static node_t *node_first(int id) 
    { node_t tmp; tmp.id = id; return RB_NFIND(nodetree_s, &Head, &tmp); }
node_t *node_next(node_t *node)
    { return RB_NEXT(nodetree_s, &Head, node); }
node_t *node_prev(node_t *node)
    { return RB_PREV(nodetree_s, &Head, node); }
node_t *node_min(void)
    { return RB_MIN(nodetree_s, &Head); }
node_t *node_max(void)
    { return RB_MAX(nodetree_s, &Head); }
node_t *node_root(void)
    { return RB_ROOT(&Head); }
static void node_free_all(void)
    {
    node_t *node;
    while((node = node_first(0)))
        node_free(node);
    }
#endif

static node_t *node_new(GLsync sync)
    {
    node_t *node;
    int id =  IdNew(); /* allocate a free id */
    if(id == -1)
        return NULL;
    if((node = (node_t*)malloc(sizeof(node_t))) == NULL) return NULL;
    memset(node, 0, sizeof(node_t));
    node->id = id;
    node->sync = sync;
    node_insert(node);
    return node;
    }

static void node_free(node_t* node)
    {
    if(node_search(node->id) == node)
        {
        node_remove(node);
        }
    free(node);  
    }

//@@TODO sync_free_all

static node_t *CheckSync(lua_State *L, int arg)
    {
    int id = luaL_checkinteger(L, arg);
    node_t *node = node_search(id);
    if(node == NULL) 
        { luaL_argerror(L, arg, "not a sync"); return NULL; }
    return node;
    }

/*------------------------------------------------------------------------------*
 | Bindings                                                                     |
 *------------------------------------------------------------------------------*/

BITFIELD_STRINGS(FlagStrings) = {
    "flush commands",
    NULL
};
BITFIELD_CODES(FlagCodes) = {
    GL_SYNC_FLUSH_COMMANDS_BIT,
};
BITFIELD_T(FlagBitfield, FlagStrings, FlagCodes)
#define CheckFlag(L, arg, mand) bitfieldCheck((L), (arg), (mand), &FlagBitfield)
#define PushFlag(L, code) bitfieldPush((L), (code), &FlagBitfield)

static int FenceSync(lua_State *L)
    {
    node_t *node;
    GLsync sync; 
    GLbitfield flags = 0;
    GLenum condition = checkcondition(L, 1);
    sync = glFenceSync(condition, flags);
    CheckError(L);
    if(sync==0) /* paranoia... */
        return luaL_error(L, "cannot create sync");
    if((node=node_new(sync))==NULL)
        {
        glDeleteSync(sync);
        CheckError(L);
        return luaL_error(L, "cannot create sync");
        }
    lua_pushinteger(L, node->id);
    return 1;
    }


static int DeleteSync(lua_State *L)
    {
    node_t *node = CheckSync(L, 1);
    GLsync sync = node->sync;
    node_free(node);
    glDeleteSync(sync);
    CheckError(L);
    return 0;
    }

static int IsSync(lua_State *L)
    {
    int id = luaL_checkinteger(L, 1);
    node_t *node = node_search(id);
    if(node == NULL) 
        { lua_pushboolean(L, 0); return 1; }
    if(glIsSync(node->sync) == GL_FALSE)
        return luaL_error(L, UNEXPECTED_ERROR);
    CheckError(L);
    lua_pushboolean(L, 1);
    return 1;
    }


static int ClientWaitSync(lua_State *L)
/* client_wait_sync(sync, timeout [, flag...]) */
    {
    GLenum status;
    node_t *node = CheckSync(L, 1);
    GLuint64 timeout = luaL_checknumber(L, 2); /* nanoseconds */
    GLbitfield flags = CheckFlag(L, 3, 0);
    status = glClientWaitSync(node->sync, flags, timeout);
    CheckError(L);
    pushsyncstatus(L, status);
    return 1;
    }

static int WaitSync(lua_State *L)
/* wait_sync(sync) */
    {
    GLbitfield flags = 0;
    GLuint64 timeout = GL_TIMEOUT_IGNORED;
    node_t *node = CheckSync(L, 1);
    glWaitSync(node->sync, flags, timeout);
    CheckError(L);
    return 0;
    }


//void glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);

static int GetObjectType(lua_State *L, GLsync sync, GLenum pname)
    {
    GLsizei length = 0;
    GLint value;
    glGetSynciv(sync, pname, 1, &length, &value);
    CheckError(L);
    if(length==0)
        return luaL_error(L, UNEXPECTED_ERROR);
    switch(value)
        {
        case GL_SYNC_FENCE: lua_pushstring(L, "fence"); break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 1;
    }

static int GetSyncStatus(lua_State *L, GLsync sync, GLenum pname)
    {
    GLsizei length = 0;
    GLint value;
    glGetSynciv(sync, pname, 1, &length, &value);
    CheckError(L);
    if(length==0)
        return luaL_error(L, UNEXPECTED_ERROR);
    switch(value)
        {
        case GL_SIGNALED: lua_pushstring(L, "signaled"); break;
        case GL_UNSIGNALED: lua_pushstring(L, "unsignaled"); break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 1;
    }

static int GetSyncCondition(lua_State *L, GLsync sync, GLenum pname)
    {
    GLsizei length = 0;
    GLint value;
    glGetSynciv(sync, pname, 1, &length, &value);
    CheckError(L);
    if(length==0)
        return luaL_error(L, UNEXPECTED_ERROR);
    pushcondition(L, value);
    return 1;
    }

static int GetSyncFlags(lua_State *L, GLsync sync, GLenum pname)
    {
#if 0
    size_t i, n, nflags = sizeof(FlagsCodes)/sizeof(FlagsCodes[0]);
    GLsizei length = 0;
    GLint value;
    glGetSynciv(sync, pname, 1, &length, &value);
    CheckError(L);
    if(length==0)
        return luaL_error(L, UNEXPECTED_ERROR);
    n = 0;
    for(i = 0; i < nflags; i++)
        {
        if(value & FlagsCodes[i])
            { lua_pushstring(L, FlagsStrings[i]); n++; }
        }
    return n;
#endif
    /* no flags are currently supported
     * NOTE: this query refers to the flags passed to glFenceSync(),
     *       not to those passed to the wait functions.
     */
    (void)L;
    (void)sync;
    (void)pname;
    return 0;
    }

static int GetSync(lua_State *L)
    {
    node_t *node = CheckSync(L, 1);
    GLenum pname = checksyncpname(L, 2);
    switch(pname)
        {
        case GL_OBJECT_TYPE: return GetObjectType(L, node->sync, pname);
        case GL_SYNC_STATUS: return GetSyncStatus(L, node->sync, pname);
        case GL_SYNC_CONDITION: return GetSyncCondition(L, node->sync, pname);
        case GL_SYNC_FLAGS: return GetSyncFlags(L, node->sync, pname);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 0;
    }


static int RawSync(lua_State *L)
    {
    node_t *node = CheckSync(L, 1);
    lua_pushlightuserdata(L, (void*)&node->sync);
    return 1;
    }



static const struct luaL_Reg Functions[] = 
    {
        { "fence_sync", FenceSync },
        { "delete_sync", DeleteSync },
        { "is_sync", IsSync },
        { "client_wait_sync", ClientWaitSync },
        { "wait_sync", WaitSync },
        { "get_sync", GetSync },
        { "raw_sync", RawSync },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_sync(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

