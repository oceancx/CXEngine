#!/usr/bin/env lua
local glfw = require("moonglfw")
local gl = require("moongl")

local TITLE = "Chapter 2 - Scene using separable shaders and program pipelines"
local W, H = 500, 500

-- GLFW/GL initializations ----------------------------------------------------
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('resizable', false)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   end
end)

glfw.set_window_size_callback(window, function(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
end)

-- Compile shader program -----------------------------------------------------

local function load_shader_code(filename)
   local f = assert(io.open(filename))
   local code = f:read("a")
   f:close()
	return code
end

local function check_link_status(program)
   if not gl.get_program(program, 'link status') then
      error("Failed to link shader program!\nProgram log: "..gl.get_program_info_log(program))
   end
end

local function link(program)
	-- Link the program
   gl.link_program(program)
	check_link_status(program)
end

local function compile_shader(filename, shadertype)
	local code = load_shader_code(filename)
   local shader = gl.create_shader(shadertype)
	gl.shader_source(shader, code)
	gl.compile_shader(shader)
   if not gl.get_shader(shader, 'compile status') then
  		error("Shader compilation failed for "..filename.."\nShader log: "..
			gl.get_shader_info_log(shader))
   end
   return shader
end

local function create_shader_programs1()
	local vertCode = load_shader_code("shaders/separable.vert")
	local fragCode1 = load_shader_code("shaders/separable1.frag")
	local fragCode2 = load_shader_code("shaders/separable2.frag")

	local prog = {}
	prog[1] = gl.create_shader_program('vertex', vertCode)
	check_link_status(prog[1])
	prog[2] = gl.create_shader_program('fragment', fragCode1)
	check_link_status(prog[2])
	prog[3] = gl.create_shader_program('fragment', fragCode2)
	check_link_status(prog[3])
	return prog
end

local function create_shader_programs2()
	local vertShader = compile_shader("shaders/separable.vert", 'vertex')
	local fragShader1 = compile_shader("shaders/separable1.frag", 'fragment')
	local fragShader2 = compile_shader("shaders/separable2.frag", 'fragment')

	local prog = {}
	prog[1] = gl.create_program()
	prog[2] = gl.create_program()
	prog[3] = gl.create_program()

	gl.program_parameter(prog[1], 'separable', true)
	gl.program_parameter(prog[2], 'separable', true)
	gl.program_parameter(prog[3], 'separable', true)

	-- Attach the shaders to the program objects
	gl.attach_shader(prog[1], vertShader)
	gl.attach_shader(prog[2], fragShader1)
	gl.attach_shader(prog[3], fragShader2)

	link(prog[1])
	link(prog[2])
	link(prog[3])
	return prog
end

-- Parse command line options -------------------------------------------------

local USAGE = "\nUsage: "..arg[0].." <option>\n"..[[
Options:
        -1   Use gl.create_shader_program()
        -2   Use gl.create_program()
]]

local option = arg[1]
local prog -- program handles

-- Create the separable shader programs.
if not option then
   print(USAGE)
   os.exit(true)
elseif option == '-1' then
   prog = create_shader_programs1()
elseif option == '-2' then
   prog = create_shader_programs2()
else
   print("Invalid option")
   print(USAGE)
   os.exit(true)
end

-- Create the two shader pipelines
local pipelines = { gl.create_program_pipelines(2) }
-- Pipeline 1 uses the vertex shader (prog[1]) and first fragment shader (prog[2])
gl.use_program_stages(pipelines[1], prog[1], 'vertex')
gl.use_program_stages(pipelines[1], prog[2], 'fragment')
-- Pipeline 2 uses the vertex shader (prog[1]) and second fragment shader (prog[3])
gl.use_program_stages(pipelines[2], prog[1], 'vertex')
gl.use_program_stages(pipelines[2], prog[3], 'fragment')

-- Init scene -----------------------------------------------------------------

local position_data = {
   -0.8, -0.8, 0.0,
    0.8, -0.8, 0.0,
    0.0,  0.8, 0.0
}
local color_data = {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
}

-- Create and populate the buffer objects
local position_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', position_data), 'static draw')
local color_vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', color_data), 'static draw')

-- Create and set-up the vertex array object
local vao = gl.new_vertex_array()
gl.enable_vertex_attrib_array(0)  -- Vertex position
gl.enable_vertex_attrib_array(1)  -- Vertex color
gl.bind_buffer('array', position_vbo)
gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
gl.bind_buffer('array', color_vbo)
gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)
gl.unbind_vertex_array() -- same as gl.bind_vertex_array(0)

-- Event loop -----------------------------------------------------------------

gl.clear_color(0.5, 0.5, 0.5, 1.0)

while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Render
   gl.use_program(0)
	-- Set the uniform
  	local uniLocation = gl.get_uniform_location(prog[1], "uColorMask")
  	gl.program_uniformf(prog[1], uniLocation, 0.0, 1.0, 0.0)

   gl.clear('color')
   gl.bind_vertex_array(vao)
	-- Use the first pipeline on the left
	gl.viewport(0, 0, W/2, H)
   gl.bind_program_pipeline(pipelines[1])
   gl.draw_arrays('triangles', 0, 3)

	-- Use the second pipeline on the right
	gl.viewport(W/2, 0, W/2, H)
   gl.bind_program_pipeline(pipelines[2])
   gl.draw_arrays('triangles', 0, 3)

   gl.unbind_vertex_array()
   glfw.swap_buffers(window)
end

