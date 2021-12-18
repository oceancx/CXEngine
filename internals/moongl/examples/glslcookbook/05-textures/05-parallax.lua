#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Parallax mapping"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = rad(65), pi/24 -- rad, rad/s
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
   projection = glmath.perspective(rad(35.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Parse command line options and create the shader program
local USAGE = "\nUsage: "..arg[0].." <option>\n"..[[
Options:
        -1   Parallax mapping
        -2   Steep parallax mapping
]]

local option = arg[1]
local vertex_shader, fragment_shader

if not option then
   print(USAGE)
   os.exit(true)
elseif option == '-1' then
   vertex_shader = "shaders/parallax.vert"
   fragment_shader = "shaders/parallax.frag"
elseif option == '-2' then
   vertex_shader = "shaders/steep-parallax.vert"
   fragment_shader = "shaders/steep-parallax.frag"
   glfw.set_window_title(window, TITLE.." (steep)")
else
   print("Invalid option")
   print(USAGE)
   os.exit(true)
end

local prog, vsh, fsh = gl.make_program('vertex', vertex_shader, 'fragment', fragment_shader)
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
resize(window, W, H) -- creates projection

local function set_matrices(view, model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

gl.uniformf(loc["Light.L"], .7, .7, .7)
gl.uniformf(loc["Light.La"], .01, .01, .01)
gl.uniformf(loc["Material.Ks"], .7, .7, .7)
gl.uniformf(loc["Material.Shininess"], 40.0)

-- Generate the meshes
local plane = new_plane(8, 8, 1, 1)

-- Load the textures
local normal_map = texture.load_texture("../media/texture/mybrick/mybrick-normal.png")
local height_map = texture.load_texture("../media/texture/mybrick/mybrick-height.png")
local color_map = texture.load_texture("../media/texture/mybrick/mybrick-color.png")
gl.active_texture(0)
gl.bind_texture('2d', color_map)
gl.texture_parameter('2d', 'min filter', 'linear')
gl.active_texture(1)
gl.bind_texture('2d', normal_map)
gl.texture_parameter('2d', 'min filter', 'linear')
gl.active_texture(2)
gl.bind_texture('2d', height_map)
gl.texture_parameter('2d', 'min filter', 'linear')

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.enable('multisample')
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

   local view = glmath.look_at(vec3(-1, 0, 8), vec3(-1,0,0), vec3(0,1,0))
   gl.uniformf(loc["Light.Position"], view*vec4(2.0, 2.0, 1.0, 1.0))
   local model = glmath.rotate(angle--[[rad(65)--]], 0, 1, 0) * glmath.rotate(rad(90), 1, 0, 0)
   set_matrices(view, model)
   plane:render()

   glfw.swap_buffers(window)
end

