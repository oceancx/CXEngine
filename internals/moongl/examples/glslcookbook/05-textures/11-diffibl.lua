#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_objmesh = require("common.objmesh")
local new_skybox = require("common.skybox")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Diffuse image based lighting"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/2, pi/6 -- rad, rad/s
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
   vertex = "shaders/diffuseibl.vert", 
   fragment = "shaders/diffuseibl.frag"
})
gl.delete_shaders(vsh, fsh)
local sbprog, vsh, fsh = gl.make_program({
   vertex = "shaders/skybox.vert", 
   fragment = "shaders/skybox.frag"
})
gl.delete_shaders(vsh, fsh)

-- Get the locations of the uniform variables
local uniforms = {
    "CamPos",
    "ModelMatrix",
    "MVP",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

local sbuniforms = {
    "MVP",
}

local sbloc = {}
for _,name in ipairs(sbuniforms) do sbloc[name] = gl.get_uniform_location(sbprog, name) end


-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(view, model, sb)
   local mv = view * model
   if sb then
      gl.uniform_matrix4f(sbloc["MVP"], true, projection * mv)
   else
      gl.uniform_matrix4f(loc["ModelMatrix"], true, model)
      gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
   end
end

-- Generate the meshes
local spot = new_objmesh("../media/spot/spot_triangulated.obj")
local skybox = new_skybox()

-- Load the textures
local diffCube = texture.load_hdr_cube_map("../media/texture/cube/grace-diffuse/grace-diffuse")
local cube = texture.load_hdr_cube_map("../media/texture/cube/grace/grace")
local colorTex = texture.load_texture("../media/spot/spot_texture.png")

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

   local cam_pos = vec3(4*cos(angle), 0, 4*sin(angle))
   local view = glmath.look_at(cam_pos, vec3(0,0,0), vec3(0,1,0))

   -- Render
   gl.clear('color', 'depth')

   local model = mat4()
   gl.active_texture(1)
   gl.bind_texture('cube map', cube)
   gl.use_program(sbprog)
   set_matrices(view, model, true)
   skybox:render()

   gl.active_texture(0)
   gl.bind_texture('cube map', diffCube)
   gl.active_texture(1)
   gl.bind_texture('2d', colorTex)
   gl.use_program(prog)
   gl.uniformf(loc["CamPos"], cam_pos)
   local model = glmath.rotate(rad(180.0), 0, 1, 0)
   set_matrices(view, model, false)
   spot:render()

   glfw.swap_buffers(window)
end

