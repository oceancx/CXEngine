#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
local new_cubemap = require("common.cubemap")
local new_camera = require("common.camera")

-- A few shortcuts:
local vec3, mat4 = glmath.vec3, glmath.mat4
local rotate, translate, scale = glmath.rotate, glmath.translate, glmath.scale
local clamp = glmath.clamp
local perspective = glmath.perspective
local rad, sin, cos = math.rad, math.sin, math.cos

local SCR_WIDTH, SCR_HEIGHT = 1280, 720
-- camera:
local camera = new_camera(vec3(0.0, 0.0, 3.0))
local last_x, last_y = SCR_WIDTH/2, SCR_HEIGHT/2 -- initially at the center
local first_mouse = true

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers

-- build, compile, and link our shader programs --------------------------------
local prog, vsh, fsh = gl.make_program({vertex="shaders/6.2.cubemaps.vert",
                                         fragment="shaders/6.2.cubemaps.frag"})
gl.delete_shaders(vsh, fsh)
local prog1, vsh, fsh = gl.make_program({vertex="shaders/6.2.skybox.vert",
                                         fragment="shaders/6.2.skybox.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ----------
local cube_vertices = {
   -- positions          -- normals
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,
   0.5, -0.5, -0.5,  0.0,  0.0, -1.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
  -0.5,  0.5, -0.5,  0.0,  0.0, -1.0,
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0,

  -0.5, -0.5,  0.5,  0.0,  0.0, 1.0,
   0.5, -0.5,  0.5,  0.0,  0.0, 1.0,
   0.5,  0.5,  0.5,  0.0,  0.0, 1.0,
   0.5,  0.5,  0.5,  0.0,  0.0, 1.0,
  -0.5,  0.5,  0.5,  0.0,  0.0, 1.0,
  -0.5, -0.5,  0.5,  0.0,  0.0, 1.0,

  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,
  -0.5,  0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,
  -0.5, -0.5,  0.5, -1.0,  0.0,  0.0,
  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,

   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,
   0.5,  0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0,
   0.5, -0.5,  0.5,  1.0,  0.0,  0.0,
   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,

  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,
   0.5, -0.5, -0.5,  0.0, -1.0,  0.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
  -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,
  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,

  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,
   0.5,  0.5, -0.5,  0.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
  -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,
  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0
}

local skybox_vertices = {
   -- positions          
  -1.0,  1.0, -1.0,
  -1.0, -1.0, -1.0,
   1.0, -1.0, -1.0,
   1.0, -1.0, -1.0,
   1.0,  1.0, -1.0,
  -1.0,  1.0, -1.0,

  -1.0, -1.0,  1.0,
  -1.0, -1.0, -1.0,
  -1.0,  1.0, -1.0,
  -1.0,  1.0, -1.0,
  -1.0,  1.0,  1.0,
  -1.0, -1.0,  1.0,

   1.0, -1.0, -1.0,
   1.0, -1.0,  1.0,
   1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
   1.0,  1.0, -1.0,
   1.0, -1.0, -1.0,

  -1.0, -1.0,  1.0,
  -1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
   1.0, -1.0,  1.0,
  -1.0, -1.0,  1.0,

  -1.0,  1.0, -1.0,
   1.0,  1.0, -1.0,
   1.0,  1.0,  1.0,
   1.0,  1.0,  1.0,
  -1.0,  1.0,  1.0,
  -1.0,  1.0, -1.0,

  -1.0, -1.0, -1.0,
  -1.0, -1.0,  1.0,
   1.0, -1.0, -1.0,
   1.0, -1.0, -1.0,
  -1.0, -1.0,  1.0,
   1.0, -1.0,  1.0
}


-- cube VAO
local cube_vao = gl.new_vertex_array()
local cube_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(cube_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 3, 'float', false, 6*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 3, 'float', false, 6*gl.sizeof('float'), 3*gl.sizeof('float'))
gl.unbind_vertex_array()
-- skybox VAO
local skybox_vao = gl.new_vertex_array()
local skybox_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(skybox_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 3, 'float', false, 3*gl.sizeof('float'), 0)
gl.unbind_vertex_array()

-- load textures
local cubemap_texture = new_cubemap({
   "../resources/textures/skybox/right.jpg",
   "../resources/textures/skybox/left.jpg",
   "../resources/textures/skybox/top.jpg",
   "../resources/textures/skybox/bottom.jpg",
   "../resources/textures/skybox/front.jpg",
   "../resources/textures/skybox/back.jpg",
})

-- get the locations of the uniforms:
local loc = {} -- holds the locations for prog (indexed by the uniform variables names)
gl.use_program(prog)
loc.model = gl.get_uniform_location(prog, "model")
loc.view = gl.get_uniform_location(prog, "view")
loc.projection = gl.get_uniform_location(prog, "projection")
loc.cameraPos = gl.get_uniform_location(prog, "cameraPos")
loc.skybox = gl.get_uniform_location(prog, "skybox")
gl.uniformi(loc.skybox, 0)
local loc1 = {} -- holds the locations for prog1 (indexed by the uniform variables names)
gl.use_program(prog1)
loc1.view = gl.get_uniform_location(prog1, "view")
loc1.projection = gl.get_uniform_location(prog1, "projection")
loc1.skybox = gl.get_uniform_location(prog1, "skybox")
gl.uniformi(loc1.skybox, 0)

glfw.set_framebuffer_size_callback(window, function (window, w, h)
   gl.viewport(0, 0, w, h)
   SCR_WIDTH, SCR_HEIGHT = w, h
end)

glfw.set_cursor_pos_callback(window, function(window, xpos, ypos)
   -- whenever the mouse moves, this callback is called
   if first_mouse then
      last_x, last_y = xpos, ypos
      first_mouse = false
   end
   local xoffset = xpos - last_x
   local yoffset = last_y - ypos -- reversed since y-coordinates go from bottom to top
   last_x, last_y = xpos, ypos
   camera:process_mouse(xoffset, yoffset, true)
end)

glfw.set_scroll_callback(window, function(window, xoffset, yoffset)
   -- whenever the mouse scroll wheel scrolls, this callback is called
   camera:process_scroll(yoffset)
end)

-- tell GLFW to capture our mouse:
glfw.set_input_mode(window, 'cursor', 'disabled')

-- configure global opengl state
gl.enable('depth test')

local last_frame_time = 0.0 -- last frame time

local function keypressed(x) return glfw.get_key(window, x)=='press' end

-- render loop
while not glfw.window_should_close(window) do
   local t = glfw.get_time()
   local dt = t - last_frame_time
   last_frame_time = t

   -- process input
   if keypressed('escape') then glfw.set_window_should_close(window, true) end
   -- camera movement controlled either by WASD keys or arrow keys:
   if keypressed('w') or keypressed('up') then camera:process_keyboard('forward', dt) end
   if keypressed('a') or keypressed('left') then camera:process_keyboard('left', dt) end
   if keypressed('s') or keypressed('down') then camera:process_keyboard('backward', dt) end
   if keypressed('d') or keypressed('right') then camera:process_keyboard('right', dt) end

   -- render
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   -- draw scene as normal
   gl.use_program(prog)
   local view = camera:view()
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(loc.view, true, view)
   gl.uniform_matrix4f(loc.projection, true, projection)
   gl.uniformf(loc.cameraPos, camera.position)
   -- cube
   gl.bind_vertex_array(cube_vao)
   gl.active_texture(0)
   gl.bind_texture('cube map', cubemap_texture)
   gl.uniform_matrix4f(loc.model, true, mat4())
   gl.draw_arrays('triangles', 0, 36)
   gl.unbind_vertex_array()

   -- draw skybox as last
   gl.depth_func('lequal') -- so that depth test passes when values are equal to depth buffer's content
   gl.use_program(prog1)
   view[1][4], view[2][4], view[3][4] = 0, 0, 0 -- remove translation
   gl.uniform_matrix4f(loc1.view, true, view)
   gl.uniform_matrix4f(loc1.projection, true, projection)
   gl.bind_vertex_array(skybox_vao)
   gl.active_texture(0)
   gl.bind_texture('cube map', cubemap_texture)
   gl.draw_arrays('triangles', 0, 36)
   gl.unbind_vertex_array()
   gl.depth_func('less') -- set depth function back to default

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

gl.delete_vertex_arrays(cube_vao, skybox_vao)
gl.delete_buffers(cube_vbo, skybox_vbo)

