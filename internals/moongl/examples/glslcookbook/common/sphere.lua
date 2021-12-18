-- Drawable sphere
local gl = require("moongl")
local glmath = require("moonglmath")

local pi, sin, cos = math.pi, math.sin, math.cos

local function generate(radius, nslices, nstacks)
   -- nverts = (nslices+1) * (nstacks+1)
   -- nindices = (nslices * 2 * (nstacks-1) ) * 3;
   local positions, normals, texcoords, indices = {}, {}, {}, {}

   -- Generate positions and normals
   for i=0, nslices do
      local theta = i * 2*pi/nslices
      local s = i/nslices
      for j = 0, nstacks do
         local phi = j*pi/nstacks
         local t = j/nstacks
         local nx, ny, nz = sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi)
         table.insert(positions, {radius*nx, radius*ny, radius*nz})
         table.insert(normals, {nx, ny, nz})
         table.insert(texcoords, {s, t})
      end
   end

   -- Generate the element list
   for i = 0, nslices-1 do
      local stackStart = i * (nstacks+1)
      local nextStackStart = (i+1) * (nstacks+1)
      for j = 0, nstacks-1 do
         if j == 0 then 
            table.insert(indices, {stackStart, stackStart+1, nextStackStart+1})
         elseif j == nstacks - 1 then
            table.insert(indices, {stackStart+j, stackStart+j+1, nextStackStart+j})
         else
            table.insert(indices, {stackStart+j, stackStart+j+1, nextStackStart+j+1})
            table.insert(indices, {nextStackStart+j, stackStart+j, nextStackStart+j+1})
         end
      end
   end

   -- Convert data to binary strings:
   return gl.pack('float', positions), gl.pack('float', normals),
          gl.pack('float', texcoords), gl.pack('uint', indices)
end

-------------------------------------------------------------------------------
local function new(radius, nslices, nstacks)
   local positions, normals, texcoords, indices = generate(radius, nslices, nstacks)
   
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
