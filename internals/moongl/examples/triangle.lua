#!/usr/bin/env lua
-- MoonGL example: triangle.lua
-- 

gl = require "moongl"
glfw = require "moonglfw"

width, height = 800, 600

loc_pos = 0 -- location for position attribute
loc_col = 1 -- location for color attribute

-- Vertex shader:
vshSource = string.format([[
#version 330 core
layout(location=%d) in vec4 position;
layout(location=%d) in vec4 color;
out vec4 Color;

void main() {
   gl_Position = position;
   Color = color;
}
]], loc_pos, loc_col)

-- Fragment shader:
fshSource = [[
#version 330 core
in vec4 Color;
out vec4 out_Color;

void main() {
   out_Color = Color;
}
]]


-- Create a window with GLFW and initialize the GL context:
glfw.window_hint('context version major', 3)
glfw.window_hint('context version minor', 3)
glfw.window_hint('opengl profile', 'core')
window = glfw.create_window(width, height, "Hello, triangle!")
glfw.make_context_current(window)
gl.init()


screen_width, screen_height = glfw.get_framebuffer_size(window)
gl.viewport(0, 0, screen_width, screen_height)

-- Compile shaders and link them to create a shading program:
prog, vsh, fsh = gl.make_program_s('vertex', vshSource, 'fragment', fshSource)
gl.delete_shaders(vsh, fsh)


-- Positions and colors for the triangle's vertices:
positions = {
   -0.5, -0.5,  0.0, 1.0, -- bottom left
    0.5, -0.5,  0.0, 1.0, -- bottom right
    0.0,  0.5,  0.0, 1.0, -- middle top
}

colors = {
    1.0, 0.0, 0.0, 1.0, -- red
    0.0, 1.0, 0.0, 1.0, -- green
    0.0, 0.0, 1.0, 1.0, -- blue
}
   

-- Create a vertex buffer array, load the vertex data on the GPU and define the attributes:
vao = gl.new_vertex_array()
-- position attribute:
vbo_pos = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', positions), 'static draw')
gl.vertex_attrib_pointer(loc_pos, 4, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(loc_pos)
gl.unbind_buffer('array')
-- color attribute:
vbo_col = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', colors), 'static draw')
gl.vertex_attrib_pointer(loc_col, 4, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(loc_col)
gl.unbind_buffer('array')
-- Unbind the vao (will be bound again in the loop):
gl.unbind_vertex_array()


-- Register a callback for keyboard events:
glfw.set_key_callback(window, function(window, key, scancode, action)
   print("keyboard event", window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true) -- will cause exiting the loop
   end
end)

-- Register a callback for window resize events:
glfw.set_window_size_callback(window, function(_, w, h) gl.viewport(0, 0, w, h) end)


-- Event loop:
while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Clear the screen:
   gl.clear_color(0, 0, 0, 1) -- black
   gl.clear('color')

   -- Draw the vertices:
   gl.use_program(prog)
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end


-- cleanup:
gl.delete_buffers(vbo_pos)
gl.delete_buffers(vbo_col)
gl.delete_vertex_arrays(vao)
gl.clean_program(prog)

