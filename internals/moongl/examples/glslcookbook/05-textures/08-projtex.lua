#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local new_teapot = require("common.teapot")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Projected texture"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/2, pi/8 -- rad, rad/s
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
   projection = glmath.perspective(rad(50.0), w/h, 0.3, 1000.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program({
   vertex = "shaders/projtex.vert",
   fragment = "shaders/projtex.frag"
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
   "Light.L",
   "Light.La",
   "ProjectorMatrix",
   "ModelMatrix",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(view, model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelMatrix"], true, model)
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

gl.uniformf(loc["Light.L"], 1, 1, 1)
gl.uniformf(loc["Light.La"], .2, .2, .2)
gl.uniformf(loc["Light.Position"], 0.0,0.0,0.0,1.0)

local proj_view = glmath.look_at(vec3(2.0,5.0,5.0), vec3(-2.0,-4.0,0.0), vec3(0.0,1.0,0.0))
local proj_proj = glmath.perspective(rad(30.0), 1.0, 0.2, 1000.0)
local bias = glmath.translate(0.5, 0.5, 0.5)*glmath.scale(.5)
gl.uniform_matrix4f(loc["ProjectorMatrix"], true, bias*proj_proj*proj_view)

-- Generate the meshes
local teapot = new_teapot(14)
local plane = new_plane(100, 100, 1, 1)

-- Load the texture
local flower_tex = texture.load_texture("../media/texture/flower.png")
gl.active_texture(0)
gl.bind_texture('2d', flower_tex)
gl.texture_parameter('2d', 'wrap s', 'clamp to border')
gl.texture_parameter('2d', 'wrap t', 'clamp to border')

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

   local camera_pos = vec3(7*cos(angle), 2.0, 7*sin(angle))
   local view = glmath.look_at(camera_pos, vec3(0,0,0), vec3(0,1,0))

   set_material(vec3(.5, .2, .1), vec3(.95, .95, .95), vec3(.1, .1, .1), 100.0)
   local model = glmath.translate(0, -1, 0)*glmath.rotate(rad(-90), 1, 0, 0)
   set_matrices(view, model)
   teapot:render()

   set_material(vec3(.4, .4, .4), vec3(0, 0, 0), vec3(.1, .1, .1), 1.0)
   local model = glmath.translate(0, -.75, 0)
   set_matrices(view, model)
   plane:render()

   glfw.swap_buffers(window)
end

