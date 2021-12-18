#!/usr/bin/env lua
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")

local TITLE = "Chapter 2 - Scene with a uniform block variable"
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

local prog, vsh, fsh = gl.make_program('vertex', "shaders/uniformblock.vert", 'fragment', "shaders/uniformblock.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Init uniform block buffer --------------------------------------------------

-- Get the index of the uniform block
local block_index = gl.get_uniform_block_index(prog, "BlobSettings")

-- Allocate space for the buffer
local block_size = gl.get_active_uniform_block(prog, block_index, 'data size')

-- Query for the offsets of each block variable
local names = {
	"BlobSettings.InnerColor",
	"BlobSettings.OuterColor",
	"BlobSettings.RadiusInner",
	"BlobSettings.RadiusOuter"
}

local indices = {gl.get_uniform_indices(prog, table.unpack(names))}
local offsets = {gl.get_active_uniforms(prog, 'offset', table.unpack(indices))}

-- Store data within the buffer at the appropriate offsets
local inner_color = { 1.0, 1.0, 0.75, 1.0 }
local outer_color = { 0.0, 0.0, 0.0, 0.0 }
local inner_radius = 0.25 -- inner radius
local outer_radius =	0.45 -- outer radius

local block_buffer = glmath.malloc(block_size)
block_buffer:write(offsets[1], 'float', inner_color)
block_buffer:write(offsets[2], 'float', outer_color)
block_buffer:write(offsets[3], 'float', inner_radius)
block_buffer:write(offsets[4], 'float', outer_radius)
-- print(table.concat(block_buffer:read(0, nil, 'float'), ', '))

-- Create the buffer object and copy the data
local ubo = gl.new_buffer('uniform')
gl.buffer_data('uniform', {block_buffer:ptr(), block_size}, 'dynamic draw')
glmath.free(block_buffer) -- we don't need this any more

-- Bind the buffer object to the uniform block
gl.bind_buffer_base('uniform', 1, ubo)

-- We don't need this if we specify the binding within the shader (OpenGL 4.2 and above):
-- gl.uniform_block_binding(prog, block_index, 1)

-- Init scene -----------------------------------------------------------------

local position_data = {
   -0.8, -0.8, 0.0,
    0.8, -0.8, 0.0,
    0.8,  0.8, 0.0,
   -0.8, -0.8, 0.0,
    0.8, 0.8, 0.0,
   -0.8, 0.8, 0.0
}
local texcoord_data = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 1.0,
    0.0, 1.0
}

-- Create and populate the buffer objects
local position_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', position_data), 'static draw')
local texcoord_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', texcoord_data), 'static draw')

-- Create and set-up the vertex array object
local vao = gl.new_vertex_array()
gl.enable_vertex_attrib_array(0)  -- Vertex position
gl.enable_vertex_attrib_array(1)  -- Vertex texture coords
gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', texcoord_vbo)
gl.vertex_attrib_pointer(1, 2, 'float', false, 0, 0)
gl.unbind_vertex_array() -- same as gl.bind_vertex_array(0)

-- Event loop -----------------------------------------------------------------

gl.enable('blend')
gl.blend_func('src alpha', 'one minus src alpha')
gl.clear_color(0.5, 0.5, 0.5, 1.0)

while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Render
   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 6)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end

