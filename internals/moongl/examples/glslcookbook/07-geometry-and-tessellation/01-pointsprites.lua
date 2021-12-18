#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 7 - Point sprites"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/8 -- rad, rad/s
local animate = false

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then
      animate = not animate
   end
end)

local projection
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   projection = glmath.perspective(rad(60), W/H, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, gsh, fsh = gl.make_program(
   'vertex', "shaders/pointsprite.vert",
   'geometry', "shaders/pointsprite.geom",
   'fragment', "shaders/pointsprite.frag")

gl.delete_shaders(vsh, gsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "SpriteTex",
   "Size2",
   "ModelViewMatrix",
   "ProjectionMatrix",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection)
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, view * model)
   gl.uniform_matrix4f(loc["ProjectionMatrix"], true, projection)
end

-- Load the texture
local texid = texture.load_texture("../media/texture/flower.png")
gl.active_texture(0)
gl.bind_texture('2d', texid)
gl.uniformi(loc["SpriteTex"], 0)

-- Half size of the sprite quad
gl.uniformf(loc["Size2"], .15)

-- Set up the buffer with the locations for the point sprites
local num_sprites = 50
local sprites = gl.new_vertex_array()
do 
   math.randomseed(os.time())
   local function rand() return math.random()*2-1 end -- random no. in [-1,1]
   local locations = {}
   for i = 0, num_sprites do 
      locations[#locations + 1] = vec3(rand(), rand(), rand())
   end
   local vbo = gl.new_buffer('array')
   gl.buffer_data('array', gl.pack('float', locations), 'static draw')
   gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
   gl.enable_vertex_attrib_array(0)
   gl.unbind_vertex_array()
end

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")

gl.clear_color(.5, .5, .5, 1)
gl.enable('depth test')

local model = mat4()

local t0 = glfw.now()

while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Update
   local t = glfw.now()
   local dt = t - t0
   t0 = t
   if animate then
      angle = angle + speed*dt
      if angle >= 2*pi then angle = angle - 2*pi end
   end

   local view = glmath.look_at(vec3(0,0,3)*cos(angle), vec3(0,0,0), vec3(0,1,0))
   gl.clear('color', 'depth')
   set_matrices(model, view, projection)
   gl.bind_vertex_array(sprites)
   gl.draw_arrays('points', 0, num_sprites)
   gl.finish()
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end
