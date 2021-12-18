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

local gl = moongl -- require("moongl")

--[[
-- To make error messages point to the correct culprit we could use the following
-- solution. However, since this adds a performance penalty and since the needed
-- information (i.e. the line number of the invoking script) is already in the stack
-- traceback, we won't use it.

local function ppcall(f, ...)
   local ok, errmsg = pcall(f, ...)
   if not ok then error(errmsg, 3) end
end

gl.uniformf = function(loc, ...) ppcall(gl.uniform, loc, 'float', ...) end
--]]

-- Wrappers for uniform variables:

do

local flatten = gl.flatten
local uniform = gl.uniform
local uniformv = gl.uniformv
local program_uniform = gl.program_uniform
local program_uniformv = gl.program_uniformv
local uniform_matrix = gl.uniform_matrix
local uniform_matrixv = gl.uniform_matrixv
local program_uniform_matrix = gl.program_uniform_matrix
local program_uniform_matrixv = gl.program_uniform_matrixv
local get_uniform = gl.get_uniform
local vertex_attrib = gl.vertex_attrib
local vertex_attrib_n = gl.vertex_attrib_n
local pack = gl.pack
local unpack = gl.unpack
gl.uniformb = function(loc, ...) uniform(loc, 'bool', flatten({...})) end
gl.uniformi = function(loc, ...) uniform(loc, 'int', flatten({...})) end
gl.uniformui = function(loc, ...) uniform(loc, 'uint', flatten({...})) end
gl.uniformf = function(loc, ...) uniform(loc, 'float', flatten({...})) end
gl.uniformd = function(loc, ...) uniform(loc, 'double', flatten({...})) end

gl.uniformbv = function(loc, cnt, ...) uniformv(loc, cnt, 'bool', flatten({...})) end
gl.uniformiv = function(loc, cnt, ...) uniformv(loc, cnt, 'int', flatten({...})) end
gl.uniformuiv = function(loc, cnt, ...) uniformv(loc, cnt, 'uint', flatten({...})) end
gl.uniformfv = function(loc, cnt, ...) uniformv(loc, cnt, 'float', flatten({...})) end
gl.uniformdv = function(loc, cnt, ...) uniformv(loc, cnt, 'double', flatten({...})) end

gl.program_uniformb = function(prog, loc, ...) program_uniform(prog, loc, 'bool', flatten({...})) end
gl.program_uniformi = function(prog, loc, ...) program_uniform(prog, loc, 'int', flatten({...})) end
gl.program_uniformui = function(prog, loc, ...) program_uniform(prog, loc, 'uint', flatten({...})) end
gl.program_uniformf = function(prog, loc, ...) program_uniform(prog, loc, 'float', flatten({...})) end
gl.program_uniformd = function(prog, loc, ...) program_uniform(prog, loc, 'double', flatten({...})) end

gl.program_uniformbv = function(prog, loc, cnt, ...) program_uniformv(prog, loc, cnt, 'bool', flatten({...})) end
gl.program_uniformiv = function(prog, loc, cnt, ...) program_uniformv(prog, loc, cnt, 'int', flatten({...})) end
gl.program_uniformuiv = function(prog, loc, cnt, ...) program_uniformv(prog, loc, cnt, 'uint', flatten({...})) end
gl.program_uniformfv = function(prog, loc, cnt, ...) program_uniformv(prog, loc, cnt, 'float', flatten({...})) end
gl.program_uniformdv = function(prog, loc, cnt, ...) program_uniformv(prog, loc, cnt, 'double', flatten({...})) end

gl.uniform_matrix2b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '2x2', tr, flatten({...})) end
gl.uniform_matrix3b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '3x3', tr, flatten({...})) end
gl.uniform_matrix4b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2b = gl.uniform_matrix2b 
gl.uniform_matrix3x3b = gl.uniform_matrix3b 
gl.uniform_matrix4x4b = gl.uniform_matrix4b 
gl.uniform_matrix2x3b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3b = function(loc, tr, ...) uniform_matrix(loc, 'bool', '4x3', tr, flatten({...})) end

gl.uniform_matrix2i = function(loc, tr, ...) uniform_matrix(loc, 'int', '2x2', tr, flatten({...})) end
gl.uniform_matrix3i = function(loc, tr, ...) uniform_matrix(loc, 'int', '3x3', tr, flatten({...})) end
gl.uniform_matrix4i = function(loc, tr, ...) uniform_matrix(loc, 'int', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2i = gl.uniform_matrix2i 
gl.uniform_matrix3x3i = gl.uniform_matrix3i 
gl.uniform_matrix4x4i = gl.uniform_matrix4i 
gl.uniform_matrix2x3i = function(loc, tr, ...) uniform_matrix(loc, 'int', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2i = function(loc, tr, ...) uniform_matrix(loc, 'int', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4i = function(loc, tr, ...) uniform_matrix(loc, 'int', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2i = function(loc, tr, ...) uniform_matrix(loc, 'int', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4i = function(loc, tr, ...) uniform_matrix(loc, 'int', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3i = function(loc, tr, ...) uniform_matrix(loc, 'int', '4x3', tr, flatten({...})) end

gl.uniform_matrix2ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '2x2', tr, flatten({...})) end
gl.uniform_matrix3ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '3x3', tr, flatten({...})) end
gl.uniform_matrix4ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2ui = gl.uniform_matrix2ui 
gl.uniform_matrix3x3ui = gl.uniform_matrix3ui 
gl.uniform_matrix4x4ui = gl.uniform_matrix4ui 
gl.uniform_matrix2x3ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3ui = function(loc, tr, ...) uniform_matrix(loc, 'uint', '4x3', tr, flatten({...})) end

gl.uniform_matrix2f = function(loc, tr, ...) uniform_matrix(loc, 'float', '2x2', tr, flatten({...})) end
gl.uniform_matrix3f = function(loc, tr, ...) uniform_matrix(loc, 'float', '3x3', tr, flatten({...})) end
gl.uniform_matrix4f = function(loc, tr, ...) uniform_matrix(loc, 'float', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2f = gl.uniform_matrix2f 
gl.uniform_matrix3x3f = gl.uniform_matrix3f 
gl.uniform_matrix4x4f = gl.uniform_matrix4f 
gl.uniform_matrix2x3f = function(loc, tr, ...) uniform_matrix(loc, 'float', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2f = function(loc, tr, ...) uniform_matrix(loc, 'float', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4f = function(loc, tr, ...) uniform_matrix(loc, 'float', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2f = function(loc, tr, ...) uniform_matrix(loc, 'float', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4f = function(loc, tr, ...) uniform_matrix(loc, 'float', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3f = function(loc, tr, ...) uniform_matrix(loc, 'float', '4x3', tr, flatten({...})) end

gl.uniform_matrix2d = function(loc, tr, ...) uniform_matrix(loc, 'double', '2x2', tr, flatten({...})) end
gl.uniform_matrix3d = function(loc, tr, ...) uniform_matrix(loc, 'double', '3x3', tr, flatten({...})) end
gl.uniform_matrix4d = function(loc, tr, ...) uniform_matrix(loc, 'double', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2d = gl.uniform_matrix2d 
gl.uniform_matrix3x3d = gl.uniform_matrix3d 
gl.uniform_matrix4x4d = gl.uniform_matrix4d 
gl.uniform_matrix2x3d = function(loc, tr, ...) uniform_matrix(loc, 'double', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2d = function(loc, tr, ...) uniform_matrix(loc, 'double', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4d = function(loc, tr, ...) uniform_matrix(loc, 'double', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2d = function(loc, tr, ...) uniform_matrix(loc, 'double', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4d = function(loc, tr, ...) uniform_matrix(loc, 'double', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3d = function(loc, tr, ...) uniform_matrix(loc, 'double', '4x3', tr, flatten({...})) end

gl.uniform_matrix2bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '2x2', tr, flatten({...})) end
gl.uniform_matrix3bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '3x3', tr, flatten({...})) end
gl.uniform_matrix4bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2bv = gl.uniform_matrix2bv 
gl.uniform_matrix3x3bv = gl.uniform_matrix3bv 
gl.uniform_matrix4x4bv = gl.uniform_matrix4bv 
gl.uniform_matrix2x3bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3bv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'bool', '4x3', tr, flatten({...})) end

gl.uniform_matrix2iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '2x2', tr, flatten({...})) end
gl.uniform_matrix3iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '3x3', tr, flatten({...})) end
gl.uniform_matrix4iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2iv = gl.uniform_matrix2iv 
gl.uniform_matrix3x3iv = gl.uniform_matrix3iv 
gl.uniform_matrix4x4iv = gl.uniform_matrix4iv 
gl.uniform_matrix2x3iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3iv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'int', '4x3', tr, flatten({...})) end


gl.uniform_matrix2uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '2x2', tr, flatten({...})) end
gl.uniform_matrix3uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '3x3', tr, flatten({...})) end
gl.uniform_matrix4uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2uiv = gl.uniform_matrix2uiv 
gl.uniform_matrix3x3uiv = gl.uniform_matrix3uiv 
gl.uniform_matrix4x4uiv = gl.uniform_matrix4uiv 
gl.uniform_matrix2x3uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3uiv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'uint', '4x3', tr, flatten({...})) end

gl.uniform_matrix2fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '2x2', tr, flatten({...})) end
gl.uniform_matrix3fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '3x3', tr, flatten({...})) end
gl.uniform_matrix4fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2fv = gl.uniform_matrix2fv 
gl.uniform_matrix3x3fv = gl.uniform_matrix3fv 
gl.uniform_matrix4x4fv = gl.uniform_matrix4fv 
gl.uniform_matrix2x3fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3fv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'float', '4x3', tr, flatten({...})) end

gl.uniform_matrix2dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '2x2', tr, flatten({...})) end
gl.uniform_matrix3dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '3x3', tr, flatten({...})) end
gl.uniform_matrix4dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '4x4', tr, flatten({...})) end
gl.uniform_matrix2x2dv = gl.uniform_matrix2dv 
gl.uniform_matrix3x3dv = gl.uniform_matrix3dv 
gl.uniform_matrix4x4dv = gl.uniform_matrix4dv 
gl.uniform_matrix2x3dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '2x3', tr, flatten({...})) end
gl.uniform_matrix3x2dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '3x2', tr, flatten({...})) end
gl.uniform_matrix2x4dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '2x4', tr, flatten({...})) end
gl.uniform_matrix4x2dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '4x2', tr, flatten({...})) end
gl.uniform_matrix3x4dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '3x4', tr, flatten({...})) end
gl.uniform_matrix4x3dv = function(loc, cnt, tr, ...) uniform_matrixv(loc, cnt, 'double', '4x3', tr, flatten({...})) end

gl.program_uniform_matrix2b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2b = gl.program_uniform_matrix2b 
gl.program_uniform_matrix3x3b = gl.program_uniform_matrix3b 
gl.program_uniform_matrix4x4b = gl.program_uniform_matrix4b 
gl.program_uniform_matrix2x3b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3b = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'bool', '4x3', tr, flatten({...})) end

gl.program_uniform_matrix2i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2i = gl.program_uniform_matrix2i 
gl.program_uniform_matrix3x3i = gl.program_uniform_matrix3i 
gl.program_uniform_matrix4x4i = gl.program_uniform_matrix4i 
gl.program_uniform_matrix2x3i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3i = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'int', '4x3', tr, flatten({...})) end


gl.program_uniform_matrix2ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2ui = gl.program_uniform_matrix2ui 
gl.program_uniform_matrix3x3ui = gl.program_uniform_matrix3ui 
gl.program_uniform_matrix4x4ui = gl.program_uniform_matrix4ui 
gl.program_uniform_matrix2x3ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3ui = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'uint', '4x3', tr, flatten({...})) end

gl.program_uniform_matrix2f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2f = gl.program_uniform_matrix2f 
gl.program_uniform_matrix3x3f = gl.program_uniform_matrix3f 
gl.program_uniform_matrix4x4f = gl.program_uniform_matrix4f 
gl.program_uniform_matrix2x3f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3f = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'float', '4x3', tr, flatten({...})) end


gl.program_uniform_matrix2d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2d = gl.program_uniform_matrix2d 
gl.program_uniform_matrix3x3d = gl.program_uniform_matrix3d 
gl.program_uniform_matrix4x4d = gl.program_uniform_matrix4d 
gl.program_uniform_matrix2x3d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3d = function(prog, loc, tr, ...) program_uniform_matrix(prog, loc, 'double', '4x3', tr, flatten({...})) end


gl.program_uniform_matrix2bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2bv = gl.program_uniform_matrix2bv 
gl.program_uniform_matrix3x3bv = gl.program_uniform_matrix3bv 
gl.program_uniform_matrix4x4bv = gl.program_uniform_matrix4bv 
gl.program_uniform_matrix2x3bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3bv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'bool', '4x3', tr, flatten({...})) end


gl.program_uniform_matrix2iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2iv = gl.program_uniform_matrix2iv 
gl.program_uniform_matrix3x3iv = gl.program_uniform_matrix3iv 
gl.program_uniform_matrix4x4iv = gl.program_uniform_matrix4iv 
gl.program_uniform_matrix2x3iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3iv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'int', '4x3', tr, flatten({...})) end


gl.program_uniform_matrix2uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2uiv = gl.program_uniform_matrix2uiv 
gl.program_uniform_matrix3x3uiv = gl.program_uniform_matrix3uiv 
gl.program_uniform_matrix4x4uiv = gl.program_uniform_matrix4uiv 
gl.program_uniform_matrix2x3uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3uiv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'uint', '4x3', tr, flatten({...})) end

gl.program_uniform_matrix2fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2fv = gl.program_uniform_matrix2fv 
gl.program_uniform_matrix3x3fv = gl.program_uniform_matrix3fv 
gl.program_uniform_matrix4x4fv = gl.program_uniform_matrix4fv 
gl.program_uniform_matrix2x3fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3fv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'float', '4x3', tr, flatten({...})) end

gl.program_uniform_matrix2dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '2x2', tr, flatten({...})) end
gl.program_uniform_matrix3dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '3x3', tr, flatten({...})) end
gl.program_uniform_matrix4dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '4x4', tr, flatten({...})) end
gl.program_uniform_matrix2x2dv = gl.program_uniform_matrix2dv 
gl.program_uniform_matrix3x3dv = gl.program_uniform_matrix3dv 
gl.program_uniform_matrix4x4dv = gl.program_uniform_matrix4dv 
gl.program_uniform_matrix2x3dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '2x3', tr, flatten({...})) end
gl.program_uniform_matrix3x2dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '3x2', tr, flatten({...})) end
gl.program_uniform_matrix2x4dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '2x4', tr, flatten({...})) end
gl.program_uniform_matrix4x2dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '4x2', tr, flatten({...})) end
gl.program_uniform_matrix3x4dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '3x4', tr, flatten({...})) end
gl.program_uniform_matrix4x3dv = function(prog, loc, cnt, tr, ...) program_uniform_matrixv(prog, loc, cnt, 'double', '4x3', tr, flatten({...})) end

gl.get_uniformb = function(prog, loc, sz) return get_uniform(prog, loc, 'bool', sz) end
gl.get_uniformi = function(prog, loc, sz) return get_uniform(prog, loc, 'int', sz) end
gl.get_uniformui = function(prog, loc, sz) return get_uniform(prog, loc, 'uint', sz) end
gl.get_uniformf = function(prog, loc, sz) return get_uniform(prog, loc, 'float', sz) end
gl.get_uniformd = function(prog, loc, sz) return get_uniform(prog, loc, 'double', sz) end

gl.vertex_attribb = function(i, ...) vertex_attrib(i, 'byte', flatten({...})) end
gl.vertex_attribub = function(i, ...) vertex_attrib(i, 'ubyte', flatten({...})) end
gl.vertex_attribs = function(i, ...) vertex_attrib(i, 'short', flatten({...})) end
gl.vertex_attribus = function(i, ...) vertex_attrib(i, 'ushort', flatten({...})) end
gl.vertex_attribi = function(i, ...) vertex_attrib(i, 'int', flatten({...})) end
gl.vertex_attribui = function(i, ...) vertex_attrib(i, 'uint', flatten({...})) end
gl.vertex_attribf = function(i, ...) vertex_attrib(i, 'float', flatten({...})) end
gl.vertex_attribd = function(i, ...) vertex_attrib(i, 'double', flatten({...})) end

gl.vertex_attrib_nb = function(i, ...) vertex_attrib_n(i, 'byte', flatten({...})) end
gl.vertex_attrib_nub = function(i, ...) vertex_attrib_n(i, 'ubyte', flatten({...})) end
gl.vertex_attrib_ns = function(i, ...) vertex_attrib_n(i, 'short', flatten({...})) end
gl.vertex_attrib_nus = function(i, ...) vertex_attrib_n(i, 'ushort', flatten({...})) end
gl.vertex_attrib_ni = function(i, ...) vertex_attrib_n(i, 'int', flatten({...})) end
gl.vertex_attrib_nui = function(i, ...) vertex_attrib_n(i, 'uint', flatten({...})) end

-- Wrappers for pack() and unpack():

gl.packb = function(...) return pack('byte', ...) end
gl.packub = function(...) return pack('ubyte', ...) end
gl.packs = function(...) return pack('short', ...) end
gl.packus = function(...) return pack('ushort', ...) end
gl.packi = function(...) return pack('int', ...) end
gl.packui = function(...) return pack('uint', ...) end
gl.packf = function(...) return pack('float', ...) end
gl.packd = function(...) return pack('double', ...) end

gl.unpackb = function(...) return unpack('byte', ...) end
gl.unpackub = function(...) return unpack('ubyte', ...) end
gl.unpacks = function(...) return unpack('short', ...) end
gl.unpackus = function(...) return unpack('ushort', ...) end
gl.unpacki = function(...) return unpack('int', ...) end
gl.unpackui = function(...) return unpack('uint', ...) end
gl.unpackf = function(...) return unpack('float', ...) end
gl.unpackd = function(...) return unpack('double', ...) end

end
