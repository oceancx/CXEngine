#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_quad = require("common.quad")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 6 - Multi-sample anti-aliasing"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('samples', 8)
local window = glfw.create_window(W, H, TITLE .." (on)")
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/2, pi/8 -- rad, rad/s
local animate = false
local msaa = true

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then
      animate = not animate
   elseif key == 'm' and action == 'press' then
      msaa = not msaa
      if msaa then gl.enable('multisample') else gl.disable('multisample') end
      glfw.set_window_title(window, TITLE ..(msaa and " (on)" or " (off)"))
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
local prog, vsh, fsh = gl.make_program('vertex', "shaders/centroid.vert",
                                       'fragment', "shaders/centroid.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection)
   local mv = view * model
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

-- Generate the meshes
local quad = new_quad()

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")
print("Press M to toggle MSAA on/off")

local t0 = glfw.now()
local model = glmath.rotate(rad(30.0), vec3(0.0,0.0,1.0))

gl.clear_color(.5, .5, .5, 1)
gl.enable('depth test')
local bufs = gl.get('sample buffers')
local samples = gl.get('samples')
print(fmt("MSAA: buffers = %d samples = %d", bufs, samples))
gl.enable('multisample')

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

   -- Render -----------------------------------------------
   gl.clear('color', 'depth')
   local view = glmath.look_at(vec3(3.0*cos(angle),0.0,3.0*sin(angle)), vec3(0,0,0), vec3(0,1,0))
   set_matrices(model, view, projection)
   quad:render()
   gl.finish()
   glfw.swap_buffers(window)
end

