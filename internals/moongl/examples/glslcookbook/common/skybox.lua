-- Drawable skybox
local gl = require("moongl")
local glmath = require("moonglmath")

local indices = gl.pack('uint', {
    0,2,1,0,3,2,
    4,6,5,4,7,6,
    8,10,9,8,11,10,
    12,14,13,12,15,14,
    16,18,17,16,19,18,
    20,22,21,20,23,22
})

local function generate(side)
	local h = side/2 -- half side
	local positions = gl.pack('float', {
      -h, -h, h, h, -h, h, h, h, h, -h,  h, h, -- Front
       h, -h, h, h, -h, -h, h, h, -h, h,  h, h, -- Right
      -h, -h, -h, -h,  h, -h, h,  h, -h, h, -h, -h, -- Back
      -h, -h, h, -h,  h, h, -h,  h, -h, -h, -h, -h, -- Left
      -h, -h, h, -h, -h, -h, h, -h, -h, h, -h, h, -- Bottom
      -h,  h, h,  h,  h, h, h, h, -h, -h,  h, -h -- Top
	})
	return positions, indices
end


-------------------------------------------------------------------------------
local function new(side)
	local positions, indices = generate(side or 50.0)
	
	-- Create and populate the buffers
	local p_vbo = gl.new_buffer('array')
	gl.buffer_data('array', positions, 'static draw')
	local ebo = gl.new_buffer('element array')
	gl.buffer_data('element array', indices, 'static draw')

	-- Create the vertex array object and populate it
	local vao = gl.new_vertex_array()
	gl.enable_vertex_attrib_array(0) -- position
	gl.bind_buffer('array', p_vbo)
	gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
	gl.bind_buffer('element array', ebo) -- indices
	gl.unbind_vertex_array()

	local count = #indices/gl.sizeof('uint') -- no. of indices

	return setmetatable({}, {
		__index = {
		------------------------------------------------
		vao = function(self) return vao end,
		positions = function(self) return positions end,
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
				gl.delete_buffers(p_vbo, ebo)
				vao, positions, indices = nil
			end
		end,
		------------------------------------------------
		},
		__gc = function(self) self:delete() end,
	})
end

return new
