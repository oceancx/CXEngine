#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")

-- A few shortcuts:
local vec2, vec3, mat4 = glmath.vec2, glmath.vec3, glmath.mat4
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
local prog, vsh, fsh = gl.make_program({
   vertex="shaders/10.1.instancing.vert",
   fragment="shaders/10.1.instancing.frag",
})
gl.delete_shaders(vsh, fsh)


-- generate a list of 100 quad locations/translation-vectors
local translations = {}
for y = -10, 10-2, 2 do
   for x = -10, 10-2, 2 do
      table.insert(translations, vec2(x, y)/10 + vec2(0.1, 0.1))
   end
end

-- store instance data in an array buffer
local instance_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(translations), 'static draw')
gl.unbind_buffer('array')

-- set up vertex data (and buffer(s)) and configure vertex attributes ----------

local quad_vertices = {
   -- positions     -- colors
  -0.05,  0.05,  1.0, 0.0, 0.0,
   0.05, -0.05,  0.0, 1.0, 0.0,
  -0.05, -0.05,  0.0, 0.0, 1.0,
   -0.05,  0.05,  1.0, 0.0, 0.0,
   0.05, -0.05,  0.0, 1.0, 0.0,
   0.05,  0.05,  0.0, 1.0, 1.0
}

local quad_vao = gl.new_vertex_array()
local quad_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(quad_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 2, 'float', false, 5*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 3, 'float', false, 5*gl.sizeof('float'), 2*gl.sizeof('float'))
-- also set instance data
gl.enable_vertex_attrib_array(2)
gl.bind_buffer('array', instance_vbo) -- this attribute comes from a different vertex buffer
gl.vertex_attrib_pointer(2, 2, 'float', false, 2*gl.sizeof('float'), 0)
gl.unbind_buffer('array')
gl.vertex_attrib_divisor(2, 1) -- tell OpenGL this is an instanced vertex attribute.
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

   -- draw 100 instanced quads
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   gl.use_program(prog)
   gl.bind_vertex_array(quad_vao)
   gl.draw_arrays('triangles', 0, 6, 100) -- 100 triangles of 6 vertices each
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

gl.delete_vertex_arrays(quad_vao)
gl.delete_buffers(quad_vbo, instance_vbo)

