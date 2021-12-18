#!/usr/bin/env lua
-- MoonGL example: colorcube.lua
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
-- Display a color cube
--
-- Colors are assigned to each vertex and then the rasterizer interpolates
-- those colors across the triangles. We use an orthographic projection as
-- the default projetion.
-- In this version, we send the angles to the vertex shader and compute 
-- the rotation matrix there.

glut = require("moonglut") -- Note that MoonGLUT is now discontinued (use MoonGLFW instead).
glmath = require("moonglmath")
gl = require("moongl")

point4 = glmath.vec4
color4 = glmath.vec4
point4sz = 4*gl.sizeof('float')
color4sz = point4sz 

local points = {}
local colors = {}

-- Vertices of a unit cube centered at origin, sides aligned with axes
local vertices = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
}

-- RGBA colors
local vertex_colors = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  -- black
    color4( 1.0, 0.0, 0.0, 1.0 ),  -- red
    color4( 1.0, 1.0, 0.0, 1.0 ),  -- yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  -- green
    color4( 0.0, 0.0, 1.0, 1.0 ),  -- blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  -- magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  -- white
    color4( 0.0, 1.0, 1.0, 1.0 )   -- cyan
}

-- Array of rotation angles (in degrees) for each coordinate axis
local Xaxis = 1
local Yaxis = 2
local Zaxis = 3
local Axis = Xaxis
local Theta = { 30.0, 30.0, 30.0 }

local theta = 0  -- The location of the "theta" shader uniform variable
------------------------------------------------------------------------------

-- quad generates two triangles for each face and assigns colors to the vertices
function quad( a, b, c, d )
    colors[#colors+1] = vertex_colors[a]; points[#points+1] = vertices[a]
    colors[#colors+1] = vertex_colors[b]; points[#points+1] = vertices[b]
    colors[#colors+1] = vertex_colors[c]; points[#points+1] = vertices[c]
    colors[#colors+1] = vertex_colors[a]; points[#points+1] = vertices[a]
    colors[#colors+1] = vertex_colors[c]; points[#points+1] = vertices[c]
    colors[#colors+1] = vertex_colors[d]; points[#points+1] = vertices[d]
end

------------------------------------------------------------------------------

-- generate 12 triangles: 36 vertices and 36 colors
function colorcube()
    quad( 2, 1, 4, 3 )
    quad( 3, 4, 8, 7 )
    quad( 4, 1, 5, 8 )
    quad( 7, 6, 2, 3 )
    quad( 5, 6, 7, 8 )
    quad( 6, 5, 1, 2 )
end

------------------------------------------------------------------------------

-- OpenGL initialization
function init()
   colorcube();

   -- Create a vertex array object
   local vao = gl.new_vertex_array()

   -- Create and initialize a buffer object
   local buffer = gl.new_buffer('array');
   gl.buffer_data('array', #points*point4sz + #colors*color4sz, 'static draw')
   local offset = gl.buffer_sub_data('array', 0, gl.pack('float', points))
   gl.buffer_sub_data('array', offset, gl.pack('float', colors))

   -- Load shaders and use the resulting shader program
   local program = gl.make_program('vertex', "vshader.glsl", 'fragment', "fshader.glsl")
   gl.use_program(program)

   -- set up vertex arrays
   local vPosition = gl.get_attrib_location(program, "vPosition")
   gl.enable_vertex_attrib_array( vPosition )
   gl.vertex_attrib_pointer( vPosition, 4, 'float', false, 0, 0)

   local vColor = gl.get_attrib_location(program, "vColor")
   gl.enable_vertex_attrib_array( vColor )
   gl.vertex_attrib_pointer( vColor, 4, 'float', false, 0, offset)

   theta = gl.get_uniform_location( program, "theta" )
    
   gl.enable( 'depth test')
   gl.clear_color( 1.0, 1.0, 1.0, 1.0 )
end

------------------------------------------------------------------------------

function display()
   gl.clear( 'color', 'depth' )
    
   gl.uniform( theta, 'float', gl.flatten(Theta))
   gl.draw_arrays( 'triangles', 0, #points) -- NumVertices

   glut.swap_buffers()
end

------------------------------------------------------------------------------

function keyboard(_, key)
   if key=='esc' or key=='q' or key=='Q' then
      os.exit()
   end
end

------------------------------------------------------------------------------

function mouse(_, button, state )
   if state == 'down' then
      if button == 'left' then Axis = Xaxis
      elseif button == 'middle' then Axis = Yaxis
      elseif button == 'right' then Axis = Zaxis
      end
   end
end

------------------------------------------------------------------------------

function idle()
   Theta[Axis] = Theta[Axis] + 0.5
   if Theta[Axis] > 360.0 then
      Theta[Axis] = Theta[Axis] - 360.0
   end
   glut.post_redisplay()
end

-- main ------------------------------------------------------------------------
glut.init( arg )
glut.init_display_mode( 'rgba', 'double', 'depth' )
glut.init_window_size( 512, 512 )
glut.create_window( "Color Cube" )
gl.init()
init()
glut.display_func( display )
glut.keyboard_func( keyboard )
glut.mouse_func( mouse )
glut.idle_func( idle )
print("Use mouse buttons to change the rotation axis (left=x, middle=y, right=z)")
glut.main_loop()

