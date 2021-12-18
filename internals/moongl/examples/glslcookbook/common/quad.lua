-- Drawable full-screen quad
local gl = require("moongl")
local glmath = require("moonglmath")

local positions = gl.pack('float', {
     -1.0, -1.0,  0.0, 
      1.0, -1.0,  0.0, 
      1.0,  1.0,  0.0,
     -1.0, -1.0,  0.0, 
      1.0,  1.0,  0.0, 
     -1.0,  1.0,  0.0
   })
local texcoords = gl.pack('float', {
      0.0, 0.0, 
      1.0, 0.0, 
      1.0, 1.0,
      0.0, 0.0, 
      1.0, 1.0, 
      0.0, 1.0
})

local function new()
   local vao = gl.new_vertex_array()
   local p_vbo = gl.new_buffer('array')
   gl.buffer_data('array', positions, 'static draw')
   gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
   gl.enable_vertex_attrib_array(0)  -- Vertex position
   local t_vbo = gl.new_buffer('array')
   gl.buffer_data('array', texcoords, 'static draw')
   gl.vertex_attrib_pointer(2, 2, 'float', false, 0, 0)
   gl.enable_vertex_attrib_array(2)  -- Texture coordinates
   gl.unbind_vertex_array()

   return setmetatable({}, {
      __index = {
         ----------------------------------------------------------------------
         vao = function(plane) return vao end,
         ----------------------------------------------------------------------
         render = function(plane)
            gl.bind_vertex_array(vao)
            gl.draw_arrays('triangles', 0, 6)
            gl.unbind_vertex_array()
         end,
         ----------------------------------------------------------------------
         delete = function(plane)
            gl.delete_vertex_arrays(vao)
            gl.delete_buffers(p_vbo, t_vbo)
            vao, p_vbo, t_vbo = nil
         end,
         ----------------------------------------------------------------------
      },
   })
end

return new

