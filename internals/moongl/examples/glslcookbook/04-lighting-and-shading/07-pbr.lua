#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local new_objmesh = require("common.objmesh")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 4 - Physically based rendering (PBR) shader"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/6 -- rad, rad/s
local animate = true

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
   projection = glmath.perspective(rad(60.0), w/h, 0.5, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local vertex_shader = "shaders/pbr.vert"
local fragment_shader = "shaders/pbr.frag"
local prog, vsh, fsh = gl.make_program('vertex', vertex_shader, 'fragment', fragment_shader)
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Rough",
    "Material.Metal",
    "Material.Color",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}
for i=0,2 do
   table.insert(uniforms, fmt("Light[%d].Position", i))
   table.insert(uniforms, fmt("Light[%d].L", i))
end

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(0, 4, 7), vec3(0,0,0), vec3(0,1,0))
resize(window, W, H) -- creates projection

local function set_matrices(model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

local function set_material(rough, metal, color)
   gl.uniformf(loc["Material.Rough"], rough) -- float
   gl.uniformb(loc["Material.Metal"], metal) -- boolean
   gl.uniformf(loc["Material.Color"], color) -- vec3
end

local light_pos = vec4(5.0, 5.0, 5.0, 1.0)

gl.uniformf(loc["Light[0].L"], 45, 45, 45)
gl.uniformf(loc["Light[0].Position"], view*light_pos)
gl.uniformf(loc["Light[1].L"], .3, .3, .3)
gl.uniformf(loc["Light[1].Position"], 0, .15, -1, 0)
gl.uniformf(loc["Light[2].L"], 45, 45, 45)
gl.uniformf(loc["Light[2].Position"], view*vec4(-7, 3, 7, 1))

-- Generate the meshes
local plane = new_plane(20, 20, 1, 1)
local mesh = new_objmesh("../media/spot/spot_triangulated.obj")

-- Prepare the cows' data
local cows = {}

-- Dielectric cows with varying roughness
local num_cows = 9 -- no. of dielectric cows
local cow_base_color = vec3(0.1, 0.33, 0.17)
for i = 1, num_cows do
   cows[i] = {
      pos = vec3((i-1)*10/(num_cows-1)-5, 0, 0),
      rough = i/num_cows,
      metal = false,
      color = cow_base_color,
   }
end

-- Metal cows
local metal_rough = .43
cows[#cows+1] = { pos=vec3(-3.0,0,3), rough=.43, metal=true, color={  1, .71, .29}} -- gold
cows[#cows+1] = { pos=vec3(-1.5,0,3), rough=.43, metal=true, color={.95, .64, .54}} -- copper
cows[#cows+1] = { pos=vec3( 0.0,0,3), rough=.43, metal=true, color={.91, .92, .92}} -- aluminum
cows[#cows+1] = { pos=vec3( 1.5,0,3), rough=.43, metal=true, color={.542, .497, .449}} -- titanium
cows[#cows+1] = { pos=vec3( 3.0,0,3), rough=.43, metal=true, color={.95, .93, .88}} -- silver

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
      light_pos = vec4(7*cos(angle), 3.0, 7*sin(angle), 1.0)
   end

   -- Render
   gl.clear('color', 'depth')

   gl.uniformf(loc["Light[0].Position"], view * light_pos)

   -- Draw floor
   set_material(.9, false, vec3(.2, .2, .2))
   local model = glmath.translate(0, -.75, 0)
   set_matrices(model)
   plane:render()

   -- Draw the cows
   local rotate = glmath.rotate(rad(180), 0, 1, 0)
   for _, cow in ipairs(cows) do
      set_material(cow.rough, cow.metal, cow.color)
      set_matrices(glmath.translate(cow.pos)*rotate)
      mesh:render()
   end

   glfw.swap_buffers(window)
end

