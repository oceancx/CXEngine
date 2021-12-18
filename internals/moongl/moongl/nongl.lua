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

-- Wrappers to bind_xxx() functions that unbind the current xxx object.
-- These are functionally redundant, but make the code somewhat clearer to read
-- (it is awkward to read a 'bind xxx' statement that actually means 'unbind xxx').
gl.unbind_buffer = function(target) gl.bind_buffer(target) end
gl.unbind_program_pipeline = function() gl.bind_program_pipeline() end
gl.unbind_texture = function(target) gl.bind_texture(target) end
gl.unbind_sampler = function(unit) gl.bind_sampler(unit) end
gl.unbind_framebuffer = function(target) gl.bind_framebuffer(target) end
gl.unbind_renderbuffer = function(target) gl.bind_renderbuffer(target) end
gl.unbind_vertex_array = function() gl.bind_vertex_array() end
gl.unbind_transform_feedback = function(target) gl.bind_transform_feedback(target) end

