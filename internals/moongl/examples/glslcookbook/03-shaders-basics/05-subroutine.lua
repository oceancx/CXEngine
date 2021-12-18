#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_teapot = require("common.teapot")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad

local TITLE = "Chapter 3 - Using a shader subroutine"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/3 -- rad, rad/s
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
   projection = glmath.perspective(rad(50.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local vertex_shader = "shaders/subroutine.vert"
local fragment_shader = "shaders/subroutine.frag"
local prog, vsh, fsh = gl.make_program('vertex', vertex_shader, 'fragment', fragment_shader)
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the indices of the subroutines
local ads_subroutine = gl.get_subroutine_index(prog, 'vertex', "phongModel")
local diffuse_subroutine = gl.get_subroutine_index(prog, 'vertex', "diffuseOnly")

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Kd",
    "Light.Ld",
    "Material.Ka",
    "Light.La",
    "Material.Ks",
    "Light.Ls",
    "Material.Shininess",
    "Light.Position",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}
local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(0,0,12), vec3(0,0,0), vec3(0,1,0))
resize(window, W, H) -- creates projection

local function set_matrices(model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

gl.uniformf(loc["Material.Kd"], 0.9, 0.5, 0.3)
gl.uniformf(loc["Light.Ld"], 1.0, 1.0, 1.0)
gl.uniformf(loc["Material.Ka"], 0.9, 0.5, 0.3)
gl.uniformf(loc["Light.La"], 0.4, 0.4, 0.4)
gl.uniformf(loc["Material.Ks"], 0.8, 0.8, 0.8)
gl.uniformf(loc["Light.Ls"], 1.0, 1.0, 1.0)
gl.uniformf(loc["Material.Shininess"], 100.0)

-- Generate the mesh
local teapot = new_teapot(13)

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(0.5, 0.5, 0.5, 1.0)

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

   local light_pos = vec4(0,0,0,1)
   gl.uniformf(loc["Light.Position"], light_pos)

   local model

   -- Render the first teapot with ADS subroutine:
   gl.uniform_subroutines('vertex', ads_subroutine)
   model =  glmath.translate(-3, -1.5, 0)*
            glmath.rotate(angle, 0, 1, 0)*
            glmath.rotate(rad(-90), 1, 0, 0)
   set_matrices(model)
   teapot:render()

   -- Render the second teapot with diffuse subroutine:
   gl.uniform_subroutines('vertex', diffuse_subroutine)
   model =  glmath.translate(3, -1.5, 0)*
            glmath.rotate(angle, 0, 1, 0)*
            glmath.rotate(rad(-90), 1, 0, 0)
   set_matrices(model)
   teapot:render()

   glfw.swap_buffers(window)
end

