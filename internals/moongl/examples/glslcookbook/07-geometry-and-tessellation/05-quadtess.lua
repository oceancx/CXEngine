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
local max = math.max
local fmt = string.format
local printf = function(...) io.write(fmt(...)) end

local TITLE = "Chapter 7 - Quad tessellation"
local W, H = 800, 600

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = pi/4, pi/8 -- rad, rad/s
local animate = false

local inner, outer = 4, 4 -- tessellation levels
glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then animate = not animate
   elseif key == 'down' and action ~= 'release' then inner = max(inner-1, 1)
   elseif key == 'up' and action ~= 'release' then inner = inner+1
   elseif key == 'left' and action ~= 'release' then outer = max(outer-1, 1)
   elseif key == 'right' and action ~= 'release' then outer = outer+1
   end
end)

local projection, viewport
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   local c = 3.5
   projection = glmath.ortho(-0.4*c, 0.4*c, -0.3*c, 0.3*c, 0.1, 100.0)
   viewport = mat4(w/2,   0,  0, w/2,
                     0, h/2,  0, h/2,
                     0,   0,  1,   0,
                     0,   0,  0,   1)
end

glfw.set_window_size_callback(window, resize)

-- Create the shader program
local prog, vsh, gsh, tcsh, tesh, fsh = gl.make_program(
   'vertex', "shaders/quadtess.vert",
   'geometry', "shaders/quadtess.geom",
   'tess control', "shaders/quadtess.tesc",
   'tess evaluation', "shaders/quadtess.tese",
   'fragment', "shaders/quadtess.frag")

gl.delete_shaders(vsh, gsh, tcsh, tesh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
   "Inner",
   "Outer",
   "LineWidth",
   "LineColor",
   "QuadColor",
   "MVP",
   "ViewportMatrix",
}

local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
resize(window, W, H) -- creates projection

local function set_matrices(model, view, projection, viewport)
   gl.uniform_matrix4f(loc["ViewportMatrix"], true, viewport)
   gl.uniform_matrix4f(loc["MVP"], true, projection * view * model)
end

gl.uniformf(loc["LineWidth"], 1.5)
gl.uniformf(loc["LineColor"], .05, 0, .05, 1.0)
gl.uniformf(loc["QuadColor"], 1.0, 1.0, 1.0, 1.0)

-- Set up the patch VAO
local vao = gl.new_vertex_array()
local vbo = gl.new_buffer('array')
local positions = gl.pack('float', {-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0})
gl.buffer_data('array', positions, 'static draw')
gl.vertex_attrib_pointer(0, 2, 'float', false, 0, 0)
gl.enable_vertex_attrib_array(0)
gl.unbind_vertex_array()
-- Set the number of vertices per patch.  IMPORTANT!!
gl.patch_parameter('vertices', 4)
printf("Max patch vertices: %d\n", gl.get("max patch vertices"))

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(.5, .5, .5, 1)

print("Press space to toggle animation on/off")
print("Press up/down/left/right to increase/decrease the tessellation levels")

local t0 = glfw.now()

while not glfw.window_should_close(window) do
   glfw.poll_events()

   local t = glfw.now()
   local dt = t - t0
   t0 = t
   if animate then
      angle = angle + speed*dt
      if angle >= 2*pi then angle = angle - 2*pi end
   end

   gl.uniformi(loc["Inner"], inner)
   gl.uniformi(loc["Outer"], outer)
   glfw.set_window_title(window, TITLE.." inner/outer="..inner.."/"..outer)
 
   gl.clear('color', 'depth')
   local model = glmath.rotate(angle, 0, 1, 0)
   local view = glmath.look_at(vec3(0,0,1.5), vec3(0,0,0), vec3(0,1,0))
   set_matrices(model, view, projection, viewport)
   gl.bind_vertex_array(vao)
   gl.draw_arrays('patches', 0, 4)
   gl.finish()

   glfw.swap_buffers(window)
end

