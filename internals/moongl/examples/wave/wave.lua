#!/usr/bin/env lua
-- MoonGL example: wave.lua
--------------------------------------------------------------------------------
-- This example is derived from the example code that comes with the book:
-- "Interactive Computer Graphics - A Top-Down Approach with OpenGL"
-- by Edward Angel and Dave Shreiner, Sixth Edition, Addison-Wesley 2012
--
-- The original code and license can be found at:
-- http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/
-- (a copy of the license is in the moongl/thirdparty/ directory).
---------------------------------------------------------------------------------
--
-- sets up flat mesh 
-- sets up elapsed time parameter for use by shaders
-- vertex shader varies height of mesh sinusoidally
-- uses a pass through fragment shader

glfw = require("moonglfw")
gl = require("moongl")
glmath = require("moonglmath")
vec4 = glmath.vec4
point4 = glmath.vec4

local N = 64

local nearVal = 1.0 -- near distance
local farVal  = 20.0 -- far distance
local program = 0 -- program object id
-- Uniforms' locations:
local timeParam = 0
local MVP_loc = 0
local MV_loc = 0


local data = {}   --float data[N][N]; array of data heights
for i=1, N do -- flat mesh
	data[i] = {}
	for j = 1, N do
		data[i][j] = 0.0
	end
end

-- GLSL initialization

points = {} -- point4 points[6*N*N]
function mesh()
	for i=1, N do
		for j = 1, N do
			points[#points+1] = point4(i/N, data[i][j], j/N, 1.0)
			points[#points+1] = point4(i/N, data[i][j], (j+1)/N, 1.0)
			points[#points+1] = point4((i+1)/N, data[i][j], (j+1)/N, 1.0)
			points[#points+1] = point4((i+1)/N, data[i][j], (j+1)/N, 1.0)
			points[#points+1] = point4((i+1)/N, data[i][j], j/N, 1.0)
			points[#points+1] = point4(i/N, data[i][j], j/N, 1.0)
		end
	end
end

function init()
   mesh()

   -- Create a vertex array object
   local vao = gl.new_vertex_array()

	-- Create and initialize a buffer object
   local buffer = gl.new_buffer("array")
	gl.buffer_data("array", gl.pack("float", points), "static draw")	

	-- Create shading program
	local program = gl.make_program("vertex", "vwave.glsl", "fragment", "fwave.glsl")
	gl.use_program(program)

   -- set up vertex arrays
   local vPosition = gl.get_attrib_location(program, "vPosition")
   gl.enable_vertex_attrib_array(vPosition)
   gl.vertex_attrib_pointer(vPosition, 4, "float", false, 0, 0)

   timeParam = gl.get_uniform_location(program, "time")
   MVP_loc = gl.get_uniform_location(program, "ModelViewProjectionMatrix")
   MV_loc = gl.get_uniform_location(program, "ModelViewMatrix")

   MV=glmath.look_at(vec4(2.0, 2.0, 2.0, 1.0), vec4(0.5, 0.0, 0.5, 1.0), vec4(0.0, 1.0, 0.0, 1.0))
   MVP=glmath.ortho(-0.75,0.75,-0.75,0.75,-5.5,5.5)*MV
   gl.uniform_matrix(MVP_loc, "float", "4x4", true, gl.flatten(MVP))
   gl.uniform_matrix(MV_loc, "float", "4x4", true, gl.flatten(MVP))
    
   gl.clear_color( 1.0, 1.0, 1.0, 1.0 )
   gl.polygon_mode("front and back", "fill")

end

function keyboard(_, key)
	if key=="escape" or key=="q" then
		os.exit()
	end
end

function reshape(_, w, h)
	gl.viewport(0, 0, w, h)
end

-- main
glfw.window_hint("context version major", 3)
glfw.window_hint("context version minor", 3)
glfw.window_hint("opengl profile", 'core')
glfw.window_hint("doublebuffer", true)
window = glfw.create_window(512, 512, "waving mesh example")
glfw.make_context_current(window)
gl.init()

glfw.set_key_callback(window, keyboard)
glfw.set_framebuffer_size_callback(window, reshape)

init()

while not glfw.window_should_close(window) do
	glfw.poll_events()
   -- send elapsed time (in milliseconds) to shaders
	gl.uniform(timeParam, "float", glfw.get_time()*1000)
   gl.clear("color", "depth");
   gl.draw_arrays("triangles", 0, #points) -- 6*N*N
   glfw.swap_buffers(window)
end

