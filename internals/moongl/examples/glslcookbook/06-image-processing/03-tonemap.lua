#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_plane = require("common.plane")
local new_sphere = require("common.sphere")
local new_teapot = require("common.teapot")
local new_quad = require("common.quad")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 6 - Tone mapping"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/8 -- rad, rad/s
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
   --@@projection = glmath.perspective(rad(60.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', "shaders/tonemap.vert", 'fragment', "shaders/tonemap.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Pass",
   "AveLum",
   "Material.Kd",
   "Material.Ks",
   "Material.Ka",
   "Material.Shininess",
   "ModelViewMatrix",
   "NormalMatrix",
   "MVP",
}

for i = 0, 2 do 
   table.insert(uniforms, "Lights["..i.."].L")
   table.insert(uniforms, "Lights["..i.."].La")
   table.insert(uniforms, "Lights["..i.."].Position")
end

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

gl.uniformf(loc["Lights[0].L"], 5, 5, 5)
gl.uniformf(loc["Lights[1].L"], 5, 5, 5)
gl.uniformf(loc["Lights[2].L"], 5, 5, 5)
gl.uniformf(loc["Lights[0].La"], .2, .2, .2)
gl.uniformf(loc["Lights[1].La"], .2, .2, .2)
gl.uniformf(loc["Lights[2].La"], .2, .2, .2)

local light_pos0 = vec4(-7, 4, 2.5, 1)
local light_pos1 = vec4( 0, 4, 2.5, 1)
local light_pos2 = vec4( 7, 4, 2.5, 1)

-- Generate the meshes
local plane = new_plane(20, 10, 1, 1)
local teapot = new_teapot(14)
local sphere = new_sphere(2.0, 50, 50)
local quad = new_quad()

-- Setup the fbo ----------------------------
local hdr_fbo = gl.new_framebuffer('draw read')
-- The depth buffer
local depth_buf = gl.new_renderbuffer('renderbuffer')
gl.renderbuffer_storage('renderbuffer', 'depth component', W, H)
-- The HDR color buffer
gl.active_texture(0)
local hdr_tex = gl.new_texture('2d')
gl.texture_storage('2d', 1, 'rgb32f', W, H)
gl.texture_parameter('2d', 'min filter', 'nearest')
gl.texture_parameter('2d', 'mag filter', 'nearest')
-- Attach the images to the framebuffer
gl.framebuffer_renderbuffer('draw read', 'depth attachment', 'renderbuffer', depth_buf)
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', hdr_tex, 0)
gl.draw_buffers('none', 'color attachment 0')
gl.unbind_framebuffer('draw read')

-- Event loop -----------------------------------------------------------------


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

   gl.enable('depth test')
   gl.clear_color(.5, .5, .5, 1)
   gl.viewport(0, 0, W, H)
   gl.bind_framebuffer('draw read', hdr_fbo)
   gl.clear('color', 'depth')
    
   local view = glmath.look_at(vec3(2, 0, 14), vec3(0,0,0), vec3(0,1,0))
   local projection = glmath.perspective(rad(60.0), W/H, 0.3, 100.0)

   -- Draw scene

   gl.uniformf(loc["Lights[0].L"], 1, 1, 1)
   gl.uniformf(loc["Lights[1].L"], 1, 1, 1)
   gl.uniformf(loc["Lights[2].L"], 1, 1, 1)

   gl.uniformf(loc["Lights[0].Position"], view*light_pos0)
   gl.uniformf(loc["Lights[1].Position"], view*light_pos1)
   gl.uniformf(loc["Lights[2].Position"], view*light_pos2)

   set_material(vec3(.9, .3, .2), vec3(1, 1, 1), vec3(.2, .2, .2), 100.0)

   -- The backdrop plane
   local model = glmath.rotate(rad(90), 1, 0, 0)
   set_matrices(model, view, projection)
   plane:render()

   -- The bottom plane
   local model = glmath.translate(0, -5, 0)
   set_matrices(model, view, projection)
   plane:render()

   -- Top plane
   model = glmath.translate(0, 5, 0)*glmath.rotate(rad(180), 1, 0, 0)
   set_matrices(model, view, projection)
   plane:render()

   -- The sphere
   set_material(vec3(.4, .9, .4), vec3(1, 1, 1), vec3(.2, .2, .2), 100.0)
   model = glmath.translate(-3,-3,2)
   set_matrices(model, view, projection)
   sphere:render()

   -- The teapot
   set_material(vec3(.4, .4, .9), vec3(1, 1, 1), vec3(.2, .2, .2), 100.0)
   model = glmath.translate(4,-5,1.5)*glmath.rotate(angle, 0, 1, 0)*glmath.rotate(rad(-90), 1, 0, 0)
   set_matrices(model, view, projection)
   teapot:render()

   -- Compute Luminance
   gl.active_texture(0)
   gl.bind_texture('2d', hdr_tex)
   -- This is a time consuming operation if done of the CPU (expecially with Lua, which
   -- is not very efficient at crunching numbers). It would be better to do it on the GPU,
   -- in a compute shader.
   local size = W*H
   local data = gl.get_texture_image('2d', 0, 'rgb', 'float', size*3*gl.sizeof('float'))
   data = gl.unpack('float', data)
   local sum = 0
   for i = 0, size-1, 3 do
      sum = sum + log(0.00001 + .2126*data[i+1] + .7152*data[i+2] +.0722*data[i+3])
   end
   gl.uniformf(loc["AveLum"], exp(sum/size ))

   -- Render (pass 2) --------------------------------------
   gl.uniformi(loc["Pass"], 2)

    -- Revert to default framebuffer and render the quad
   gl.unbind_framebuffer('draw read')
   gl.disable('depth test')
   gl.clear('color')
   set_matrices(mat4(), mat4(), mat4())
   quad:render()

   glfw.swap_buffers(window)
end

