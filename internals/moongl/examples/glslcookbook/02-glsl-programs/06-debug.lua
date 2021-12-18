#!/usr/bin/env lua

local gl = require("moongl")
local glfw = require("moonglfw")

local fmt = string.format
local function printf(...) io.write(fmt(...)) end	

local TITLE = "Chapter 2 - Debug"
local W, H = 500, 500

-- GLFW/GL inits:
glfw.version_hint(4, 3, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('resizable', false)
glfw.window_hint('opengl debug context', true)
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

-- Set the debug callback --------------------------------------------------------
-- gl.enable('debug output') -- this is not necessary when we have a debug context.
gl.debug_message_callback(function (source, type_, id, severity, message)
	printf("%s:%s[%s](%d): %s\n", source, type_, severity, id, message)
end)
gl.debug_message_control("don't care", "don't care", "don't care", true, nil)
gl.debug_message_insert('application', 'marker', 'notification', 0, "Start debugging")

-- Build the shading program --------------------------------------------------
local vshader = "shaders/basic.vert"
local fshader = "shaders/basic.frag"
local program, vsh, fsh = gl.make_program('vertex', vshader, 'fragment', fshader)
--local program, vsh, fsh = gl.make_program({vertex=vshader, fragment=fshader}) -- alt.
gl.delete_shaders(vsh, fsh)
gl.use_program(program)

-- Get and print the active attributes ----------------------------------------
local num_attribs = gl.get_program_interface(program, 'program input', 'active resources')
print("Active attributes:")
for i = 0, num_attribs-1 do
	-- NB: the index expected by gl.get_program_resource_xxx() i is 0-based
	local type_ = gl.get_program_resource(program, 'program input', i, 'type')
	local location = gl.get_program_resource(program, 'program input', i, 'location')
	local name = gl.get_program_resource_name(program, 'program input', i)
   printf("%-5d %s (%s)\n", location, name, type_)
end

-- Create the VBO --------------------------------------------------------------
local positions = {
  -0.8, -0.8, 0.0,
   0.8, -0.8, 0.0,
   0.0,  0.8, 0.0
}
local colors = {
   1.0, 0.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 0.0, 1.0
}

-- Create and populate the buffer objects
local position_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', positions), 'static draw')
local color_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', colors), 'static draw')

-- Create and set-up the vertex array object
local vao = gl.new_vertex_array()

gl.enable_vertex_attrib_array(0) -- Vertex position
gl.enable_vertex_attrib_array(1) -- Vertex color

gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', color_vbo)
gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)

-- Main loop -------------------------------------------------------------------
gl.clear_color(0.5, 0.5, 0.5, 1.0)

-- Main loop:
while not glfw.window_should_close(window) do
   glfw.poll_events()
   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   glfw.swap_buffers(window)
end

gl.delete_program(program)
gl.delete_vertex_arrays(vao)
gl.delete_buffers(position_vbo, color_vbo)
