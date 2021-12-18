#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")
local mi = require("moonimage")
local glmath = require("moonglmath")
package.path = package.path..";../?.lua" -- hack to require from ../common
local new_camera = require("common.camera")

-- A few shortcuts:
local vec3 = glmath.vec3
local rotate, translate = glmath.rotate, glmath.translate
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

-- build, compile, and link our shader program ---------------------------------
local prog, vsh, fsh = gl.make_program({vertex="shaders/7.4.camera.vert",
                                         fragment="shaders/7.4.camera.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ---------
local vertices = {
   -- positions      -- texture coords
  -0.5, -0.5, -0.5,  0.0, 0.0,
   0.5, -0.5, -0.5,  1.0, 0.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
  -0.5,  0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 0.0,

  -0.5, -0.5,  0.5,  0.0, 0.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 1.0,
   0.5,  0.5,  0.5,  1.0, 1.0,
  -0.5,  0.5,  0.5,  0.0, 1.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,

  -0.5,  0.5,  0.5,  1.0, 0.0,
  -0.5,  0.5, -0.5,  1.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,
  -0.5,  0.5,  0.5,  1.0, 0.0,

   0.5,  0.5,  0.5,  1.0, 0.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5,  0.5,  0.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 0.0,

  -0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5, -0.5,  1.0, 1.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,

  -0.5,  0.5, -0.5,  0.0, 1.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5,  0.5,  0.5,  1.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 0.0,
  -0.5,  0.5,  0.5,  0.0, 0.0,
  -0.5,  0.5, -0.5,  0.0, 1.0
}

-- world space positions of our cubes
local cubePositions = {
   vec3( 0.0,  0.0,  0.0),
   vec3( 2.0,  5.0, -15.0),
   vec3(-1.5, -2.2, -2.5),
   vec3(-3.8, -2.0, -12.3),
   vec3( 2.4, -0.4, -3.5),
   vec3(-1.7,  3.0, -7.5),
   vec3( 1.3, -2.0, -2.5),
   vec3( 1.5,  2.0, -2.5),
   vec3( 1.5,  0.2, -1.5),
   vec3(-1.3,  1.0, -1.5),
}

local vao = gl.gen_vertex_arrays()
local vbo = gl.gen_buffers()
gl.bind_vertex_array(vao)
gl.bind_buffer('array', vbo)
gl.buffer_data('array', gl.pack('float', vertices), 'static draw')
-- position attribute
gl.vertex_attrib_pointer(0, 3, 'float', false, 5*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
-- texture coords attribute
gl.vertex_attrib_pointer(1, 2, 'float', false, 5*gl.sizeof('float'), 3*gl.sizeof('float'))
gl.enable_vertex_attrib_array(1)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- load and create 2 textures -------------------------------------------------
mi.flip_vertically_on_load(true)
local texture1 = gl.gen_textures() 
gl.bind_texture('2d', texture1)
gl.texture_parameter('2d', 'wrap s', 'repeat')
gl.texture_parameter('2d', 'wrap t', 'repeat')
gl.texture_parameter('2d', 'min filter', 'linear')
gl.texture_parameter('2d', 'mag filter', 'linear')
local data, w, h, channels = mi.load("../resources/textures/container.jpg")
-- print(#data, w, h, channels, #channels*w*h)
gl.texture_image('2d', 0, 'rgb', 'rgb', 'ubyte', data, w, h)
gl.generate_mipmap('2d')

local texture2 = gl.gen_textures() 
gl.bind_texture('2d', texture2)
gl.texture_parameter('2d', 'wrap s', 'repeat')
gl.texture_parameter('2d', 'wrap t', 'repeat')
gl.texture_parameter('2d', 'min filter', 'linear')
gl.texture_parameter('2d', 'mag filter', 'linear')
local data, w, h, channels = mi.load("../resources/textures/awesomeface.png")
-- print(#data, w, h, channels, #channels*w*h)
gl.texture_image('2d', 0, 'rgb', 'rgba', 'ubyte', data, w, h)
gl.generate_mipmap('2d')
data, w, h, nchannels = nil -- we don't need these any more

-- tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
gl.use_program(prog) -- don't forget to activate/use the shader before setting uniforms!
gl.uniformi(gl.get_uniform_location(prog, "texture1"), 0)
gl.uniformi(gl.get_uniform_location(prog, "texture2"), 1)
-- get the locations of the uniforms holding the transform matrices: 
local model_loc = gl.get_uniform_location(prog, "model")
local view_loc = gl.get_uniform_location(prog, "view")
local projection_loc = gl.get_uniform_location(prog, "projection")
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
   gl.clear_color(0.2, 0.3, 0.3, 1.0)
   gl.clear('color', 'depth')
   -- bind textures on corresponding texture units, and draw
   gl.active_texture(0)
   gl.bind_texture('2d', texture1)
   gl.active_texture(1)
   gl.bind_texture('2d', texture2)
   
   gl.use_program(prog)
   -- camera/view transformation
   gl.uniform_matrix4f(view_loc, true, camera:view())
   -- projection transform (in this example it may change at every frame):
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(projection_loc, true, projection)
   -- render the cubes, each with a different model transform:
   gl.bind_vertex_array(vao)
   local rotdir = vec3(1.0, 0.3, 0.5):normalize()
   for i, pos in ipairs(cubePositions) do
      -- calculate the model matrix for each object and pass it to shader before drawing
      local angle = rad(20.0 * (i-1))
      local model = translate(pos)*rotate(angle, rotdir)
      gl.uniform_matrix4f(model_loc, true, model)
      gl.draw_arrays('triangles', 0, 36)
   end
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(vao)
gl.delete_buffers(vbo)
