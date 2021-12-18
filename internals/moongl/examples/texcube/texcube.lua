#!/usr/bin/env lua
-- MoonGL example: texcube.lua
--------------------------------------------------------------------------------
-- This example is derived from the example code that comes with the book:
-- "Interactive Computer Graphics - A Top-Down Approach with OpenGL"
-- by Edward Angel and Dave Shreiner, Sixth Edition, Addison-Wesley 2012
--
-- The original code and license can be found at:
-- http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/
-- (a copy of the license is in the moongl/thirdparty/ directory).
--
-- The texturing part is derived from the examples from the on-line book
-- "Learn OpenGL" by Joey de Vries (http://learnopengl.com).
--
---------------------------------------------------------------------------------
--
-- Display a textured cube
--

gl = require("moongl")
glfw = require("moonglfw")
glmath = require("moonglmath")
mi = require("moonimage")

vec2 = glmath.vec2
vec3 = glmath.vec3
vec4 = glmath.vec4
float = gl.sizeof('float')

local positions = {} -- list of vertex positions
local tex_coords = {} -- list of vertex texture coordinates

-- Vertices of a unit cube centered at origin, sides aligned with axes:
local vertices = {
    vec4( -0.5, -0.5,  0.5, 1.0 ), -- 1
    vec4(  0.5, -0.5,  0.5, 1.0 ), -- 2            5 _____ 6           y
    vec4(  0.5,  0.5,  0.5, 1.0 ), -- 3            /:    /|            |
    vec4( -0.5,  0.5,  0.5, 1.0 ), -- 4         4 /____ /3|            |____x
    vec4( -0.5,  0.5, -0.5, 1.0 ), -- 5          |  :...|.|           /
    vec4(  0.5,  0.5, -0.5, 1.0 ), -- 6          | .8   | /7         /
    vec4(  0.5, -0.5, -0.5, 1.0 ), -- 7          |______|/          z
    vec4( -0.5, -0.5, -0.5, 1.0 ), -- 8         1       2
}

local vertex_tex_coords = {
   vec2(0.0, 0.0), -- 1   4___3
   vec2(1.0, 0.0), -- 2   |  /|
   vec2(1.0, 1.0), -- 3   |/__|
   vec2(0.0, 1.0), -- 4   1   2
}

local function add_quad(a, b, c, d)
-- generates two triangles for each face and assigns tex_coords to the vertices
--  d___c
--  |  /|
--  |/__|
--  a   b
   positions[#positions+1] = vertices[a]
   positions[#positions+1] = vertices[b]
   positions[#positions+1] = vertices[c]
   positions[#positions+1] = vertices[a]
   positions[#positions+1] = vertices[c]
   positions[#positions+1] = vertices[d]
   tex_coords[#tex_coords+1] = vertex_tex_coords[1]
   tex_coords[#tex_coords+1] = vertex_tex_coords[2]
   tex_coords[#tex_coords+1] = vertex_tex_coords[3]
   tex_coords[#tex_coords+1] = vertex_tex_coords[1]
   tex_coords[#tex_coords+1] = vertex_tex_coords[3]
   tex_coords[#tex_coords+1] = vertex_tex_coords[4]
end

function makecube() 
-- generates 12 triangles
   add_quad(1,2,3,4)
   add_quad(4,3,6,5)
   add_quad(5,6,7,8)
   add_quad(8,7,2,1)
   add_quad(2,7,6,3)
   add_quad(8,1,4,5)
end


------------------------------------------------------------------------------

-- Array of rotation angles (in degrees) for each coordinate axis
local Axis = 'x'
local Theta = vec3( 30, 30, 30 )
local theta_loc = 0  -- The location of the "theta" shader uniform variable

------------------------------------------------------------------------------

-- OpenGL initialization
function init()
   makecube()
   sizeof_positions = #positions * 4 * float
   sizeof_tex_coords = #tex_coords * 2 * float

   -- Create a vertex array object
   local vao = gl.new_vertex_array()

   -- Create a buffer object and load the vertex data:
   local buffer = gl.new_buffer('array');
   gl.buffer_data('array', sizeof_positions + sizeof_tex_coords, 'static draw')
   local offset = gl.buffer_sub_data('array', 0, gl.pack('float', positions))
   gl.buffer_sub_data('array', offset, gl.pack('float', tex_coords))
   
   num_vertices = #positions
   -- release some memory
   positions, tex_coords = nil, nil 
   collectgarbage()
   

   -- Load shaders and compile the shader program
   program, vsh, fsh = gl.make_program('vertex', "vshader.glsl", 'fragment', "fshader.glsl")
   gl.delete_shaders(vsh, fsh)

   gl.use_program(program)

   -- Set up vertex arrays
   local vPosition = gl.get_attrib_location(program, "vPosition")
   gl.enable_vertex_attrib_array( vPosition )
   gl.vertex_attrib_pointer( vPosition, 4, 'float', false, 0, 0)

   local vTexCoords = gl.get_attrib_location(program, "vTexCoords")
   gl.enable_vertex_attrib_array( vTexCoords )
   gl.vertex_attrib_pointer( vTexCoords, 2, 'float', false, 0, offset)


--[[
   -- See: http://stackoverflow.com/questions/7380773/glteximage2d-segfault-related-to-width-height
   gl.pixel_store('unpack alignment', 1)
   gl.pixel_store('unpack row length', 0)
   gl.pixel_store('unpack skip pixels', 0)
   gl.pixel_store('unpack skip rows', 0)
--]]

   -- Load the first texture (marble)
   texture1 = gl.new_texture('2d')

   gl.texture_parameter('2d', 'wrap s', 'repeat')
   gl.texture_parameter('2d', 'wrap t', 'repeat')
   gl.texture_parameter('2d', 'min filter', 'linear')
   gl.texture_parameter('2d', 'mag filter', 'linear')
   image, width, height = mi.load("marble.jpg", 'rgb')
   gl.texture_image('2d', 0, 'rgb', 'rgb', 'ubyte', image, width, height)
   gl.generate_mipmap('2d')
   gl.unbind_texture('2d')

   -- Load the second texture ('powered by Lua' logo)
   texture2 = gl.new_texture('2d')
   gl.texture_parameter('2d', 'wrap s', 'repeat')
   gl.texture_parameter('2d', 'wrap t', 'repeat')
   gl.texture_parameter('2d', 'min filter', 'linear')
   gl.texture_parameter('2d', 'mag filter', 'linear')
   image, width, height = mi.load("powered-by-lua.png", 'rgb')
   gl.texture_image('2d', 0, 'rgb', 'rgb', 'ubyte', image, width, height)
   gl.generate_mipmap('2d')
   gl.unbind_texture('2d')

   -- Clean up some memory:
   image = nil
   collectgarbage()
 
   -- Get the location of the theta uniform variable (we will need it later):
   theta_loc = gl.get_uniform_location( program, "theta" )
   gl.uniform(theta_loc, 'float', Theta.x, Theta.y, Theta.z)
    
   gl.enable( 'depth test')

   gl.clear_color(0.82, 0.82, 1.0, 1.0) -- Lua background color

end

-- callbacks -----------------------------------------------------------------

function keyboard(_, key)
   if key=='escape' or key=='q' then
      glfw.set_window_should_close(window, true)
   end
end

function mouse(_, button, action )
   if action == 'press' then
      if button == 'left' then Axis = 'x'
      elseif button == 'middle' then Axis = 'y'
      elseif button == 'right' then Axis = 'z'
      end
   end
end

function reshape(_, w, h)
   gl.viewport(0, 0, w, h)
end

-- main ------------------------------------------------------------------------
glfw.window_hint("context version major", 3)
glfw.window_hint("context version minor", 3)
glfw.window_hint("opengl profile", 'core')
window = glfw.create_window(512, 512, "Color Cube")
glfw.make_context_current(window)
gl.init()
init()

glfw.set_key_callback(window, keyboard)
glfw.set_mouse_button_callback(window, mouse)
glfw.set_framebuffer_size_callback(window, reshape)

print("Use mouse buttons to change the rotation axis (left=x, middle=y, right=z)")

while not glfw.window_should_close(window) do
   glfw.poll_events()

   Theta[Axis] = Theta[Axis] + 0.5
   if Theta[Axis] > 360.0 then
      Theta[Axis] = Theta[Axis] - 360.0
   end

   gl.clear( 'color', 'depth' )

   -- Bind textures using texture units:
   gl.active_texture(0)
   gl.bind_texture('2d', texture1)
   gl.uniform(gl.get_uniform_location(program, "Texture1"), 'int', 0)
   gl.active_texture(1)
   gl.bind_texture('2d', texture2)
   gl.uniform(gl.get_uniform_location(program, "Texture2"), 'int', 1)
 
   -- load the new angles:
   gl.uniform(theta_loc, 'float', Theta.x, Theta.y, Theta.z)
   -- eventually, draw the cube:
   gl.draw_arrays( 'triangles', 0, num_vertices)
   glfw.swap_buffers(window)
end

