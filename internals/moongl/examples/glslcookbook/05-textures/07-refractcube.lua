#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_teapot = require("common.teapot")
local new_skybox = require("common.skybox")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Refraction with a cube map"
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
   projection = glmath.perspective(rad(50.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader programs
local prog, vsh, fsh = gl.make_program({
   vertex = "shaders/cubemap_refract.vert",
   fragment = "shaders/cubemap_refract.frag"
})
gl.delete_shaders(vsh, fsh)
local skyprog, vsh, fsh = gl.make_program({
   vertex = "shaders/skybox.vert", 
   fragment = "shaders/skybox.frag"
})
gl.delete_shaders(vsh, fsh)


-- Get the locations of the uniform variables
local uniforms = {
   "WorldCameraPosition",
   "Material.Eta",
   "Material.ReflectionFactor",
   "ModelMatrix",
   "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

local skyloc = {}
skyloc["MVP"] = gl.get_uniform_location(skyprog, "MVP")


-- Initialize the uniform variables
resize(window, W, H) -- creates projection

-- Generate the meshes
local teapot = new_teapot(14)
local sky = new_skybox(100.0)

-- Load the textures
local cube_tex = texture.load_hdr_cube_map("../media/texture/cube/pisa-hdr/pisa")
--local cube_tex = texture.load_cube_map("../media/texture/cube/pisa/pisa", "png")
gl.active_texture(0)
gl.bind_texture('cube map', cube_tex)
gl.active_texture(1)
gl.bind_texture('cube map', cube_tex)

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
   local pv = projection*view

   -- Draw sky
   gl.use_program(skyprog)
   local model = mat4()
   gl.uniform_matrix4f(skyloc["MVP"], true, pv*model)
   sky:render()

   -- Draw scene
   gl.use_program(prog)
   gl.uniformf(loc["WorldCameraPosition"], camera_pos)
   gl.uniformf(loc["Material.Eta"], .94)
   gl.uniformf(loc["Material.ReflectionFactor"], .1)
   local model = glmath.translate(0.0,-1.0,0.0)*glmath.rotate(rad(-90), 1.0,0.0,0.0)
   gl.uniform_matrix4f(loc["ModelMatrix"], true, model)
   gl.uniform_matrix4f(loc["MVP"], true, pv*model)
   teapot:render()

   glfw.swap_buffers(window)
end

