#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
package.path = package.path..";../?.lua" -- hack to require from ../common
local new_camera = require("common.camera")

-- A few shortcuts:
local vec3, mat4 = glmath.vec3, glmath.mat4
local rotate, translate, scale = glmath.rotate, glmath.translate, glmath.scale
local clamp = glmath.clamp
local perspective = glmath.perspective
local rad, sin, cos = math.rad, math.sin, math.cos

local SCR_WIDTH, SCR_HEIGHT = 800, 600
-- camera:
local camera = new_camera(vec3(0.0, 0.0, 3.0))
local last_x, last_y = SCR_WIDTH/2, SCR_HEIGHT/2 -- initially at the center
local first_mouse = true
-- lighting:
local light_pos = vec3(1.2, 1.0, 2.0)

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
-- lighting shader program:
local prog1, vsh, fsh = gl.make_program({vertex="shaders/2.2.basic_lighting.vert",
                                         fragment="shaders/2.2.basic_lighting.frag"})
gl.delete_shaders(vsh, fsh)
-- lamp shader program:
local prog2, vsh, fsh = gl.make_program({vertex="shaders/2.2.lamp.vert",
                                         fragment="shaders/2.2.lamp.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ---------
local vertices = {
  -- positions       normals
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,
   0.5, -0.5, -0.5,  0.0,  0.0, -1.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
  -0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,

  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,
   0.5, -0.5,  0.5,  0.0,  0.0,  1.0,
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0,
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0,
  -0.5,  0.5,  0.5,  0.0,  0.0,  1.0,
  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,

  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,
  -0.5,  0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5,  0.5, -1.0,  0.0,  0.0,
  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,

   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,
   0.5,  0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5,  0.5,  1.0,  0.0,  0.0,
   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,

  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,
   0.5, -0.5, -0.5,  0.0, -1.0,  0.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
  -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,

  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,
   0.5,  0.5, -0.5,  0.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
  -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0
}
 
-- First, set up te vao for the cube:
local cube_vao = gl.new_vertex_array() -- gen & bind in one call
local vbo = gl.new_buffer('array') -- gen & bind in one call
gl.buffer_data('array', gl.pack('float', vertices), 'static draw')
-- position attribute
gl.vertex_attrib_pointer(0, 3, 'float', false, 6*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
-- normal attribute
gl.vertex_attrib_pointer(1, 3, 'float', false, 6*gl.sizeof('float'),  3*gl.sizeof('float'))
gl.enable_vertex_attrib_array(1)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- Second, set up the vao for the lamp, which we also represent as a cube so
-- we can reuse the previous vbo (we don't need the normals here, though):
local light_vao = gl.new_vertex_array() -- gen & bind in one call
gl.bind_buffer('array', vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 6*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- get the locations of the uniforms: 
local loc1 = {} -- holds the locations for prog1 (indexed by the uniform variables names)
loc1.model = gl.get_uniform_location(prog1, "model")
loc1.view = gl.get_uniform_location(prog1, "view")
loc1.projection = gl.get_uniform_location(prog1, "projection")
loc1.objectColor = gl.get_uniform_location(prog1, "objectColor")
loc1.lightColor = gl.get_uniform_location(prog1, "lightColor")
loc1.lightPos = gl.get_uniform_location(prog1, "lightPos")
local loc2 = {} -- holds the locations for prog2
loc2.model = gl.get_uniform_location(prog2, "model")
loc2.view = gl.get_uniform_location(prog2, "view")
loc2.projection = gl.get_uniform_location(prog2, "projection")

glfw.set_framebuffer_size_callback(window, function (window, w, h)
   gl.viewport(0, 0, w, h)
   SCR_WIDTH, SCR_HEIGHT = w, h
end)

glfw.set_cursor_pos_callback(window, function(window, xpos, ypos)
   -- whenever the mouse moves, this callback is called
   if first_mouse then
      last_x, last_y = xpos, ypos
      first_mouse = false
   end
   local xoffset = xpos - last_x
   local yoffset = last_y - ypos -- reversed since y-coordinates go from bottom to top
   last_x, last_y = xpos, ypos
   camera:process_mouse(xoffset, yoffset, true)
end)

glfw.set_scroll_callback(window, function(window, xoffset, yoffset)
   -- whenever the mouse scroll wheel scrolls, this callback is called
   camera:process_scroll(yoffset)
end)

-- tell GLFW to capture our mouse:
glfw.set_input_mode(window, 'cursor', 'disabled')

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
   -- camera movement controlled either by WASD keys or arrow keys:
   if keypressed('w') or keypressed('up') then camera:process_keyboard('forward', dt) end
   if keypressed('a') or keypressed('left') then camera:process_keyboard('left', dt) end
   if keypressed('s') or keypressed('down') then camera:process_keyboard('backward', dt) end
   if keypressed('d') or keypressed('right') then camera:process_keyboard('right', dt) end

   -- render
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   
   -- render the cube:
   gl.use_program(prog1)
   gl.uniformf(loc1.objectColor, 1.0, 0.5, 0.31)
   gl.uniformf(loc1.lightColor, 1.0, 1.0, 1.0)
   gl.uniformf(loc1.lightPos, light_pos)
   gl.uniformf(loc1.lightPos, camera.position)
   local view = camera:view()
   local model = mat4()
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(loc1.view, true, view)
   gl.uniform_matrix4f(loc1.projection, true, projection)
   gl.uniform_matrix4f(loc1.model, true, model)
   gl.bind_vertex_array(cube_vao)
   gl.draw_arrays('triangles', 0, 36)
   gl.unbind_vertex_array()
 
   -- render the lamp:
   gl.use_program(prog2)
   gl.uniform_matrix4f(loc2.view, true, view)
   gl.uniform_matrix4f(loc2.projection, true, projection)
   local model = translate(light_pos)*scale(0.2) -- a smaller cube
   gl.uniform_matrix4f(loc2.model, true, model)
   gl.bind_vertex_array(light_vao)
   gl.draw_arrays('triangles', 0, 36)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(cube_vao, light_vao)
gl.delete_buffers(vbo)
