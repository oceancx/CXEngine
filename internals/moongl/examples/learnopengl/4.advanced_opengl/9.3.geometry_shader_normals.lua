#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
local new_camera = require("common.camera")
local new_model = require("common.model")

-- A few shortcuts:
local vec3, mat4 = glmath.vec3, glmath.mat4
local rotate, translate, scale = glmath.rotate, glmath.translate, glmath.scale
local transpose = glmath.transpose
local clamp = glmath.clamp
local perspective = glmath.perspective
local rad, sin, cos = math.rad, math.sin, math.cos

local SCR_WIDTH, SCR_HEIGHT = 1280, 720
-- camera:
local camera = new_camera(vec3(0.0, 0.0, 3.0))
local last_x, last_y = SCR_WIDTH/2, SCR_HEIGHT/2 -- initially at the center
local first_mouse = true

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
local prog, vsh, fsh = gl.make_program({
   vertex="shaders/9.3.default.vert",
   fragment="shaders/9.3.default.frag",
})
gl.delete_shaders(vsh, fsh)
local prog1, vsh, fsh, gsh = gl.make_program({
   vertex="shaders/9.3.normal_visualization.vert",
   fragment="shaders/9.3.normal_visualization.frag",
   geometry="shaders/9.3.normal_visualization.geom",
})
gl.delete_shaders(vsh, fsh, gsh)

-- load models
local nanosuit = new_model("../resources/objects/nanosuit/nanosuit.obj")

-- get the locations of the uniforms:
local loc = {} -- holds the locations for prog (indexed by the uniform variables names)
gl.use_program(prog)
loc.model = gl.get_uniform_location(prog, "model")
loc.view = gl.get_uniform_location(prog, "view")
loc.projection = gl.get_uniform_location(prog, "projection")
local loc1 = {} -- holds the locations for prog1 (indexed by the uniform variables names)
gl.use_program(prog1)
loc1.model = gl.get_uniform_location(prog1, "model")
loc1.view = gl.get_uniform_location(prog1, "view")
loc1.projection = gl.get_uniform_location(prog1, "projection")

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

   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   -- configure transformation matrices
   local projection = perspective(rad(45.0), SCR_WIDTH/SCR_HEIGHT, 1.0, 100.0)
   local view = camera:view()
   local model = mat4()
   
   -- draw model as usual
   gl.use_program(prog)
   gl.uniform_matrix4f(loc.projection, true, projection)
   gl.uniform_matrix4f(loc.model, true, model) 
   gl.uniform_matrix4f(loc.view, true, view)
   nanosuit:draw(prog)

   -- draw model with normal-visualizing shader
   gl.use_program(prog1)
   gl.uniform_matrix4f(loc1.projection, true, projection)
   gl.uniform_matrix4f(loc1.model, true, model) 
   gl.uniform_matrix4f(loc1.view, true, view)
   nanosuit:draw(prog1)

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

