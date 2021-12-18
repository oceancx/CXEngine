#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad
local sin, cos = math.sin, math.cos
local exp, log = math.exp, math.log
local fmt = string.format

local TITLE = "Chapter 7 - Bezier curve"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/2, pi/8 -- rad, rad/s
local animate = false
local num_segments = 50
glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then
      animate = not animate
   elseif key == 'up' and action == 'press' then
      num_segments = num_segments+1
   elseif key == 'down' and action == 'press' and num_segments > 0 then
      num_segments = num_segments-1
   end
end)

local projection
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   local c = 3.5
   projection = glmath.ortho(-0.4*c, 0.4*c, -0.3*c, 0.3*c, 0.1, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader programs
local prog1, vsh, fsh, tcsh, tesh = gl.make_program( -- draws the curve
   'vertex', "shaders/bezcurve.vert",
   'tess control', "shaders/bezcurve.tesc",
   'tess evaluation', "shaders/bezcurve.tese",
   'fragment', "shaders/bezcurve.frag")
gl.delete_shaders(vsh, tcsh, tesh, fsh)
local prog2, vsh, fsh = gl.make_program( -- draws the control points
   'vertex', "shaders/solid.vert",
   'fragment', "shaders/solid.frag")
gl.delete_shaders(vsh, fsh)

-- Get the locations of the uniform variables
local uniforms1 = { "NumSegments", "NumStrips", "LineColor", "MVP" }
local loc1 = {}
for _,name in ipairs(uniforms1) do loc1[name] = gl.get_uniform_location(prog1, name) end
local uniforms2 = { "Color", "MVP", }
local loc2 = {}
for _,name in ipairs(uniforms2) do loc2[name] = gl.get_uniform_location(prog2, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection)
   local mvp = projection * view * model
   gl.use_program(prog1)
   gl.uniform_matrix4f(loc1["MVP"], true, mvp)
   gl.use_program(prog2)
   gl.uniform_matrix4f(loc2["MVP"], true, mvp)
end

gl.use_program(prog1)
gl.uniformi(loc1["NumStrips"], 1)
gl.uniformf(loc1["LineColor"], 1.0, 1.0, 0.5, 1.0)
gl.use_program(prog2)
gl.uniformf(loc2["Color"], 0.5, 1.0, 1.0, 1.0)


-- Set up the VBO for the patch points (control points)
local points = {
   -1.0, -1.0,
   -0.5,  1.0, 
    0.5, -1.0, 
    1.0,  1.0
 }

local vao = gl.new_vertex_array()
local vbo = gl.new_buffer('array')
gl.buffer_data('array', gl.pack('float', points), 'static draw')
gl.vertex_attrib_pointer(0, 2, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(0)
gl.unbind_vertex_array()

-- Event loop -----------------------------------------------------------------

--print("Press space to toggle animation on/off")
print("Press up/down to increase/decrease the number of segments")

gl.enable('depth test')
gl.point_size(10.0)
-- Set the number of vertices per patch.  IMPORTANT!!
gl.patch_parameter('vertices', 4)
   
local camera_pos = vec3(0.0,0.0,1.5)
local view = glmath.look_at(camera_pos, vec3(0,0,0), vec3(0,1,0))
local model = mat4()

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

   gl.clear('color', 'depth')
   glfw.set_window_title(window, TITLE.." (segments="..num_segments..")")
   set_matrices(model, view, projection)
   gl.bind_vertex_array(vao)
   -- Draw the curve
   gl.use_program(prog1)
   gl.uniformi(loc1["NumSegments"], num_segments)
   gl.draw_arrays('patches', 0, 4)
   -- Draw the control points
   gl.use_program(prog2)
   gl.draw_arrays('points', 0, 4)

   gl.finish()

   glfw.swap_buffers(window)
end

