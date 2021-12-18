#!/usr/bin/env lua
local glfw = require("moonglfw")
local gl = require("moongl")

local function printf(...) io.write(string.format(...)) end

local TITLE = "Chapter 2 - Prints active attributes"
local W, H = 500, 500

-- GLFW/GL initializations ----------------------------------------------------
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('resizable', false)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   end
end)

glfw.set_window_size_callback(window, function(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
end)

-- Compile shader program -----------------------------------------------------

local prog, vsh, fsh = gl.make_program('vertex', "shaders/basic.vert", 'fragment', "shaders/basic.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Print active attributes ----------------------------------------------------

-- Note: glGetProgramResource requires OpenGL >=4.3
local num_attribs = gl.get_program_interface(prog, 'program input', 'active resources')
for i = 0, num_attribs-1 do
	local name = gl.get_program_resource_name(prog, 'program input', i)
	local rtype = gl.get_program_resource(prog, 'program input', i, 'type')
	local location = gl.get_program_resource(prog, 'program input', i, 'location')
   printf("%-5d %s (%s)\n", location, name, rtype)
end

-- Init scene -----------------------------------------------------------------

local position_data = {
   -0.8, -0.8, 0.0,
    0.8, -0.8, 0.0,
    0.0,  0.8, 0.0
}
local color_data = {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
}

-- Create and populate the buffer objects
local position_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', position_data), 'static draw')
local color_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', color_data), 'static draw')

-- Create and set-up the vertex array object
local vao = gl.new_vertex_array()
gl.enable_vertex_attrib_array(0)  -- vertex position
gl.enable_vertex_attrib_array(1)  -- vertex color
gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', color_vbo)
gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)

gl.bind_vertex_buffer(0, position_vbo, 0, gl.sizeof('float')*3)
gl.bind_vertex_buffer(1, color_vbo, 0, gl.sizeof('float')*3)

gl.vertex_attrib_format(0, 3, 'float', false, 0)
gl.vertex_attrib_binding(0, 0)
gl.vertex_attrib_format(1, 3, 'float', false, 0)
gl.vertex_attrib_binding(1, 1)

gl.unbind_vertex_array() -- same as gl.bind_vertex_array(0)

-- Event loop -----------------------------------------------------------------

gl.clear_color(0.5, 0.5, 0.5, 1.0)

while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Render
   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end

