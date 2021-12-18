-- Drawable plane
local gl = require("moongl")
local glmath = require("moonglmath")

local vec2, vec3, vec4 = glmath.vec2, glmath.vec3, glmath.vec4

local function generate(xsize, zsize, xdivs, zdivs, smax, tmax)
	local positions, normals, tangents, texcoords, indices = {}, {}, {}, {}, {}

	local npoints = (xdivs+1)*(zdivs+1)
	local iFactor, jFactor = zsize/zdivs, xsize/xdivs
   local texi, texj = smax/xdivs, tmax/zdivs
   local x, z
   for i = 0, zdivs do
		z = iFactor*i - zsize/2
      for j = 0, xdivs do
			x = jFactor*j - xsize/2
        	table.insert(positions, vec3(x, 0, z))
        	table.insert(normals, vec3(0, 1, 0))
        	table.insert(texcoords, vec2(j*texi, (zdivs-i)*texj))
		end
	end

	for i = 1, npoints do
		table.insert(tangents, vec4(1, 0, 0, 1))
	end

   local row_start, next_row_start
   for i = 0, zdivs-1 do
      row_start = i*(xdivs+1)
		next_row_start = (i+1)*(xdivs+1)
		for j = 0, zdivs-1 do
			  table.insert(indices, 
			  		{ row_start+j, next_row_start+j, next_row_start+j+1,
					  row_start+j, next_row_start+j+1, row_start+j+1})
		end
	end

	-- Convert data to binary strings:
	return gl.pack('float', positions), gl.pack('float', normals), gl.pack('float', tangents),
			 gl.pack('float', texcoords), gl.pack('uint', indices)
end


-------------------------------------------------------------------------------
local function new(xsize, zsize, xdivs, zdivs, smax, tmax)
	local positions, normals, tangents, texcoords, indices =
		generate(xsize, zsize, xdivs, zdivs, smax or 1.0, tmax or 1.0)
	
	-- Create and populate the buffers
	local p_vbo = gl.new_buffer('array')
	gl.buffer_data('array', positions, 'static draw')
	local n_vbo = gl.new_buffer('array')
	gl.buffer_data('array', normals, 'static draw')
	local tg_vbo = gl.new_buffer('array')
	gl.buffer_data('array', tangents, 'static draw')
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
	gl.enable_vertex_attrib_array(3) -- tangents
	gl.bind_buffer('array', tg_vbo)
	gl.vertex_attrib_pointer(3, 4, 'float', false, 0, 0)
	gl.bind_buffer('element array', ebo) -- indices
	gl.unbind_vertex_array()

	local count = #indices/gl.sizeof('uint') -- no. of indices

	return setmetatable({}, {
		__index = {
		------------------------------------------------
		vao = function(self) return vao end,
		positions = function(self) return positions end,
		normals = function(self) return normals end,
		tangents = function(self) return tangents end,
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
				gl.delete_buffers(p_vbo, n_vbo, tg_vbo, tc_vbo, ebo)
				vao, positions, normals, tangents, texcoords, indices = nil
			end
		end,
		------------------------------------------------
		},
		__gc = function(self) self:delete() end,
	})
end

return new
