#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local gl = require("moongl")
local glfw = require("moonglfw")
local glmath = require("moonglmath")
local new_texture = require("common.texture")
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
local prog, vsh, fsh = gl.make_program({vertex="shaders/5.1.framebuffers.vert",
                                         fragment="shaders/5.1.framebuffers.frag"})
gl.delete_shaders(vsh, fsh)
local prog1, vsh, fsh = gl.make_program({vertex="shaders/5.1.framebuffers_screen.vert",
                                         fragment="shaders/5.1.framebuffers_screen.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ----------
local cube_vertices = {
   -- positions          -- texture Coords
  -0.5, -0.5, -0.5,  0.0, 0.0,
   0.5, -0.5, -0.5,  1.0, 0.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
  -0.5,  0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 0.0,

  -0.5, -0.5,  0.5,  0.0, 0.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 1.0,
   0.5,  0.5,  0.5,  1.0, 1.0,
  -0.5,  0.5,  0.5,  0.0, 1.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,

  -0.5,  0.5,  0.5,  1.0, 0.0,
  -0.5,  0.5, -0.5,  1.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,
  -0.5,  0.5,  0.5,  1.0, 0.0,

   0.5,  0.5,  0.5,  1.0, 0.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5,  0.5,  0.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 0.0,

  -0.5, -0.5, -0.5,  0.0, 1.0,
   0.5, -0.5, -0.5,  1.0, 1.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
   0.5, -0.5,  0.5,  1.0, 0.0,
  -0.5, -0.5,  0.5,  0.0, 0.0,
  -0.5, -0.5, -0.5,  0.0, 1.0,

  -0.5,  0.5, -0.5,  0.0, 1.0,
   0.5,  0.5, -0.5,  1.0, 1.0,
   0.5,  0.5,  0.5,  1.0, 0.0,
   0.5,  0.5,  0.5,  1.0, 0.0,
  -0.5,  0.5,  0.5,  0.0, 0.0,
  -0.5,  0.5, -0.5,  0.0, 1.0
}
local plane_vertices = {
   -- positions      texture coords 
   5.0, -0.5,  5.0,  2.0, 0.0,
  -5.0, -0.5,  5.0,  0.0, 0.0,
  -5.0, -0.5, -5.0,  0.0, 2.0,

   5.0, -0.5,  5.0,  2.0, 0.0,
  -5.0, -0.5, -5.0,  0.0, 2.0,
   5.0, -0.5, -5.0,  2.0, 2.0                      
}

-- vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
local quad_vertices = {
-- positions   texCoords
  -1.0,  1.0,  0.0, 1.0,
  -1.0, -1.0,  0.0, 0.0,
   1.0, -1.0,  1.0, 0.0,

  -1.0,  1.0,  0.0, 1.0,
   1.0, -1.0,  1.0, 0.0,
   1.0,  1.0,  1.0, 1.0
}

-- cube VAO
local cube_vao = gl.new_vertex_array()
local cube_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(cube_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 3, 'float', false, 5*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 2, 'float', false, 5*gl.sizeof('float'), 3*gl.sizeof('float'))
gl.unbind_vertex_array()
-- plane VAO
local plane_vao = gl.new_vertex_array()
local plane_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(plane_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 3, 'float', false, 5*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 2, 'float', false, 5*gl.sizeof('float'), 3*gl.sizeof('float'))
gl.unbind_vertex_array()
-- quad VAO
local quad_vao = gl.new_vertex_array()
local quad_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.packf(quad_vertices), 'static draw')
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(0, 2, 'float', false, 4*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(1)
gl.vertex_attrib_pointer(1, 2, 'float', false, 4*gl.sizeof('float'), 2*gl.sizeof('float'))
gl.unbind_vertex_array()

-- load textures
local cube_texture = new_texture("../resources/textures/marble.jpg")
local floor_texture = new_texture("../resources/textures/metal.png")

-- get the locations of the uniforms:
local loc = {} -- holds the locations for prog (indexed by the uniform variables names)
gl.use_program(prog)
loc.model = gl.get_uniform_location(prog, "model")
loc.view = gl.get_uniform_location(prog, "view")
loc.projection = gl.get_uniform_location(prog, "projection")
loc.texture1 = gl.get_uniform_location(prog, "texture1")
gl.uniformi(loc.texture1, 0)

local loc1 = {} -- holds the locations for prog (indexed by the uniform variables names)
gl.use_program(prog1)
loc1.screenTexture = gl.get_uniform_location(prog1, "screenTexture")
gl.uniformi(loc1.screenTexture, 0)

-- framebuffer configuration ---------------------------------------------------
local fbo = gl.gen_framebuffers()
gl.bind_framebuffer('draw read', fbo) -- 'draw read' = GL_FRAMEBUFFER
-- create a color attachment texture
local textureColorbuffer = gl.new_texture('2d')
gl.texture_image('2d', 0, 'rgb', 'rgb', 'ubyte', nil, SCR_WIDTH, SCR_HEIGHT)
gl.texture_parameter('2d', 'min filter', 'linear')
gl.texture_parameter('2d', 'mag filter', 'linear')
gl.framebuffer_texture_2d('draw read', 'color attachment 0', '2d', textureColorbuffer, 0)
-- create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
local rbo = gl.gen_renderbuffers()
gl.bind_renderbuffer('renderbuffer', rbo)
gl.renderbuffer_storage('renderbuffer', 'depth24 stencil8', SCR_WIDTH, SCR_HEIGHT)
-- now actually attach it to the fbo
gl.framebuffer_renderbuffer('draw read', 'depth stencil attachment', 'renderbuffer', rbo)
-- check that the framebuffer is complete
assert(gl.check_framebuffer_status('draw read')=='complete', "framebuffer is not complete!")
gl.unbind_framebuffer('draw read')

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

   -- bind to framebuffer and draw scene as we normally would to color texture 
   gl.bind_framebuffer('draw read', fbo)
   gl.enable('depth test') -- enable depth testing (disabled when rendering screen-space quad)
   -- make sure we clear the framebuffer's content
   gl.clear_color(0.1, 0.1, 0.1, 1.0)
   gl.clear('color', 'depth')
   -- draw the scene to the framebuffer:
   gl.use_program(prog)
   local view = camera:view()
   local projection = perspective(rad(camera.zoom), SCR_WIDTH/SCR_HEIGHT, 0.1, 100.0)
   gl.uniform_matrix4f(loc.view, true, view)
   gl.uniform_matrix4f(loc.projection, true, projection)
   -- cubes
   gl.bind_vertex_array(cube_vao)
   gl.active_texture(0)
   gl.bind_texture('2d', cube_texture)
   gl.uniform_matrix4f(loc.model, true, translate(-1.0, 0.0, -1.0))
   gl.draw_arrays('triangles', 0, 36)
   gl.uniform_matrix4f(loc.model, true, translate(2.0, 0.0, 0.0))
   gl.draw_arrays('triangles', 0, 36)
   -- floor
   gl.bind_vertex_array(plane_vao)
   gl.bind_texture('2d', floor_texture)
   gl.uniform_matrix4f(loc.model, true, mat4())
   gl.draw_arrays('triangles', 0, 6)

   -- now bind back the default framebuffer and draw a fullscreen quad textured
   -- with the content of the fbo's color attachment
   gl.bind_framebuffer('draw read', 0)
   gl.disable('depth test') -- so screen-space quad isn't discarded due to depth test
   -- clear all relevant buffers
   gl.clear_color(1.0, 1.0, 1.0, 1.0) -- not actually necessary (the quad will cover the whole screen)
   gl.clear('color')
   gl.use_program(prog1)
   gl.bind_vertex_array(quad_vao)
   gl.bind_texture('2d', textureColorbuffer) -- use the color attachment as texture for the quad
   gl.draw_arrays('triangles', 0, 6)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

gl.delete_vertex_arrays(cube_vao, plane_vao, quad_vao)
gl.delete_buffers(cube_vbo, plane_vbo, quad_vbo)
gl.delete_framebuffers(fbo)
gl.delete_renderbuffers(rbo)

