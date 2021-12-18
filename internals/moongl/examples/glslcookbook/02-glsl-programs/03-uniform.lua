#!/usr/bin/env lua
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")

local TITLE = "Chapter 2 - Basic scene with a uniform variable"
local W, H = 500, 500

-- GLFW/GL initializations ----------------------------------------------------
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('resizable', false)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local animate = true
glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
	elseif key == 'space' and action == 'press' then
		animate = not animate
   end
end)

glfw.set_window_size_callback(window, function(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
end)

-- Compile shader program -----------------------------------------------------

local prog, vsh, fsh = gl.make_program('vertex', "shaders/uniform.vert", 'fragment', "shaders/uniform.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

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
gl.enable_vertex_attrib_array(0)  -- Vertex position
gl.enable_vertex_attrib_array(1)  -- Vertex color
gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', color_vbo)
gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)
gl.unbind_vertex_array() -- same as gl.bind_vertex_array(0)

-- Event loop -----------------------------------------------------------------

gl.clear_color(0.5, 0.5, 0.5, 1.0)

local pi = math.pi
local angle, speed = 0, pi/3 -- rad, rad/s
local location = gl.get_uniform_location(prog, "RotationMatrix")

print("Press space to toggle animation on/off")
local t0 = glfw.now()
while not glfw.window_should_close(window) do
   glfw.poll_events()

	-- Update
	local t = glfw.now()
	local dt = t - t0
	t0 = t
	if animate then
		angle = angle + speed*dt
		if angle >= 2*pi then angle = angle - 2*pi end
	end
	local rotation_matrix = glmath.rotate(angle, 0, 0, 1)
	gl.uniform_matrix4f(location, true, rotation_matrix)

   -- Render
   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end

