#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_teapot = require("common.teapotpatch")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local max = math.max
local fmt = string.format
local printf = function(...) io.write(fmt(...)) end

local TITLE = "Chapter 7 - Tessellating based on depth"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/3, pi/8 -- rad, rad/s
local animate = false

local mintesslevel, maxtesslevel = 2, 15 -- tessellation level
glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then animate = not animate
   elseif key == 'down' and action ~= 'release' then mintesslevel = max(mintesslevel-1, 1)
   elseif key == 'up' and action ~= 'release' then mintesslevel = mintesslevel+1
   elseif key == 'left' and action ~= 'release' then maxtesslevel = max(maxtesslevel-1, 1)
   elseif key == 'right' and action ~= 'release' then maxtesslevel = maxtesslevel+1
   end
end)

local projection, viewport
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   projection = glmath.perspective(rad(60.0), w/h, 0.3, 30.0)
   viewport = mat4(w/2,   0,  0, w/2,
                     0, h/2,  0, h/2,
                     0,   0,  1,   0,
                     0,   0,  0,   1)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, gsh, tcsh, tesh, fsh = gl.make_program(
   'vertex', "shaders/tessteapotdepth.vert",
   'geometry', "shaders/tessteapotdepth.geom",
   'tess control', "shaders/tessteapotdepth.tesc",
   'tess evaluation', "shaders/tessteapotdepth.tese",
   'fragment', "shaders/tessteapotdepth.frag")

gl.delete_shaders(vsh, gsh, tcsh, tesh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "MinTessLevel",
   "MaxTessLevel",
   "MaxDepth",
   "MinDepth",
   "LineWidth",
   "LineColor",
   "LightPosition",
   "LightIntensity",
   "Kd",
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
   --local normal_mv = mat3(mv):inv():transpose() --@@
   local normal_mv = mat3(mv):transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
   gl.uniform_matrix4f(loc["ViewportMatrix"], true, viewport)
end

gl.uniformf(loc["MaxDepth"], 20.0)
gl.uniformf(loc["MinDepth"], 2.0)
gl.uniformf(loc["LineWidth"], .8)
gl.uniformf(loc["LineColor"], .05,0,.05,1.0)
gl.uniformf(loc["LightPosition"], 0,0,0,1)
gl.uniformf(loc["LightIntensity"], 1.0,1.0,1.0)
gl.uniformf(loc["Kd"], .9, .9, 1.0)

-- Generate the meshes
local teapot = new_teapot()

-- Set the number of vertices per patch.  IMPORTANT!!
gl.patch_parameter('vertices', 16)
printf("Max patch vertices: %d\n", gl.get("max patch vertices"))

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(.5, .5, .5, 1)

print("Press space to toggle animation on/off")
print("Press up/down/left/right to increase/decrease the tessellation levels")

local t0 = glfw.now()

while not glfw.window_should_close(window) do
   glfw.poll_events()

   local t = glfw.now()
   local dt = t - t0
   t0 = t
   if animate then
      angle = angle + speed*dt
      if angle >= 2*pi then angle = angle - 2*pi end
   end

   gl.uniformi(loc["MinTessLevel"], mintesslevel)
   gl.uniformi(loc["MaxTessLevel"], maxtesslevel)
   glfw.set_window_title(window, TITLE..
      " (tesslevel: min="..mintesslevel..", max="..maxtesslevel..")")
 
   gl.clear('color', 'depth')
   local camera_pos = vec3(0.0, 1.0, 6.25)
   local view = glmath.look_at(camera_pos, vec3(0,0,0), vec3(0,1,0))
   local rotate = glmath.rotate(angle, 0, 1, 0)*glmath.rotate(-pi/2, 1, 0, 0)

   local model = glmath.translate(-2.0,0.0,0.0)*glmath.translate(0.0,-1.5,0.0)*rotate
   set_matrices(model, view, projection, viewport)
   teapot:render()

   local model = glmath.translate(2.0,0.0,-5.0)*glmath.translate(0.0,-1.5,0.0)*rotate
   set_matrices(model, view, projection, viewport)
   teapot:render()

   local model = glmath.translate(7.0,0.0,-10.0)*glmath.translate(0.0,-1.5,0.0)*rotate
   set_matrices(model, view, projection, viewport)
   teapot:render()

   local model = glmath.translate(17.0,0.0,-20.0)*glmath.translate(0.0,-1.5,0.0)*rotate
   set_matrices(model, view, projection, viewport)
   teapot:render()
   gl.finish()

   glfw.swap_buffers(window)
end

