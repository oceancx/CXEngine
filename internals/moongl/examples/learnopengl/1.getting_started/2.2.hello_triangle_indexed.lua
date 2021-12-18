#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")

local SCR_WIDTH, SCR_HEIGHT = 800, 600

local vertex_shader_source = [[
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
]]

local fragment_shader_source = [[
#version 330 core
out vec4 FragColor;
void main() {
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
]]

-- glfw inits and window creation ---------------------------------------------
glfw.version_hint(3, 3, 'core')
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers
glfw.set_framebuffer_size_callback(window, function (window, width, height)
    gl.viewport(0, 0, width, height)
end)

-- build and compile our shader program ----------------------------------------
-- vertex shader
local vsh = gl.create_shader('vertex')
gl.shader_source(vsh, vertex_shader_source)
gl.compile_shader(vsh)
if not gl.get_shader(vsh, 'compile status') then
   error(gl.get_shader_info_log(vsh))
end
-- fragment shader
local fsh = gl.create_shader('fragment')
gl.shader_source(fsh, fragment_shader_source)
gl.compile_shader(fsh)
if not gl.get_shader(fsh, 'compile status') then
   error(gl.get_shader_info_log(fsh))
end
-- link shaders
local prog = gl.create_program()
gl.attach_shader(prog, vsh)
gl.attach_shader(prog, fsh)
gl.link_program(prog)
-- check for linking errors
if not gl.get_program(prog, 'link status') then
   error(gl.get_program_info_log(prog))
end
gl.delete_shader(vsh)
gl.delete_shader(fsh)

-- set up vertex data (and buffer(s)) and configure vertex attributes ---------
local vertices = {
  0.5,  0.5, 0.0,  -- top right
  0.5, -0.5, 0.0,  -- bottom right
 -0.5, -0.5, 0.0,  -- bottom left
 -0.5,  0.5, 0.0   -- top left 
}
local indices = {  -- note that we start from 0!
   0, 1, 3,  -- first triangle
   1, 2, 3   -- second triangle
}

local vao = gl.gen_vertex_arrays()
local vbo = gl.gen_buffers()
local ebo = gl.gen_buffers()
-- bind the Vertex Array Object first, then bind and set vertex buffer(s), and then
-- configure vertex attributes(s).
gl.bind_vertex_array(vao)
gl.bind_buffer('array', vbo)
gl.buffer_data('array', gl.pack('float', vertices), 'static draw')
gl.bind_buffer('element array', ebo);
gl.buffer_data('element array', gl.pack('uint', indices), 'static draw')
gl.vertex_attrib_pointer(0, 3, 'float', false, 3*gl.sizeof('float'), 0)
gl.enable_vertex_attrib_array(0)
-- note that this is allowed, the call to gl.vertex_attrib_pointer registered vbo as
-- the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
gl.unbind_buffer('array')
-- DON'T: gl.unbind_buffer('element array')
-- Remember: do NOT unbind the EBO while a VAO is active as the bound element buffer
-- object IS stored in the VAO; keep the EBO bound.
-- You can unbind the vao afterwards so other VAO calls won't accidentally modify this vao,
-- but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways
-- so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
gl.unbind_vertex_array() 

-- To draw in wireframe polygons, run the script with the '-w' option as firts arg:
if arg[1]=='-w' then
   gl.polygon_mode('front and back', 'line')
end

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
   gl.bind_vertex_array(vao)
-- gl.draw_arrays('triangles', 0, 3)
   gl.draw_elements('triangles', 6, 'uint', 0)
   gl.unbind_vertex_array()

   -- swap buffers and poll IO events
   glfw.swap_buffers(window)
   glfw.poll_events()
end

-- optional: de-allocate all resources once they've outlived their purpose:
gl.delete_vertex_arrays(vao)
gl.delete_buffers(vbo)
gl.delete_buffers(ebo)

