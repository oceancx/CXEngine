#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_objmesh = require("common.objmesh")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 6 - Gamma correction"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE .." (on)")
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/2, pi/8 -- rad, rad/s
local animate = false
local gamma_correction = true

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then
      animate = not animate
   elseif key == 'g' and action == 'press' then
      gamma_correction = not gamma_correction
      glfw.set_window_title(window, TITLE ..(gamma_correction and " (on)" or " (off)"))
   end
end)

local projection
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   local c = 2.5
   projection = glmath.ortho(-0.4*c, 0.4*c, -0.3*c, 0.3*c, 0.1, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', "shaders/gamma.vert",
                                       'fragment', "shaders/gamma.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Gamma",
   "Light.Intensity",
   "Light.Position",
   "Material.Kd",
   "Material.Ks",
   "Material.Ka",
   "Material.Shininess",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection)
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

set_material(vec3(1, 1, 1), vec3(0, 0, 0), vec3(0, 0, 0), 100.0)
gl.uniformf(loc["Light.Intensity"], 1.0, 1.0, 1.0)

-- Generate the meshes
local ogre = new_objmesh("../media/bs_ears.obj")

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")
print("Press G to toggle gamma correction on/off")

local t0 = glfw.now()
local model = mat4()

gl.clear_color(.5, .5, .5, 1)
gl.enable('depth test')

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
   gl.uniformf(loc["Gamma"], gamma_correction and 2.2 or 0.0)

   -- Render -----------------------------------------------
   gl.clear('color', 'depth')
   local view = glmath.look_at(vec3(3.0*cos(angle),0.0,3.0*sin(angle)), vec3(0,0,0), vec3(0,1,0))
   gl.uniformf(loc["Light.Position"], view*vec4(10.0,0.0,0.0,1.0))
   set_matrices(model, view, projection)
   ogre:render()
   gl.finish()
   glfw.swap_buffers(window)
end

