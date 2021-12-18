#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
package.path = package.path..";../?.lua" -- hack to require from ../common
local new_camera = require("common.camera")
local new_model = require("common.model")

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

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
local prog, vsh, fsh = gl.make_program({vertex="shaders/1.model_loading.vert",
                                         fragment="shaders/1.model_loading.frag"})
gl.delete_shaders(vsh, fsh)

-- load models
local nanosuit = new_model("../resources/objects/nanosuit/nanosuit.obj")

-- To draw in wireframe polygons, run the script with the '-w' option as firts arg:
if arg[1]=='-w' then
   gl.polygon_mode('front and back', 'line')
end

-- get the locations of the uniforms:
local loc = {} -- holds the locations for prog (indexed by the uniform variables names)
loc.model = gl.get_uniform_location(prog, "model")
loc.view = gl.get_uniform_location(prog, "view")
loc.projection = gl.get_uniform_location(prog, "projection")

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

   -- render the loaded model
   gl.clear_color(0.05, 0.05, 0.05, 1.0)
   gl.clear('color', 'depth')
   gl.use_program(prog)
   local model = translate(0.0, -1.75, 0.0)*scale(0.2)
   local view = camera:view()
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(loc.model, true, model)
   gl.uniform_matrix4f(loc.view, true, view)
   gl.uniform_matrix4f(loc.projection, true, projection)
   nanosuit:draw(prog)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(cube_vao, light_vao)
gl.delete_buffers(vbo)

