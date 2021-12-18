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

local TITLE = "Chapter 7 - Wireframe"
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

local projection, viewport
local function resize(window, w, h)
   W, H = w, h
   local c = 1.5
   projection = glmath.ortho(-0.4*c, 0.4*c, -0.3*c, 0.3*c, 0.1, 100.0)
   local sx, sy, ws, hs, ns, fs = 0, 0, w, h, 0, 1
   gl.viewport(sx, sy, ws, hs)
   -- viewport transform (NDC to screen space):
   viewport = mat4(ws/2,   0,      0,      sx + ws/2,
                     0,   hs/2,    0,      sy + hs/2,
                     0,    0,   (fs-ns)/2, (ns+fs)/2,
                     0,    0,      0,          1    )
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, gsh, fsh = gl.make_program(
   'vertex', "shaders/shadewire.vert",
   'geometry', "shaders/shadewire.geom",
   'fragment', "shaders/shadewire.frag")
gl.delete_shaders(vsh, gsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Line.Width",
   "Line.Color",
   "Light.Position",
   "Light.Intensity",
   "Material.Kd",
   "Material.Ka",
   "Material.Ks",
   "Material.Shininess",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
   "ViewportMatrix",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection, viewport)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
   gl.uniform_matrix4f(loc["ViewportMatrix"], true, viewport)
end

gl.uniformf(loc["Line.Width"], 0.75)
gl.uniformf(loc["Line.Color"], 0.05, 0.0, 0.05, 1.0)
gl.uniformf(loc["Light.Position"], 0.0, 0.0, 0.0, 1.0)
gl.uniformf(loc["Light.Intensity"], 1.0, 1.0, 1.0)
gl.uniformf(loc["Material.Kd"], 0.7, 0.7, 0.7)
gl.uniformf(loc["Material.Ka"], 0.2, 0.2, 0.2)
gl.uniformf(loc["Material.Ks"], 0.8, 0.8, 0.8)
gl.uniformf(loc["Material.Shininess"], 100.0)
   
-- Generate the meshes
local ogre = new_objmesh("../media/bs_ears.obj")

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")

gl.clear_color(.5, .5, .5, 1)
gl.enable('depth test')

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

   gl.clear('color', 'depth')
   local model = glmath.translate(0, 0, -5)*glmath.rotate(angle, 0, 1 ,0)
   local view = glmath.look_at(vec3(0, 0, 3), vec3(0,0,0), vec3(0,1,0))
   set_matrices(model, view, projection, viewport)
   ogre:render()
   gl.finish()

   glfw.swap_buffers(window)
end

