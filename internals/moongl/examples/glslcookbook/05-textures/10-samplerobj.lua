#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Sampler objects"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/12 -- rad, rad/s
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
   projection = glmath.perspective(rad(60.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program({
   vertex = "shaders/samplerobj.vert", 
   fragment = "shaders/samplerobj.frag"
})
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Kd",
    "Material.Ks",
    "Material.Ka",
    "Material.Shininess",
    "Light.Position",
    "Light.Intensity",
--  "Light.La",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(0, .1, 6.0), vec3(0,0,0), vec3(0,1,0))
resize(window, W, H) -- creates projection

local function set_matrices(model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

gl.uniformf(loc["Light.Intensity"], 1, 1, 1)
--gl.uniformf(loc["Light.La"], .2, .2, .2)
gl.uniformf(loc["Light.Position"], 0.0,20.0,0.0,1.0)
gl.uniformf(loc["Material.Kd"], 0.9, 0.9, 0.9)
gl.uniformf(loc["Material.Ks"], 0.95, 0.95, 0.95)
gl.uniformf(loc["Material.Ka"], 0.1, 0.1, 0.1)
gl.uniformf(loc["Material.Shininess"], 100.0)

-- Generate the meshes
local plane = new_plane(10, 10, 1, 1)

-- A simple 128x128 checkerboard texture
local function checkerboard(w, h, checksize)
   local WHITE, BLACK = vec4(255, 255, 255, 255), vec4(0, 0, 0, 255)
   local data = {}
   for r = 0, h-1 do
      for c = 0, w-1 do
         data[r*w+c+1]= ((math.floor(c/checksize) + math.floor(r/checksize))%2 == 0) and BLACK or WHITE
      end
   end
   return gl.pack('ubyte', data)
end

local w, h, checksize = 128, 128, 4
local data = checkerboard(w, h, checksize)

-- Create the texture object
gl.active_texture(0)
local texid = gl.new_texture('2d')
gl.texture_storage('2d', 1, 'rgba8', w, h)
gl.texture_sub_image('2d', 0, 'rgba', 'ubyte', data, 0, 0, w, h)

-- Create some sampler objects
local linear_sampler, nearest_sampler = gl.gen_samplers(2)
-- Set up the nearest sampler
gl.sampler_parameter(nearest_sampler, 'mag filter', 'nearest')
gl.sampler_parameter(nearest_sampler, 'min filter', 'nearest')
-- Set up the linear sampler
gl.sampler_parameter(linear_sampler, 'mag filter', 'linear')
gl.sampler_parameter(linear_sampler, 'min filter', 'linear')

-- Bind texture object and sampler object to texture unit
gl.active_texture(0)
gl.bind_texture('2d', texid)

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(0.9, 0.9, 0.9, 1.0)

print("Press space to toggle animation on/off")

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

   -- Render
   gl.clear('color', 'depth')

   local rot = glmath.rotate(angle, 0, 1, 0)*glmath.rotate(rad(10.0), 1,0,0)

   local model = rot * glmath.translate(-5.01,0,0)
   set_matrices(model)
   gl.bind_sampler(0, nearest_sampler)
   plane:render()

   local model = rot * glmath.translate(5.01,0,0)
   set_matrices(model)
   gl.bind_sampler(0, linear_sampler)
   plane:render()

   glfw.swap_buffers(window)
end

