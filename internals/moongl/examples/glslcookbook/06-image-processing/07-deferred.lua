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
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 6 - Deferred shading"
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
   projection = mat4()
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', "shaders/deferred.vert",
                                       'fragment', "shaders/deferred.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Pass",
   "Light.L",
   "Light.Position",
   "Material.Kd",
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

gl.uniformf(loc["Light.L"], 1, 1, 1)
gl.uniformf(loc["Light.Position"], 0, 0, 0, 1)

-- Generate the meshes
local plane = new_plane(50, 50, 1, 1)
local teapot = new_teapot(14)
local torus = new_torus(0.7*1.5, 0.3*1.5, 50,50)
local quad = new_quad()

-- Setup the fbo ----------------------------
local function create_gbuf_tex(unit, format)
   gl.active_texture(unit)
   local texid = gl.new_texture('2d')
   gl.texture_storage('2d', 1, format, W, H)
   gl.texture_parameter('2d', 'min filter', 'nearest')
   gl.texture_parameter('2d', 'mag filter', 'nearest')
   gl.texture_parameter('2d', 'max level', 0)
   return texid
end

local deferred_fbo = gl.new_framebuffer('draw read')
-- The depth buffer
local depth_buf = gl.new_renderbuffer('renderbuffer')
gl.renderbuffer_storage('renderbuffer', 'depth component', W, H)
-- Color attachments
local pos_tex = create_gbuf_tex(0, 'rgb32f')  -- Position
local norm_tex = create_gbuf_tex(1, 'rgb32f') -- Normal
local color_tex = create_gbuf_tex(2, 'rgb8')  -- Color

-- Attach the textures to the framebuffer
gl.framebuffer_renderbuffer('draw read', 'depth attachment', 'renderbuffer', depth_buf)
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', pos_tex, 0)
gl.framebuffer_texture_2d('draw read', 'color attachment 1', '2d', norm_tex, 0)
gl.framebuffer_texture_2d('draw read', 'color attachment 2', '2d', color_tex, 0)
gl.draw_buffers('none', 'color attachment 0', 'color attachment 1', 'color attachment 2')
gl.unbind_framebuffer('draw read')

-- Event loop -----------------------------------------------------------------

print("Press space to toggle animation on/off")

local model
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
   gl.bind_framebuffer('draw read', deferred_fbo)
   gl.enable('depth test')
   gl.clear_color(.5, .5, .5, 1)
   gl.clear('color', 'depth')
 
   local view = glmath.look_at(vec3(7*cos(angle),4,7*sin(angle)), vec3(0,0,0), vec3(0,1,0))
   local projection = glmath.perspective(rad(60.0), W/H, 0.3, 100.0)

   -- Draw scene ------
   -- The teapot
   gl.uniformf(loc["Material.Kd"], 0.9, 0.9, 0.9);
   local model = glmath.rotate(rad(-90.0), 1,0,0)
   set_matrices(model, view, projection)
   teapot:render()

   -- The plane
   gl.uniformf(loc["Material.Kd"], 0.4, 0.4, 0.4);
   model = glmath.translate(0, -0.75, 0)
   set_matrices(model, view, projection)
   plane:render()

   -- The torus
   gl.uniformf(loc["Material.Kd"], 0.9, 0.5, 0.2);
   model = glmath.translate(1,1,3)* glmath.rotate(rad(90), 1,0,0)
   set_matrices(model, view, projection)
   torus:render()

   gl.finish()

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

