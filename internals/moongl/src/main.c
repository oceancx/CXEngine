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

static lua_State *moongl_L = NULL;

static void AtExit(void)
    {
    if(moongl_L)
        {
        enums_free_all(moongl_L);
        object_free_all(moongl_L);
        moongl_L = NULL;
        }
    }

int luaopen_moongl(lua_State *L)
/* Lua calls this function to load the module */
    {
    moongl_L = L;
    atexit(AtExit);

    lua_newtable(L); /* the gl table */

    /* add gl functions: */
    moongl_open_init(L);
    moongl_open_object(L);
    moongl_open_enums(L);
    moongl_open_nongl(L);

    moongl_open_hint(L);
    moongl_open_capabilities(L);
    moongl_open_whole_framebuffer(L);
    moongl_open_buffer(L);
    moongl_open_vertex_array(L);
    moongl_open_getvertex(L);
    moongl_open_texture(L);
    moongl_open_teximage(L);
    moongl_open_shader(L);
    moongl_open_subroutine(L);
    moongl_open_program(L);
    moongl_open_proginterface(L);
    moongl_open_uniform(L);
    moongl_open_getuniform(L);
    moongl_open_pipeline(L);
    moongl_open_framebuffer(L);
    moongl_open_query(L);
    moongl_open_renderbuffer(L);
    moongl_open_sampler(L);
    moongl_open_draw(L);
    moongl_open_transform(L);
    moongl_open_raster(L);
    moongl_open_get(L);
    moongl_open_getstring(L);
    moongl_open_getintformat(L);
    moongl_open_perfragment(L);
    moongl_open_pixel(L);
    moongl_open_debug(L);
    moongl_open_sync(L);

    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moongl");
    //if(luaL_dostring(L, "require('moongl.make')") != 0) lua_error(L);
    //if(luaL_dostring(L, "require('moongl.nongl')") != 0) lua_error(L);
    //if(luaL_dostring(L, "require('moongl.wrappers')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moongl");

    return 1;
    }

