#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_cube = require("common.cube")
local new_objmesh = require("common.objmesh")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local fmt = string.format

local TITLE = "Chapter 5 - Render to a texture using framebuffer objects"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = rad(140), pi/8 -- rad, rad/s
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
local prog, vsh, fsh = gl.make_program({
   vertex = "shaders/rendertotex.vert",
   fragment = "shaders/rendertotex.frag"
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
    "RenderTex",
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

local function set_material(ks, shininess)
   gl.uniformf(loc["Material.Ks"], ks)
   gl.uniformf(loc["Material.Shininess"], shininess)
end

gl.uniformf(loc["Light.L"], 1, 1, 1)
gl.uniformf(loc["Light.La"], .15, .15, .15)
gl.uniformf(loc["Light.Position"], 0.0,0.0,0.0,1.0)

-- Generate the meshes
local cube = new_cube()
local spot = new_objmesh("../media/spot/spot_triangulated.obj", {texcoords=true, tangents=true})

-- Load the textures
gl.active_texture(1)
local spot_texture = texture.load_texture("../media/spot/spot_texture.png")
gl.bind_texture('2d', spot_texture)


-- Set up FBO --------------------------------
-- Generate and bind the framebuffer
local fbo = gl.new_framebuffer('draw read')
-- Create the texture object
gl.active_texture(0)
local render_tex = gl.new_texture('2d')
gl.texture_storage('2d', 1, 'rgba8', 512, 512)
gl.texture_parameter('2d', 'min filter', 'linear')
gl.texture_parameter('2d', 'mag filter', 'linear')
-- Bind the texture to the FBO
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', render_tex, 0)
-- Create the depth buffer
local depth_buf = gl.new_renderbuffer('renderbuffer')
gl.renderbuffer_storage('renderbuffer', 'depth component', 512, 512)
-- Bind the depth buffer to the FBO
gl.framebuffer_renderbuffer('draw read', 'depth attachment', 'renderbuffer', depth_buf)
-- Set the targets for the fragment output variables
gl.draw_buffers('color attachment 0')
print("Framebuffer status: ".. gl.check_framebuffer_status('draw read'))
-- Unbind the framebuffer, and revert to default framebuffer
gl.unbind_framebuffer('draw read')

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

   -- Render to texture
   gl.bind_framebuffer('draw read', fbo)
   gl.uniformi(loc["RenderTex"], 1)
   gl.viewport(0,0,512,512)
   gl.clear('color', 'depth')
   set_material(vec3(.95, .95, .95), 100.0)
   local view = glmath.look_at(vec3(0, 0, 2.5), vec3(0,0,0), vec3(0,1,0))
   projection = glmath.perspective(rad(50.0), 1.0, 0.3, 100.0)
   local model = glmath.rotate(angle, 0, 1, 0)
   set_matrices(view, model)
   spot:render()
   gl.flush()
   gl.unbind_framebuffer('draw read')

   -- Render scene
   gl.uniformi(loc["RenderTex"], 0)
   gl.viewport(0,0,W,H)
   gl.clear('color', 'depth')

   local camera_pos = vec3(2*cos(angle), 1.5, 2*sin(angle))
   local view = glmath.look_at(camera_pos, vec3(0,0,0), vec3(0,1,0))
   projection = glmath.perspective(rad(45.0), W/H, 0.3, 100.0)
   set_material(vec3(0, 0, 0), 1.0)
   local model = mat4()
   set_matrices(view, model)
   cube:render()

   glfw.swap_buffers(window)
end

