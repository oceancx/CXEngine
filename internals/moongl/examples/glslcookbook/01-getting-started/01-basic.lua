#!/usr/bin/env lua
local glfw = require("moonglfw")
local gl = require("moongl")

local TITLE = "Chapter 1 - Basic scene"
local W, H = 500, 500

-- GLFW/GL initializations ----------------------------------------------------
glfw.window_hint('context version major', 4)
glfw.window_hint('context version minor', 6)
glfw.window_hint('opengl forward compat', true)
glfw.window_hint('opengl profile', 'core')
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

-- Compile/load shader program ------------------------------------------------

local function compile_shader_program()
   print("Compiling shader program")

   -- Vertex shader --
   -- Load contents of file
   local f = assert(io.open("basic.vert"))
   local code = f:read("a")
   f:close()
   -- Create the shader object
   local vsh = gl.create_shader('vertex')
   -- Load the source code into the shader object
   gl.shader_source(vsh, code)
   -- Compile the shader
   gl.compile_shader(vsh)
   -- Check compilation status
   local result = gl.get_shader(vsh, 'compile status')
   if not result then
      error("Vertex shader compilation failed!\nShader log: "..gl.get_shader_info_log(vsh))
   end

   -- Fragment shader --
   -- Load contents of file
   local f = assert(io.open("basic.frag"))
   local code = f:read("a")
   f:close()
   -- Create the shader object
   local fsh = gl.create_shader('fragment')
   -- Load the source code into the shader object
   gl.shader_source(fsh, code)
   -- Compile the shader
   gl.compile_shader(fsh)
   -- Check compilation status
   local result = gl.get_shader(fsh, 'compile status')
   if not result then
      error("Fragment shader compilation failed!\nShader log: "..gl.get_shader_info_log(fsh))
   end

   -- Create the program object
   local prog = gl.create_program()

   --[[
   -- Bind index 0 to the shader input variable "VertexPosition"
   gl.bind_attrib_location(prog, 0, "VertexPosition")
   -- Bind index 1 to the shader input variable "VertexColor"
   gl.bind_attrib_location(prog, 1, "VertexColor")
   --]]

   -- Attach the shaders to the program object
   gl.attach_shader(prog, vsh)
   gl.attach_shader(prog, fsh)
   -- Link the program
   gl.link_program(prog)
   -- Check for successful linking
   local status = gl.get_program(prog, 'link status')
   if not status then
      error("Failed to link shader program!\nProgram log: "..gl.get_program_info_log(prog))
   end

   -- Clean up shader objects
   gl.detach_shader(prog, vsh)
   gl.detach_shader(prog, fsh)
   gl.delete_shader(vsh)
   gl.delete_shader(fsh)

   gl.use_program(prog)
   return prog
end


local function write_shader_binary(filename)
   local formats = { gl.get('program binary formats') }

   print("Number of binary formats supported by this driver = "..#formats)

   if #formats == 0 then
      print("No binary formats supported by this driver.  Unable to write shader binary.")
      return
   end

   local prog = compile_shader_program()
   local length = gl.get_program(prog, 'program binary length')
   print("Program binary length = "..length)

   local format, binary = gl.get_program_binary(prog)
   print("Writing to "..filename..", binary format = "..format)
   local f = assert(io.open(filename, "w"))
   f:write(binary)
   f:close()
end


local function load_shader_binary(filename, format)
   print("Loading shader binary "..filename..", binary format = "..format)

   -- Create the program object
   local prog = gl.create_program()

   local f = assert(io.open(filename, "r"))
   local binary = f:read("a")
   f:close()
   gl.program_binary(prog, format, binary)

   -- Check for successful linking
   if not gl.get_program(prog, 'link status') then
      error("Failed to load binary shader program!\n Program log:".. gl.get_program_info_log(prog))
   end

   gl.use_program(prog)
   return prog
end


local function load_spirv_shader(vertspv, fragspv)
   print("Loading SPIR-V shaders.")

   local vsh = gl.create_shader('vertex')
   print("Loading SPIR-V binary: "..vertspv)
   local f = assert(io.open(vertspv, "r"))
   local binary = f:read("a")
   f:close()
   gl.shader_binary({vsh}, 'spir v', binary)
   gl.specialize_shader(vsh, "main")
   if not gl.get_shader(vsh, 'compile status') then
      error("Failed to load vertex shader (SPIR-V)\n"..gl.get_shader_info_log(vsh))
   end

   local fsh = gl.create_shader('fragment')
   print("Loading SPIR-V binary: "..fragspv)
   local f = assert(io.open(fragspv, "r"))
   local binary = f:read("a")
   f:close()
   gl.shader_binary({fsh}, 'spir v', binary)
   gl.specialize_shader(fsh, "main")
   if not gl.get_shader(fsh, 'compile status') then
      error("Failed to load fragment shader (SPIR-V)\n"..gl.get_shader_info_log(fsh))
   end

   -- Create the program object
   local prog = gl.create_program()

   -- Attach the shaders to the program object
   gl.attach_shader(prog, vsh)
   gl.attach_shader(prog, fsh)
   -- Link the program
   gl.link_program(prog)
   -- Check for successful linking
   local status = gl.get_program(prog, 'link status')
   if not status then
      error("Failed to link SPIR-V program!\nProgram log: "..gl.get_program_info_log(prog))
   end

   gl.use_program(prog)
   return prog
end


-- Parse command line options -------------------------------------------------

local USAGE = "\nUsage: "..arg[0].." <option>\n"..[[
Options:
        -c   Compile the shader normally
        -w   Compile and write a binary shader program (file: program.bin, OpenGL>=4.1)
        -b   Load a binary shader program (file: program.bin, OpenGL>=4.1)
        -s   Load a SPIR-V shader program (files: vert.spv and frag.spv, OpenGL>=4.6)
]]
-- Note: To generate a SPIR-V files from GLSL source with the glslangValidator;
-- $ glslangValidator -G -o vert.spv basic.vert
-- $ glslangValidator -G -o frag.spv basic.frag

local option = arg[1]
local prog -- program handle

if not option then
   print(USAGE)
   os.exit(true)
elseif option == '-c' then
   prog = compile_shader_program()
elseif option == '-w' then
   write_shader_binary("program.bin")
   os.exit(true)
elseif option == '-b' then
   prog = load_shader_binary("program.bin", 36385)
elseif option == '-s' then
   prog = load_spirv_shader("vert.spv", "frag.spv")
else
   print("Invalid option")
   print(USAGE)
   os.exit(true)
end

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
   gl.clear('color')
   gl.bind_vertex_array(vao)
   gl.draw_arrays('triangles', 0, 3)
   gl.unbind_vertex_array()

   glfw.swap_buffers(window)
end

