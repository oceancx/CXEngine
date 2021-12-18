#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_cube = require("common.cube")
local new_sphere = require("common.sphere")
local new_quad = require("common.quad")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 6 - Order-independent transparency"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('resizable', false) -- @@
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = rad(210), pi/8 -- rad, rad/s
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
   projection = glmath.perspective(rad(50), W/H, 1.0, 1000.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', "shaders/oit.vert",
                                       'fragment', "shaders/oit.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "MaxNodes",
   "LightPosition",
   "LightIntensity",
   "Kd",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
}

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

gl.uniformf(loc["LightIntensity"], .9, .9, .9)
gl.uniformf(loc["LightPosition"], 0, 0, 0, 1)

-- Generate the meshes
local cube = new_cube()
local sphere = new_sphere(1, 40, 40)
local quad = new_quad()

-- Init shader storage --------------------------------------------------------
local counter_buf, linkedlist_buf, clear_buf = gl.gen_buffers(3)
local headptr_tex = gl.gen_textures()
do
   -- Our atomic counter
   gl.bind_buffer_base('atomic counter', 0, counter_buf)
   gl.buffer_data('atomic counter', gl.sizeof('uint'), 'dynamic draw')
   -- The texture for the head pointers
   gl.bind_texture('2d', headptr_tex)
   gl.texture_storage('2d', 1, 'r32ui', W, H)
   gl.bind_image_texture(0, headptr_tex, 0, false, 0, 'read write', 'r32ui')
   -- The buffer of linked lists
   local maxNodes = 20 * W * H
   local nodeSize = 5*gl.sizeof('float')+gl.sizeof('uint') -- sizeof(NodeType) in frag. shader
   gl.bind_buffer_base('shader storage', 0, linkedlist_buf)
   gl.buffer_data('shader storage', maxNodes*nodeSize, 'dynamic draw')
   gl.uniformui(loc["MaxNodes"], maxNodes)
   -- Create the clear-buffer for the head pointer texture
   local clearvalue = string.rep(gl.pack('uint', 0xffffffff), W*H)
   gl.bind_buffer('pixel unpack', clear_buf)
   gl.buffer_data('pixel unpack', clearvalue, 'static copy')
end
local zero = gl.pack('uint', {0}) -- clear value for counter buffer

-- Get the indices of the subroutines for the 2 passes
local pass1_subroutine = gl.get_subroutine_index(prog, 'fragment', "pass1")
local pass2_subroutine = gl.get_subroutine_index(prog, 'fragment', "pass2")

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")

local model
gl.clear_color(1, 1, 1, 1)
gl.enable('depth test')
--gl.enable('blend')
gl.blend_func('src alpha', 'one minus src alpha')

local sphere_model = {}
do
   local scale = glmath.scale(0.45)
   for i = 0, 6 do
      for j = 0, 6 do
         for k = 0, 6 do
            if (i+j+k)%2 == 0 then
               sphere_model[#sphere_model+1]  = glmath.translate(i-3, j-3, k-3)*scale
            end
         end
      end
   end
end
   
local pos, scale = 1.75, glmath.scale(2.0)
local cube_model = {
   glmath.translate(-pos, -pos,  pos)*scale,
   glmath.translate(-pos, -pos, -pos)*scale,
   glmath.translate(-pos,  pos,  pos)*scale,
   glmath.translate(-pos,  pos, -pos)*scale,
   glmath.translate( pos,  pos,  pos)*scale,
   glmath.translate( pos,  pos, -pos)*scale,
   glmath.translate( pos, -pos,  pos)*scale,
   glmath.translate( pos, -pos, -pos)*scale,
}

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
   -- Clear buffers
   gl.bind_buffer_base('atomic counter', 0, counter_buf)
   gl.buffer_sub_data('atomic counter', 0, zero)
   gl.bind_buffer('pixel unpack', clear_buf)
   gl.bind_texture('2d', headptr_tex)
   gl.texture_sub_image('2d', 0, 'red integer', 'uint', 0, 0, 0, W, H)

   -- Pass 1 ---------------------------------------
   gl.uniform_subroutines('fragment', pass1_subroutine)
   gl.clear('color', 'depth')
   local view = glmath.look_at(vec3(11*cos(angle),2,11*sin(angle)), vec3(0,0,0), vec3(0,1,0))
   gl.depth_mask(false)
   -- Draw scene
   gl.uniformf(loc["Kd"], .2, .2, .9, .55)
   for _, model in ipairs(sphere_model) do
      set_matrices(model, view, projection)
      sphere:render()
   end
   gl.uniformf(loc["Kd"], .9, .2, .2, .4)
   for _, model in ipairs(cube_model) do
      set_matrices(model, view, projection)
      cube:render()
   end

   gl.finish() --@@  gl.flush()
   -- Pass 1 ---------------------------------------
   gl.memory_barrier('shader storage')
   gl.uniform_subroutines('fragment', pass2_subroutine)
   gl.clear('color', 'depth')
   set_matrices(mat4(), mat4(), mat4())
   quad:render()

   glfw.swap_buffers(window)

end


