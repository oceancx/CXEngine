#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")

-- A few shortcuts:
local vec3, mat4 = glmath.vec3, glmath.mat4
local rotate, translate, scale = glmath.rotate, glmath.translate, glmath.scale
local transpose = glmath.transpose
local clamp = glmath.clamp
local perspective = glmath.perspective
local rad, sin, cos = math.rad, math.sin, math.cos

local SCR_WIDTH, SCR_HEIGHT = 1280, 720

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
local prog, vsh, fsh, gsh = gl.make_program({
   vertex="shaders/9.1.geometry_shader.vert",
   fragment="shaders/9.1.geometry_shader.frag",
   geometry="shaders/9.1.geometry_shader.geom",
})
gl.delete_shaders(vsh, fsh, gsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ----------
local points = {
  -0.5,  0.5, 1.0, 0.0, 0.0, -- top-left
   0.5,  0.5, 0.0, 1.0, 0.0, -- top-right
   0.5, -0.5, 0.0, 0.0, 1.0, -- bottom-right
  -0.5, -0.5, 1.0, 1.0, 0.0, -- bottom-left
}
local vao = gl.new_vertex_array()
local vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(points), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 2, 'float', false, 5*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 3, 'float', false, 5*gl.sizeof('float'), 2*gl.sizeof('float'))
gl.unbind_vertex_array()

glfw.set_framebuffer_size_callback(window, function (window, w, h)
   gl.viewport(0, 0, w, h)
   SCR_WIDTH, SCR_HEIGHT = w, h
end)

-- configure global opengl state
gl.enable('depth test')

local last_frame_time = 0.0 -- last frame time

local function keypressed(x) return glfw.get_key(window, x)=='press' end

-- render loop
while not glfw.window_should_close(window) do
   local t = glfw.get_time()
   local dt = t - last_frame_time
   last_frame_time = t

   -- process input
   if keypressed('escape') then glfw.set_window_should_close(window, true) end

   -- draw points
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   gl.use_program(prog)
   gl.bind_vertex_array(vao)
   gl.draw_arrays('points', 0, 4)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

gl.delete_vertex_arrays(vao)
gl.delete_buffers(vbo)

