#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")

local SCR_WIDTH, SCR_HEIGHT = 800, 600

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers
glfw.set_framebuffer_size_callback(window, function (window, width, height)
    gl.viewport(0, 0, width, height)
end)

-- build, compile, and link our shader program ---------------------------------
-- NOTE: With MoonGL we don't really have to define our own 'shader class' to
-- simplify this process, because we already have the gl.make_program functions
-- (see the reference manual for details):
local prog, vsh, fsh = gl.make_program({vertex="shaders/3.3.shader.vert",
                                         fragment="shaders/3.3.shader.frag"})
gl.delete_shaders(vsh, fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ---------
local vertices = {
  -- positions      -- colors
   0.5, -0.5, 0.0,  1.0, 0.0, 0.0,  -- bottom right
  -0.5, -0.5, 0.0,  0.0, 1.0, 0.0,  -- bottom left
   0.0,  0.5, 0.0,  0.0, 0.0, 1.0   -- top 
}

local vao = gl.gen_vertex_arrays()
local vbo = gl.gen_buffers()
-- bind the Vertex Array Object first, then bind and set vertex buffer(s), and then
-- configure vertex attributes(s).
gl.bind_vertex_array(vao)
gl.bind_buffer('array', vbo)
gl.buffer_data('array', gl.pack('float', vertices), 'static draw')
gl.vertex_attrib_pointer(0, 3, 'float', false, 6*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
gl.vertex_attrib_pointer(1, 3, 'float', false, 6*gl.sizeof('float'), 3*gl.sizeof('float'))
gl.enable_vertex_attrib_array(1)
gl.unbind_buffer('array')
gl.unbind_vertex_array() 

-- render loop
while not glfw.window_should_close(window) do
   -- process input
   if glfw.get_key(window, 'escape') == 'press' then
       glfw.set_window_should_close(window, true)
   end

   -- render
   gl.clear_color(0.2, 0.3, 0.3, 1.0)
   gl.clear('color')
   gl.use_program(prog)
   -- draw the triangle
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(vao)
gl.delete_buffers(vbo)

