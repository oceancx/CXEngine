#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")
local mi = require("moonimage")
local glmath = require("moonglmath")

-- A few shortcuts:
local vec3 = glmath.vec3
local rotate, translate = glmath.rotate, glmath.translate
local perspective = glmath.perspective
local rad = math.rad

local SCR_WIDTH, SCR_HEIGHT = 800, 600

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers
glfw.set_framebuffer_size_callback(window, function (window, width, height)
    gl.viewport(0, 0, width, height)
end)

-- build, compile, and link our shader program ---------------------------------
local prog, vsh, fsh = gl.make_program({vertex="shaders/6.3.coordinate_systems.vert",
                                         fragment="shaders/6.3.coordinate_systems.frag"})
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

-- configure global opengl state
gl.enable('depth test')

-- render loop
while not glfw.window_should_close(window) do
   -- process input
   if glfw.get_key(window, 'escape') == 'press' then
       glfw.set_window_should_close(window, true)
   end

   -- render
   gl.clear_color(0.2, 0.3, 0.3, 1.0)
   gl.clear('color', 'depth') -- also clear the depth buffer now!
   -- bind textures on corresponding texture units, and draw
   gl.active_texture(0)
   gl.bind_texture('2d', texture1)
   gl.active_texture(1)
   gl.bind_texture('2d', texture2)
   
   gl.use_program(prog)
   -- create and set the transformations
   local projection = perspective(rad(45.0), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   local view = translate(0.0, 0.0, -3.0)
   gl.uniform_matrix4f(view_loc, true, view)
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
