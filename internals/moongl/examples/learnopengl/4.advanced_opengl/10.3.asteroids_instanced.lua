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
local camera = new_camera(vec3(0.0, 0.0, 155.0))
local last_x, last_y = SCR_WIDTH/2, SCR_HEIGHT/2 -- initially at the center
local first_mouse = true

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
local prog, vsh, fsh = gl.make_program({
   vertex="shaders/10.3.asteroids.vert",
   fragment="shaders/10.3.asteroids.frag",
})
gl.delete_shaders(vsh, fsh)
local prog1, vsh, fsh = gl.make_program({
   vertex="shaders/10.3.planet.vert",
   fragment="shaders/10.3.planet.frag",
})
gl.delete_shaders(vsh, fsh)

-- load models
local rock = new_model("../resources/objects/rock/rock.obj")
local planet = new_model("../resources/objects/planet/planet.obj")

-- generate a large list of semi-random model transformation matrices
local amount = 100000
local modelMatrices = {}
math.randomseed(os.time())
local function rand(a, b) return math.random()*(b-a)+a end -- random in [a, b)
local radius, offset = 150.0, 25.0
local rotdir = vec3(.4, .6, .8):normalize()
for i = 0, amount-1 do
   -- 1. translation: displace along circle with 'radius' in range [-offset, offset]
   -- 2. scale: Scale between 0.05 and 0.25
   -- 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
   local angle = i/amount*360.0
   local x = sin(angle)*radius + rand(-offset, offset)
   local y = 0.4*rand(-offset, offset) -- keep asteroids' field height smaller compared to width
   local z = cos(angle)*radius + rand(-offset, offset)
   local angle = math.random()*360
   local s = rand(0.05, 0.25)
   -- we transpose the matrices here so we don't have to do it when loading them in the vbo:
   local model = transpose(translate(x, y, z)*scale(s)*rotate(angle, rotdir))
   table.insert(modelMatrices, model)
end

local sizeof_vec4 = 4*gl.sizeof('float')
local sizeof_mat4 = 16*gl.sizeof('float')

-- configure instanced array
local vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(modelMatrices), 'static draw')
-- set transformation matrices as an instance vertex attribute (with divisor 1)
-- Note: we're cheating a little by taking the vao of the model's mesh(es) and
-- adding new vertex attributes to it. Normally you'd want to do this in a more
-- organized fashion, but for learning purposes this will do.
for _, mesh in ipairs(rock:meshes()) do
   local vao = mesh:vao()
   gl.bind_vertex_array(vao)
   -- set attribute pointers for matrix (4 times vec4)
   gl.enable_vertex_attrib_array(3)
   gl.vertex_attrib_pointer(3, 4,  'float', false, sizeof_mat4, 0)
   gl.enable_vertex_attrib_array(4)
   gl.vertex_attrib_pointer(4, 4,  'float', false, sizeof_mat4, sizeof_vec4)
   gl.enable_vertex_attrib_array(5)
   gl.vertex_attrib_pointer(5, 4,  'float', false, sizeof_mat4, 2*sizeof_vec4)
   gl.enable_vertex_attrib_array(6)
   gl.vertex_attrib_pointer(6, 4,  'float', false, sizeof_mat4, 3*sizeof_vec4)
   gl.vertex_attrib_divisor(3, 1)
   gl.vertex_attrib_divisor(4, 1)
   gl.vertex_attrib_divisor(5, 1)
   gl.vertex_attrib_divisor(6, 1)
   gl.unbind_vertex_array()
end

-- get the locations of the uniforms:
local loc = {} -- holds the locations for prog (indexed by the uniform variables names)
gl.use_program(prog)
loc.view = gl.get_uniform_location(prog, "view")
loc.projection = gl.get_uniform_location(prog, "projection")
loc.texture_diffuse1 = gl.get_uniform_location(prog, "texture_diffuse1")
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

   local projection = perspective(rad(45.0), SCR_WIDTH/SCR_HEIGHT, 0.1, 1000.0)
   local view = camera:view()

   -- draw planet
   gl.use_program(prog1)
   gl.uniform_matrix4f(loc1.projection, true, projection)
   gl.uniform_matrix4f(loc1.view, true, view)
   gl.uniform_matrix4f(loc1.model, true, translate(0.0, -3.0, 0.0)*scale(4.0))
   planet:draw(prog1)

   -- draw meteorites
   gl.use_program(prog)
   gl.uniform_matrix4f(loc.projection, true, projection)
   gl.uniform_matrix4f(loc.view, true, view)
   gl.active_texture(0)
   gl.uniformi(loc.texture_diffuse1, 0)

   for _, mesh in ipairs(rock:meshes()) do
      gl.bind_vertex_array(mesh:vao())
      gl.bind_texture('2d', mesh:textures()[1].id)
      gl.draw_elements('triangles', mesh:count(), 'uint', 0, amount)
      gl.unbind_vertex_array()
   end

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

