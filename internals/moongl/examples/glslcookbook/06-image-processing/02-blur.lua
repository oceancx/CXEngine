#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local new_torus = require("common.torus")
local new_teapot = require("common.teapot")
local new_quad = require("common.quad")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 6 - Gaussian blur"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/4, pi/8 -- rad, rad/s
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
local prog, vsh, fsh = gl.make_program('vertex', "shaders/blur.vert", 'fragment', "shaders/blur.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Pass",
   "Material.Kd",
   "Material.Ks",
   "Material.Ka",
   "Material.Shininess",
   "Light.Position",
   "Light.L",
   "Light.La",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
}

for i = 0, 4 do table.insert(uniforms, "Weight["..i.."]") end

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

gl.uniformf(loc["Light.L"], 1, 1, 1)
gl.uniformf(loc["Light.La"], .2, .2, .2)
gl.uniformf(loc["Light.Position"], 0.0,0.0,0.0,1.0)

-- Compute and sum the weights
local function gauss(x, sigma2)
   return math.exp(-(x^2)/(2*sigma2)) / (2*pi*sigma2)
end
local weights = {}
local sum = 0
local sigma2 = 8.0
weights[0] = gauss(0, sigma2)
for i = 1, 4 do
   weights[i] = gauss(i, sigma2)
   sum = sum + 2*weights[i]
end

-- Normalize the weights and set the uniform
for i = 0, 4 do
   gl.uniformf(loc["Weight["..i.."]"], weights[i]/sum)
end

-- Generate the meshes
local plane = new_plane(50, 50, 1, 1)
local teapot = new_teapot(14)
local torus = new_torus(.7*1.5, .3*1.5, 50, 50)
local quad = new_quad()

-- Setup the fbos ----------------------------

-- Generate and bind the framebuffer
local render_fbo = gl.new_framebuffer('draw read')
-- Create the texture object
local render_tex = gl.new_texture('2d')
gl.texture_storage('2d', 1, 'rgba8', W, H)
gl.texture_parameter('2d', 'min filter', 'nearest')
gl.texture_parameter('2d', 'mag filter', 'nearest')
gl.texture_parameter('2d', 'max level', 0)
-- Bind the texture to the FBO
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', render_tex, 0)
-- Create the depth buffer
local depth_buf = gl.new_renderbuffer('renderbuffer')
gl.renderbuffer_storage('renderbuffer', 'depth component', W, H)
-- Bind the depth buffer to the FBO
gl.framebuffer_renderbuffer('draw read', 'depth attachment', 'renderbuffer', depth_buf)
-- Set the targets for the fragment output variables
gl.draw_buffers('color attachment 0')
-- Unbind the framebuffer, and revert to default framebuffer
gl.unbind_framebuffer('draw read')

-- Generate and bind the framebuffer
local intermediate_fbo = gl.new_framebuffer('draw read')
-- Create the texture object
gl.active_texture(0)  -- Use texture unit 0
local intermediate_tex = gl.new_texture('2d')
gl.texture_storage('2d', 1, 'rgba8', W, H)
gl.texture_parameter('2d', 'min filter', 'nearest')
gl.texture_parameter('2d', 'mag filter', 'nearest')
gl.texture_parameter('2d', 'max level', 0)
-- Bind the texture to the FBO
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', intermediate_tex, 0)
-- No depth buffer needed for this FBO
-- Set the targets for the fragment output variables
gl.draw_buffers('color attachment 0')
gl.unbind_framebuffer('draw read')

-- Event loop -----------------------------------------------------------------

gl.clear_color(0, 0, 0, 1)

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

   -- Render (pass 1) --------------------------------------
   gl.uniformi(loc["Pass"], 1)
   gl.bind_framebuffer('draw read', render_fbo)
   gl.enable('depth test')
   gl.clear('color', 'depth')
    
   local view = glmath.look_at(vec3(7*cos(angle),4.0,7*sin(angle)), vec3(0,0,0), vec3(0,1,0))
   local projection = glmath.perspective(rad(60.0), W/H, 0.3, 100.0)

   set_material(vec3(.9, .9, .9), vec3(.95, .95, .95), vec3(.1, .1, .1), 100.0)
   local model = glmath.rotate(rad(-90.0), 1, 0, 0)
   set_matrices(model, view, projection)
   teapot:render()

   set_material(vec3(.4, .4, .4), vec3(0, 0, 0), vec3(.1, .1, .1), 1.0)
   local model = glmath.translate(0.0,-0.75,0.0)
   set_matrices(model, view, projection)
   plane:render()

   set_material(vec3(.9, .5, .2), vec3(.95, .95, .95), vec3(.1, .1, .1), 100.0)
   local model = glmath.translate(1.0,1.0,3.0)*glmath.rotate(rad(90.0), 1, 0, 0)
   set_matrices(model, view, projection)
   torus:render()

   -- Render (pass 2) --------------------------------------

   gl.uniformi(loc["Pass"], 2)
   gl.bind_framebuffer('draw read', intermediate_fbo)
   gl.active_texture(0)
   gl.bind_texture('2d', render_tex)
   gl.disable('depth test')
   gl.clear('color')
   set_matrices(mat4(), mat4(), mat4())
   -- Render the full-screen quad
   quad:render()

   -- Render (pass 3) --------------------------------------

   gl.uniformi(loc["Pass"], 3)
   gl.unbind_framebuffer('draw read')
   gl.active_texture(0)
   gl.bind_texture('2d', intermediate_tex)
   gl.clear('color')
   set_matrices(mat4(), mat4(), mat4())
   -- Render the full-screen quad
   quad:render()

   glfw.swap_buffers(window)
end


