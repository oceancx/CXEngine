#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_objmesh = require("common.objmesh")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Normal map"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = rad(15), pi/8 -- rad, rad/s
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
   local c = 2.0
   projection = glmath.ortho(-0.4*c, 0.4*c, -0.3*c, 0.3*c, 0.1, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program({
   vertex="shaders/normalmap.vert",
   fragment="shaders/normalmap.frag"
})
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Ks",
    "Material.Shininess",
    "Light.Position",
    "Light.L",
    "Light.La",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(-1, .25, 2), vec3(0,0,0), vec3(0,1,0))
resize(window, W, H) -- creates projection

local function set_matrices(view, model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

gl.uniformf(loc["Light.L"], 1, 1, 1)
gl.uniformf(loc["Light.La"], .2, .2, .2)
gl.uniformf(loc["Material.Ks"], .2, .2, .2)
gl.uniformf(loc["Material.Shininess"], 1.0)

-- Generate the meshes
local ogre = new_objmesh("../media/bs_ears.obj", {texcoords=true, tangents=true})

-- Load diffuse and normal map textures
local diff_tex = texture.load_texture("../media/texture/ogre_diffuse.png");
local normal_tex = texture.load_texture("../media/texture/ogre_normalmap.png")
gl.active_texture(0)
gl.bind_texture('2d', diff_tex)
gl.active_texture(1)
gl.bind_texture('2d', normal_tex)

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

   gl.uniformf(loc["Light.Position"], view*vec4(10*cos(angle),1.0,10*sin(angle),1.0))
   local model = glmath.rotate(angle, 0, 1, 0)
   --local model = mat4()
   set_matrices(view, model)
   ogre:render()

   glfw.swap_buffers(window)
end

