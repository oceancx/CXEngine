#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
package.path = package.path..";../?.lua" -- hack to require from ../common
local new_camera = require("common.camera")
local load_texture = require("common.texture")

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
-- lighting shader program:
local prog1, vsh, fsh = gl.make_program({vertex="shaders/6.multiple_lights.vert",
                                         fragment="shaders/6.multiple_lights.frag"})
gl.delete_shaders(vsh, fsh)
-- lamp shader program:
local prog2, vsh, fsh = gl.make_program({vertex="shaders/6.lamp.vert",
                                         fragment="shaders/6.lamp.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ---------
local vertices = {
  -- positions       normals           texture coords
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
   0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  0.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
  -0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  1.0,
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,

  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,
   0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
  -0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  1.0,
  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,

  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  0.0,
  -0.5,  0.5, -0.5, -1.0,  0.0,  0.0,  1.0,  1.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  1.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  1.0,
  -0.5, -0.5,  0.5, -1.0,  0.0,  0.0,  0.0,  0.0,
  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  0.0,

   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,  1.0,  0.0,
   0.5,  0.5, -0.5,  1.0,  0.0,  0.0,  1.0,  1.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,  0.0,  1.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,  0.0,  1.0,
   0.5, -0.5,  0.5,  1.0,  0.0,  0.0,  0.0,  0.0,
   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,  1.0,  0.0,

  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
   0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
  -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  0.0,  0.0,
  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  1.0,

  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0,
   0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  1.0,  1.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  1.0,  0.0,
  -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  0.0,  0.0,
  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0
}

-- positions all containers
local cube_positions = {
   vec3( 0.0,  0.0,  0.0),
   vec3( 2.0,  5.0, -15.0),
   vec3(-1.5, -2.2, -2.5),
   vec3(-3.8, -2.0, -12.3),
   vec3( 2.4, -0.4, -3.5),
   vec3(-1.7,  3.0, -7.5),
   vec3( 1.3, -2.0, -2.5),
   vec3( 1.5,  2.0, -2.5),
   vec3( 1.5,  0.2, -1.5),
   vec3(-1.3,  1.0, -1.5)
}

-- positions of the point lights
local point_light_positions = {
   vec3( 0.7,  0.2,  2.0),
   vec3( 2.3, -3.3, -4.0),
   vec3(-4.0,  2.0, -12.0),
   vec3( 0.0,  0.0, -3.0)
}

-- First, set up te vao for the cube:
local cube_vao = gl.new_vertex_array() -- gen & bind in one call
local vbo = gl.new_buffer('array') -- gen & bind in one call
gl.buffer_data('array', gl.pack('float', vertices), 'static draw')
-- position attribute
gl.vertex_attrib_pointer(0, 3, 'float', false, 8*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
-- normal attribute
gl.vertex_attrib_pointer(1, 3, 'float', false, 8*gl.sizeof('float'),  3*gl.sizeof('float'))
gl.enable_vertex_attrib_array(1)
-- texture coords attribute
gl.vertex_attrib_pointer(2, 2, 'float', false, 8*gl.sizeof('float'),  6*gl.sizeof('float'))
gl.enable_vertex_attrib_array(2)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- Second, set up the vao for the lamp, which we also represent as a cube so
-- we can reuse the previous vbo (we don't need normals nor texture coords here, though):
local light_vao = gl.new_vertex_array() -- gen & bind in one call
gl.bind_buffer('array', vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 8*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- load textures (we now use a utility function to keep the code more organized)
local diffuse_map = load_texture("../resources/textures/container2.png")
local specular_map = load_texture("../resources/textures/container2_specular.png")

-- get the locations of the uniforms: 
local loc1 = {} -- holds the locations for prog1 (indexed by the uniform variables names)
local function getloc(var) loc1[var] = gl.get_uniform_location(prog1, var) end
getloc("model")
getloc("view")
getloc("projection")
getloc("viewPos")
getloc("dirLight.direction")
getloc("dirLight.ambient")
getloc("dirLight.diffuse")
getloc("dirLight.specular")
for i,pos in ipairs(point_light_positions) do
   getloc("pointLights["..(i-1).."].position")
   getloc("pointLights["..(i-1).."].ambient")
   getloc("pointLights["..(i-1).."].diffuse")
   getloc("pointLights["..(i-1).."].specular")
   getloc("pointLights["..(i-1).."].constant")
   getloc("pointLights["..(i-1).."].linear")
   getloc("pointLights["..(i-1).."].quadratic")
end
-- spotLight
getloc("spotLight.position")
getloc("spotLight.direction")
getloc("spotLight.ambient")
getloc("spotLight.diffuse")
getloc("spotLight.specular")
getloc("spotLight.constant")
getloc("spotLight.linear")
getloc("spotLight.quadratic")
getloc("spotLight.cutOff")
getloc("spotLight.outerCutOff")
getloc("material.diffuse")
getloc("material.specular")
getloc("material.shininess")
local loc2 = {} -- holds the locations for prog2
local function getloc(var) loc2[var] = gl.get_uniform_location(prog2, var) end
getloc("model")
getloc("view")
getloc("projection")
getloc = nil

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
   
   -- render the cubes:
   gl.use_program(prog1)
   gl.uniformf(loc1.viewPos, camera.position)
   -- material properties
   gl.uniformi(loc1["material.diffuse"], 0) -- texture unit
   gl.uniformi(loc1["material.specular"], 1) -- texture unit
   gl.uniformf(loc1["material.shininess"], 32.0)
   -- Here we set all the uniforms for the 5/6 types of lights we have. We have to set
   -- them manually and index the proper PointLight struct in the array to set each uniform
   -- variable. This can be done more code-friendly by defining light types as classes and
   -- set their values in there, or by using a more efficient uniform approach by using
   -- 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
   -- directional light
   gl.uniformf(loc1["dirLight.direction"], -0.2, -1.0, -0.3)
   gl.uniformf(loc1["dirLight.ambient"], 0.05, 0.05, 0.05)
   gl.uniformf(loc1["dirLight.diffuse"], 0.4, 0.4, 0.4)
   gl.uniformf(loc1["dirLight.specular"], 0.5, 0.5, 0.5)
   -- point lights
   for i, pos in ipairs(point_light_positions) do
      gl.uniformf(loc1["pointLights["..(i-1).."].position"], pos)
      gl.uniformf(loc1["pointLights["..(i-1).."].ambient"], 0.05, 0.05, 0.05)
      gl.uniformf(loc1["pointLights["..(i-1).."].diffuse"], 0.8, 0.8, 0.8)
      gl.uniformf(loc1["pointLights["..(i-1).."].specular"], 1.0, 1.0, 1.0)
      gl.uniformf(loc1["pointLights["..(i-1).."].constant"], 1.0)
      gl.uniformf(loc1["pointLights["..(i-1).."].linear"], 0.09)
      gl.uniformf(loc1["pointLights["..(i-1).."].quadratic"], 0.032)
   end
   -- spotLight
   gl.uniformf(loc1["spotLight.position"], camera.position)
   gl.uniformf(loc1["spotLight.direction"], camera.front)
   gl.uniformf(loc1["spotLight.ambient"], 0.0, 0.0, 0.0)
   gl.uniformf(loc1["spotLight.diffuse"], 1.0, 1.0, 1.0)
   gl.uniformf(loc1["spotLight.specular"], 1.0, 1.0, 1.0)
   gl.uniformf(loc1["spotLight.constant"], 1.0)
   gl.uniformf(loc1["spotLight.linear"], 0.09)
   gl.uniformf(loc1["spotLight.quadratic"], 0.032)
   gl.uniformf(loc1["spotLight.cutOff"], cos(rad(12.5)))
   gl.uniformf(loc1["spotLight.outerCutOff"], cos(rad(15.0)))

   -- transforms
   local view = camera:view()
   local model = mat4()
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(loc1.view, true, view)
   gl.uniform_matrix4f(loc1.projection, true, projection)
   -- bind diffuse and specular maps
   gl.active_texture(0)
   gl.bind_texture('2d', diffuse_map)
   gl.active_texture(1)
   gl.bind_texture('2d', specular_map)
   -- draw
   gl.bind_vertex_array(cube_vao)
   local rot_dir = vec3(1.0, 0.3, 0.5):normalize()
   for i, pos in ipairs(cube_positions) do
      -- calculate the model matrix for each object and pass it to shader before drawing
      local model = translate(pos)* rotate(rad(20.0*(i-1)), rot_dir)
      gl.uniform_matrix4f(loc1.model, true, model)
      gl.draw_arrays('triangles', 0, 36)
   end
   gl.unbind_vertex_array()

   gl.use_program(prog2)
   gl.uniform_matrix4f(loc2.view, true, view)
   gl.uniform_matrix4f(loc2.projection, true, projection)
   -- we now draw as many light bulbs as we have point lights.
   gl.bind_vertex_array(light_vao)
   for _, pos in ipairs(point_light_positions) do
      local model = translate(pos)*scale(.2) -- Make it a smaller cube
      gl.uniform_matrix4f(loc2.model, true, model)
      gl.draw_arrays('triangles', 0, 36)
   end
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(cube_vao, light_vao)
gl.delete_buffers(vbo)
