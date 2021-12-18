#!/usr/bin/env lua

local gl = require("moongl")
local glfw = require("moonglfw")

local fmt = string.format
local function printf(...) io.write(fmt(...)) end	

local TITLE = "Chapter 2 - Basic scene"
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

-- Dump GL info:
printf("-------------------------------------------------------------\n")
printf("GL Vendor    : %s\n", gl.get_string('renderer'))
printf("GL Renderer  : %s\n", gl.get_string('vendor'))
printf("GL Version   : %s\n", gl.get_string('version'))
printf("GL Version   : %d.%d\n", gl.get('major version'), gl.get('minor version'))
printf("GLSL Version : %s\n", gl.get_string('shading language version'))
printf("-------------------------------------------------------------\n")
if false then -- print extensions
	local n = gl.get('num extensions')
	for i = 0, n-1 do
		print(gl.get_string('extensions', i))
	end
end

---------- Vertex shader --------------------------
-- Load contents of file
local f = assert(io.open("shaders/basic.vert", "r"))
local code = f:read("a")
f:close()
-- Create the shader object
local vsh = gl.create_shader('vertex')
-- Load the source code into the shader object
gl.shader_source(vsh, code)
-- Compile the shader
gl.compile_shader(vsh)
-- Check compilation status
local result = gl.get_shader(vsh, 'compile status')
if not result then
	local log = gl.get_shader_info_log(vsh)
	error(fmt("Vertex shader compilation failed!\nShader log: \n%s", log))
end
---------- Fragment shader --------------------------
local f = assert(io.open("shaders/basic.frag", "r"))
local code = f:read("a")
f:close()
-- Create the shader object
local fsh = gl.create_shader('fragment')
-- Load the source code into the shader object
gl.shader_source(fsh, code)
-- Compile the shader
gl.compile_shader(fsh)
-- Check compilation status
local result = gl.get_shader(fsh, 'compile status')
if not result then
	local log = gl.get_shader_info_log(fsh)
	error(fmt("Fragment shader compilation failed!\nShader log: \n%s", log))
end

-- Create the program object
local program = gl.create_program()

-- Attach the shaders to the program object
gl.attach_shader(program, vsh)
gl.attach_shader(program, fsh)

-- Link the program
gl.link_program(program)

-- Check for successful linking
local status = gl.get_program(program, 'link status')
if not status then
	local log = gl.get_program_info_log(program)
	error(fmt("Failed to link shader program!\nProgram log: \n%s", log))
end

gl.use_program(program)

------------------ Create the VBO --------------------
local positions = {
  -0.8, -0.8, 0.0,
   0.8, -0.8, 0.0,
   0.0,  0.8, 0.0,
}
local colors = {
   1.0, 0.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 0.0, 1.0,
}

-- Create and populate the buffer objects
local position_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', positions), 'static draw')
local color_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', colors), 'static draw')

-- Create and set-up the vertex array object
local vao = gl.new_vertex_array()

gl.enable_vertex_attrib_array(0)  -- Vertex position
gl.enable_vertex_attrib_array(1)  -- Vertex color

gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', color_vbo)
gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)

gl.unbind_vertex_array()

gl.clear_color(0.5, 0.5, 0.5, 1.0)

-- Main loop:
while not glfw.window_should_close(window) do
   glfw.poll_events()

   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end

gl.delete_program(program)
gl.delete_vertex_arrays(vao)
gl.delete_buffers(position_vbo, color_vbo)
