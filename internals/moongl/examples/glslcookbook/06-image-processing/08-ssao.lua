#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local rand = require("common.random")
local new_plane = require("common.plane")
local new_objmesh = require("common.objmesh")
local new_quad = require("common.quad")
local texture = require("common.texture")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

math.randomseed(os.time())

local TITLE = "Chapter 6 - Screen-space ambient occlusion"
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
   projection = glmath.perspective(rad(50), W/H, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', "shaders/ssao.vert",
                                       'fragment', "shaders/ssao.frag")
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Pass",
   "Light.L",
   "Light.La",
   "Light.Position",
   "Material.Kd",
   "Material.UseTex",
   "SampleKernel",
   "RandScale",
   "ModelViewMatrix",
   "NormalMatrix",
   "ProjectionMatrix",
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

gl.uniformf(loc["Light.L"], .3, .3, .3)
gl.uniformf(loc["Light.La"], .5, .5, .5)
gl.uniformf(loc["Material.Kd"], .9, .5, .2)

-- Load textures
local wood_tex = texture.load_texture("../media/texture/hardwood2_diffuse.jpg")
local brick_tex = texture.load_texture("../media/texture/brick1.jpg")

-- Generate the meshes
local plane = new_plane(10, 10, 1, 1, 10, 7)
local mesh = new_objmesh("../media/bs_ears.obj")
local quad = new_quad()

do -- Create the random sample kernel
   local size = 64 -- kernel size
   local kernel = {}
   for i=0,size-1 do
      local scale = i*i/size/size
      kernel[#kernel+1] = rand.uniform_hemisphere() * glmath.mix(.1, 1.0, scale)
   end
   gl.uniformfv(loc["SampleKernel"], size, kernel)
   -- same as: gl.uniformv(loc["SampleKernel"], size, 'float', gl.flatten(kernel))
end
local RandRotSize = 4
do -- Create the texture of random rotation directions
   local size = RandRotSize
   local dirs = {}
   for i=1,size*size do
      dirs[#dirs+1] = rand.uniform_circle()
      print(dirs[#dirs])
   end
   local data = gl.flatten(dirs)
   local tex = gl.new_texture('2d')
   gl.texture_storage('2d', 1, 'rgb16f', size, size)
   gl.texture_sub_image('2d', 0, 'rgb', 'float', data, 0, 0, size, size)
   gl.texture_parameter('2d', 'min filter', 'nearest')
   gl.texture_parameter('2d', 'mag filter', 'nearest')
   gl.texture_parameter('2d', 'max level', 0)
   gl.active_texture(4);
   gl.bind_texture('2d', tex)
end

local function create_gbuf_tex(unit, format)
   gl.active_texture(unit)
   local texid = gl.new_texture('2d')
   gl.texture_storage('2d', 1, format, W, H)
   gl.texture_parameter('2d', 'min filter', 'nearest')
   gl.texture_parameter('2d', 'mag filter', 'nearest')
   gl.texture_parameter('2d', 'max level', 0)
   return texid
end

-- Setup the fbo ----------------------------
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
gl.draw_buffers('none', 'color attachment 0', 'color attachment 1', 'color attachment 2', 'none')
gl.unbind_framebuffer('draw read')

-- Setup the SSAO fbo ----------------------------
local ssao_fbo = gl.new_framebuffer('draw read')
-- The AO pair
local ao_tex = { create_gbuf_tex(3, 'rgb8'),  create_gbuf_tex(3, 'rgb8') }
-- Attach the first texture to the framebuffer
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', ao_tex[1], 0)
-- connect to output variable at location 4:
gl.draw_buffers('none', 'none', 'none', 'none', 'color attachment 0')
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

   -- Render to g-buffers (pass 1) --------------------------------------
   gl.uniformi(loc["Pass"], 1)
   gl.bind_framebuffer('draw read', deferred_fbo)
   gl.enable('depth test')
   gl.clear_color(.5, .5, .5, 1)
   gl.clear('color', 'depth')
 
   local model
   local view = glmath.look_at(vec3(2.1, 1.5, 2.1), vec3(0, 1, 0), vec3(0, 1, 0))
   local view = glmath.look_at(vec3(3, 1.5, 2.1), vec3(0, 1, 0), vec3(0, 1, 0))
   gl.uniformf(loc["Light.Position"], view * vec4(3.0, 3.0, 1.5, 1.0))

   -- The walls
   gl.uniformf(loc["Material.UseTex"], 1)
   gl.active_texture(5)
   gl.bind_texture('2d', wood_tex)
   model = mat4()
   set_matrices(model, view, projection)
   plane:render()
   gl.bind_texture('2d', brick_tex)
   model = glmath.translate(0, 0, -2) * glmath.rotate(rad(90), 1, 0, 0)
   set_matrices(model, view, projection)
   plane:render()

   model = glmath.translate(-2,0,0)*glmath.rotate(rad(90),0,1,0)*glmath.rotate(rad(90),1,0,0) 
   set_matrices(model, view, projection)
   plane:render()

   gl.uniformf(loc["Material.UseTex"], 0)
   model = glmath.rotate(rad(-45)+angle, 0, 1, 0)*glmath.scale(2)*glmath.translate(0, .5, 0)
   set_matrices(model, view, projection)
   mesh:render()

   -- SSAO (pass 2) --------------------------------------
   gl.uniformi(loc["Pass"], 2)
   gl.bind_framebuffer('draw read', ssao_fbo)
   gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', ao_tex[1], 0)
   gl.disable('depth test')
   gl.clear('color')
   gl.uniform_matrix4f(loc["ProjectionMatrix"], true, projection)
   set_matrices(mat4(), mat4(), mat4())
   quad:render()

   -- Blur (pass 3) --------------------------------------
   gl.uniformi(loc["Pass"], 3)
   gl.bind_framebuffer('draw read', ssao_fbo)
   -- read from first texture and write to second texture
   gl.active_texture(3)
   gl.bind_texture('2d', ao_tex[1]);
   gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', ao_tex[2], 0)
   --gl.disable('depth test')
   gl.clear('color')
   --set_matrices(mat4(), mat4(), mat4())
   gl.uniformf(loc["RandScale"], W/RandRotSize, H/RandRotSize)
   quad:render()

   -- Shade (pass 4) --------------------------------------
   gl.uniformi(loc["Pass"], 4)
   -- revert to default framebuffer
   gl.unbind_framebuffer('draw read')
   -- read from second ao texture
   gl.active_texture(3)
   gl.bind_texture('2d', ao_tex[2]);
   --gl.disable('depth test')
   gl.clear('color')
   --set_matrices(mat4(), mat4(), mat4())
   quad:render()

   glfw.swap_buffers(window)
end

