-- Drawable cube
local gl = require("moongl")
local glmath = require("moonglmath")

local normals = gl.pack('float', {
    0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, -- Front
    1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, -- Right
    0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, -- Back
   -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -- Left
    0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, -- Bottom
    0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0 -- Top
})

local texcoords = gl.pack('float', {
	0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -- Front
   0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -- Right
   0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -- Back
   0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -- Left
   0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -- Bottom
   0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0   -- Top
})

local indices = gl.pack('uint', {
   0,1,2,0,2,3,
   4,5,6,4,6,7,
   8,9,10,8,10,11,
   12,13,14,12,14,15,
   16,17,18,16,18,19,
   20,21,22,20,22,23
})

local function generate(side)
	local h = side/2 -- half side
	local positions = gl.pack('float', {
      -h, -h,  h,  h, -h,  h,  h,  h,  h, -h,  h,  h, -- Front
       h, -h,  h,  h, -h, -h,  h,  h, -h,  h,  h,  h, -- Right
      -h, -h, -h, -h,  h, -h,  h,  h, -h,  h, -h, -h, -- Back
      -h, -h,  h, -h,  h,  h, -h,  h, -h, -h, -h, -h, -- Left
      -h, -h,  h, -h, -h, -h,  h, -h, -h,  h, -h,  h, -- Bottom
      -h,  h,  h,  h,  h,  h,  h,  h, -h, -h,  h, -h, -- Top
    })
	return positions, normals, texcoords, indices
end

-------------------------------------------------------------------------------
local function new(side)
	local positions, normals, texcoords, indices = generate(side or 1.0)
	
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
