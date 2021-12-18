-- The MIT License (MIT)
--
-- Copyright (c) 2016 Stefano Trettel
--
-- Software repository: MoonGL, https://github.com/stetre/moongl
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
-- 

-- *********************************************************************
-- DO NOT require() THIS MODULE (it is loaded automatically by MoonGL)
-- *********************************************************************

do
local gl = moongl -- require("moongl")

local function shader_sourcef(id, filename)
   local file, errmsg = io.open(filename, "r")
   if not file then error(errmsg, 3) end
   local s = file:read("a")
   file:close()
   gl.shader_source(id, s)
end

local function make_program_(shader_source_func, ...)
-- Creates a program with shaders.
--   make_program(type1, source1 [type2, source2,...] )
--   make_program({type1=source1, type2=source2,...})
--   --> prog, shader1, shader2, ...
   local arg = {...}
   if type(...) == 'table' then
      arg = {}
      for k,v in pairs(...) do arg[#arg+1]=k arg[#arg+1]=v end
   end
   local prog = gl.create_program()
   local shader = { }
   local id
   for i = 1, #arg, 2 do
      id = gl.create_shader(arg[i])
      shader_source_func(id, arg[i+1])
      gl.compile_shader(id, true)
      gl.attach_shader(prog, id)
      shader[#shader + 1] = id
   end
   gl.link_program(prog, true)
   return prog, table.unpack(shader)
end

local function make_program_s(...) -- gets shader sources from strings
   return make_program_(gl.shader_source, ...)
end

local function make_program(...) -- gets shader sources from files
   return make_program_(shader_sourcef, ...)
end

local function clean_program(...)
   local arg = {...}
   local prog = arg[1]
   gl.use_program(0)
   for i = 2, #arg do
      gl.detach_shader(prog, arg[i])
      gl.delete_shader(arg[i])
   end
   gl.delete_program(prog)
end

gl.make_program_s = make_program_s
gl.make_program = make_program
gl.clean_program = clean_program

end
