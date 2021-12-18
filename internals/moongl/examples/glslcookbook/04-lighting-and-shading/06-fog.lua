#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local new_teapot = require("common.teapot")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 4 - Fog"
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
local vertex_shader = "shaders/fog.vert"
local fragment_shader = "shaders/fog.frag"
local prog, vsh, fsh = gl.make_program('vertex', vertex_shader, 'fragment', fragment_shader)
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Kd",
    "Material.Ka",
    "Material.Ks",
    "Material.Shininess",
    "Light.L",
    "Light.La",
    "Light.Position",
    "Fog.MaxDist",
    "Fog.MinDist",
    "Fog.Color",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(0,4,6), vec3(0,2,0), vec3(0,1,0))
resize(window, W, H) -- creates projection

local function set_matrices(model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

local function set_material(kd, ks, ka, shininess)
   gl.uniformf(loc["Material.Kd"], kd)
   gl.uniformf(loc["Material.Ks"], ks)
   gl.uniformf(loc["Material.Ka"], ka)
   gl.uniformf(loc["Material.Shininess"], shininess)
end

gl.uniformf(loc["Light.L"], .9, .9, .9)
gl.uniformf(loc["Light.La"], .2, .2, .2)
gl.uniformf(loc["Fog.MaxDist"], 30.0)
gl.uniformf(loc["Fog.MinDist"], 1.0)
gl.uniformf(loc["Fog.Color"], .5, .5, .5)

-- Generate the meshes
local plane = new_plane(50.0, 50.0, 1, 1)
local teapot = new_teapot(14)

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(0.5, 0.5, 0.5, 1.0) -- better be the same color as the fog

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

   local light_pos = vec4(10*cos(angle), 10, 10*sin(angle), 1)
   gl.uniformf(loc["Light.Position"], view*light_pos)

   set_material(vec3(0.9, 0.5, 0.3), vec3(0, 0, 0), vec3(0.9, 0.5, 0.3)*.3, 180.0)
   local rotate = glmath.rotate(rad(-90), 1, 0, 0)
   local dist = 0.0
   for i = 1, 4 do
      local model = glmath.translate(.6*dist-1.0, 0.0, -dist)*rotate
      set_matrices(model)
      teapot:render()
      dist = dist + 7.0
   end

   set_material(vec3(0.7, 0.7, 0.7), vec3(0, 0, 0), vec3(0.2, 0.2, 0.2), 180.0)
   local model = mat4()
   set_matrices(model)
   plane:render()

   glfw.swap_buffers(window)
end

