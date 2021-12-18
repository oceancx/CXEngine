#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
local new_camera = require("common.camera")

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
local prog_red, vsh, fsh = gl.make_program({vertex="shaders/8.advanced_glsl.vert",
                                         fragment="shaders/8.red.frag"})
gl.delete_shaders(vsh, fsh)
local prog_green, vsh, fsh = gl.make_program({vertex="shaders/8.advanced_glsl.vert",
                                         fragment="shaders/8.green.frag"})
gl.delete_shaders(vsh, fsh)
local prog_blue, vsh, fsh = gl.make_program({vertex="shaders/8.advanced_glsl.vert",
                                         fragment="shaders/8.blue.frag"})
gl.delete_shaders(vsh, fsh)
local prog_yellow, vsh, fsh = gl.make_program({vertex="shaders/8.advanced_glsl.vert",
                                         fragment="shaders/8.yellow.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ----------
local cube_vertices = {
   -- positions
  -0.5, -0.5, -0.5, 
   0.5, -0.5, -0.5,  
   0.5,  0.5, -0.5,  
   0.5,  0.5, -0.5,  
  -0.5,  0.5, -0.5, 
  -0.5, -0.5, -0.5, 

  -0.5, -0.5,  0.5, 
   0.5, -0.5,  0.5,  
   0.5,  0.5,  0.5,  
   0.5,  0.5,  0.5,  
  -0.5,  0.5,  0.5, 
  -0.5, -0.5,  0.5, 

  -0.5,  0.5,  0.5, 
  -0.5,  0.5, -0.5, 
  -0.5, -0.5, -0.5, 
  -0.5, -0.5, -0.5, 
  -0.5, -0.5,  0.5, 
  -0.5,  0.5,  0.5, 

   0.5,  0.5,  0.5,  
   0.5,  0.5, -0.5,  
   0.5, -0.5, -0.5,  
   0.5, -0.5, -0.5,  
   0.5, -0.5,  0.5,  
   0.5,  0.5,  0.5,  

  -0.5, -0.5, -0.5, 
   0.5, -0.5, -0.5,  
   0.5, -0.5,  0.5,  
   0.5, -0.5,  0.5,  
  -0.5, -0.5,  0.5, 
  -0.5, -0.5, -0.5, 

  -0.5,  0.5, -0.5, 
   0.5,  0.5, -0.5,  
   0.5,  0.5,  0.5,  
   0.5,  0.5,  0.5,  
  -0.5,  0.5,  0.5, 
  -0.5,  0.5, -0.5, 
}

-- cube VAO
local cube_vao = gl.new_vertex_array()
local cube_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(cube_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 3, 'float', false, 3*gl.sizeof('float'), 0)
gl.unbind_vertex_array()

-- configure a uniform buffer object ------------------------------------------
-- first. We get the relevant block indices
local uniformBlockIndexRed = gl.get_uniform_block_index(prog_red, "Matrices")
local uniformBlockIndexGreen = gl.get_uniform_block_index(prog_green, "Matrices")
local uniformBlockIndexBlue = gl.get_uniform_block_index(prog_blue, "Matrices")
local uniformBlockIndexYellow = gl.get_uniform_block_index(prog_yellow, "Matrices")
-- then we link each shader's uniform block to this uniform binding point
gl.uniform_block_binding(prog_red, uniformBlockIndexRed, 0)
gl.uniform_block_binding(prog_green, uniformBlockIndexGreen, 0)
gl.uniform_block_binding(prog_blue, uniformBlockIndexBlue, 0)
gl.uniform_block_binding(prog_yellow, uniformBlockIndexYellow, 0)
-- Now actually create the buffer
local sizeof_mat4 = 16*gl.sizeof('float')
local ubo = gl.new_buffer('uniform')
gl.buffer_data('uniform', 2*sizeof_mat4, 'static draw')
-- define the range of the buffer that links to a uniform binding point
gl.bind_buffer_range('uniform', 0, ubo, 0, 2*sizeof_mat4)
gl.unbind_buffer('uniform')

-- get the locations of the uniforms:
local loc_model_red = gl.get_uniform_location(prog_red, "model")
local loc_model_green = gl.get_uniform_location(prog_green, "model")
local loc_model_blue = gl.get_uniform_location(prog_blue, "model")
local loc_model_yellow = gl.get_uniform_location(prog_yellow, "model")

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

   -- render
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   -- Set the view and projection matrix in the uniform block.
   -- We only need to do this once per loop iteration.
   -- Note that we need to transpose the matrices when writing to the uniform block,
   -- because OpenGL expects them in column-major order.
   local projection = perspective(rad(45.0), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   local view = camera:view()
   gl.bind_buffer('uniform', ubo)
   gl.buffer_sub_data('uniform', 0, gl.packf(transpose(projection)))
   gl.buffer_sub_data('uniform', sizeof_mat4, gl.packf(transpose(view)))
   gl.unbind_buffer('uniform')
   gl.bind_vertex_array(cube_vao)
   -- draw red cube 
   gl.use_program(prog_red)
   gl.uniform_matrix4f(loc_model_red, true, translate(-0.75, 0.75, 0.0)) -- move top-left
   gl.draw_arrays('triangles', 0, 36)
   -- draw green cube 
   gl.use_program(prog_green)
   gl.uniform_matrix4f(loc_model_green, true, translate(0.75, 0.75, 0.0)) -- move top-right
   gl.draw_arrays('triangles', 0, 36)
   -- draw blue cube 
   gl.use_program(prog_blue)
   gl.uniform_matrix4f(loc_model_blue, true, translate(-0.75, -0.75, 0.0)) -- move bottom-left
   gl.draw_arrays('triangles', 0, 36)
   -- draw yellow cube 
   gl.use_program(prog_yellow)
   gl.uniform_matrix4f(loc_model_yellow, true, translate(0.75, -0.75, 0.0)) -- move bottom-right
   gl.draw_arrays('triangles', 0, 36)

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

gl.delete_vertex_arrays(cube_vao)
gl.delete_buffers(cube_vbo, ubo)

