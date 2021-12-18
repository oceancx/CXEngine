-- Drawable torus
local gl = require("moongl")
local glmath = require("moonglmath")

local vec2, vec3 = glmath.vec2, glmath.vec3
local PI2 = 2*math.pi
local sin, cos = math.sin, math.cos

local function generate(outer_radius, inner_radius, nsides, nrings)
   local positions, normals, texcoords, indices = {}, {}, {}, {}

   -- Generate the vertex data
   for ring = 0, nrings do -- One extra ring to duplicate first ring (nfaces=(nrings+1)*nsides)
      local u = ring*PI2/nrings
      local cu, su = cos(u), sin(u)
      for side = 0, nsides-1 do
         local v = side * PI2 / nsides
         local cv, sv = cos(v), sin(v)
         local r = outer_radius + inner_radius*cv
         table.insert(positions, vec3(r*cu, r*su, inner_radius*sv))
         table.insert(normals, vec3(cv*cu*r, cv*su*r, sv*r):normalize())
         table.insert(texcoords, vec2(u,v)/PI2)
      end
   end

   -- Generate the indices (must be 0-based)
   for ring = 0, nrings-1 do
      local ringStart = ring * nsides
      local nextRingStart = (ring + 1) * nsides
      for side = 0, nsides-1 do
         local nextSide = (side+1)%nsides
            -- The quad
            table.insert(indices, ringStart + side)
            table.insert(indices, nextRingStart + side)
            table.insert(indices, nextRingStart + nextSide)
            table.insert(indices, ringStart + side)
            table.insert(indices, nextRingStart + nextSide)
            table.insert(indices, ringStart + nextSide)
      end
   end

   -- Convert data to binary strings:
   return gl.pack('float', positions), gl.pack('float', normals),
          gl.pack('float', texcoords), gl.pack('uint', indices)
end

-------------------------------------------------------------------------------
local function new(outer_radius, inner_radius, nsides, nrings)
   local positions, normals, texcoords, indices =
      generate(outer_radius, inner_radius, nsides, nrings)
   
   -- Create and populate the buffers
   local p_vbo = gl.new_buffer('array')
   gl.buffer_data('array', positions, 'static draw')
   local n_vbo = gl.new_buffer('array')
   gl.buffer_data('array', normals, 'static draw')
   local tc_vbo = gl.new_buffer('array')
   gl.buffer_data('array', texcoords, 'static draw')
   local ebo = gl.new_buffer('element array')
   gl.buffer_data('element array', indices, 'static draw')

   -- Create the vertex array object and populate it
   local vao = gl.new_vertex_array()
   gl.enable_vertex_attrib_array(0) -- position
   gl.bind_buffer('array', p_vbo)
   gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
   gl.enable_vertex_attrib_array(1) -- normal
   gl.bind_buffer('array', n_vbo)
   gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)
   gl.enable_vertex_attrib_array(2) -- texcoords
   gl.bind_buffer('array', tc_vbo)
   gl.vertex_attrib_pointer(2, 2, 'float', false, 0, 0)
   gl.bind_buffer('element array', ebo) -- indices
   gl.unbind_vertex_array()

   local count = #indices/gl.sizeof('uint') -- no. of indices

   return setmetatable({}, {
      __index = {
      ------------------------------------------------
      vao = function(self) return vao end,
      positions = function(self) return positions end,
      normals = function(self) return normals end,
      texcoords = function(self) return texcoords end,
      indices = function(self) return indices end,
      ------------------------------------------------
      render = function(self)
         gl.bind_vertex_array(vao)
         gl.draw_elements('triangles', count, 'uint', 0)
         gl.unbind_vertex_array()
      end,
      ------------------------------------------------
      delete = function(self)
         if vao then
            gl.delete_vertex_arrays(vao)
            gl.delete_buffers(p_vbo, n_vbo, tc_vbo, ebo)
            vao, positions, normals, texcoords, indices = nil
         end
      end,
      ------------------------------------------------
      },
      __gc = function(self) self:delete() end,
   })
end

return new
