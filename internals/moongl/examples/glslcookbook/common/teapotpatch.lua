local gl = require("moongl")
local glmath = require("moonglmath")
local teapotdata = require("common.teapotdata")

local mat3 = glmath.mat3

local function GetPatch(patch_num)
	local patchdata = teapotdata.patchdata[patch_num]
	local positions = teapotdata.positions
	local patch1 = {{}, {}, {}, {}}
	local patch2 = {{}, {}, {}, {}} -- reverse v
	local pos
	for i = 1, 4 do
		for j = 1, 4 do
			patch1[i][j] = positions[patchdata[4*i+j-4]+1]
			patch2[i][j] = positions[patchdata[4*i-j+1]+1]
		end
	end
	return patch1, patch2
end

local insert = table.insert
local function BuildPatch(patch, positions, reflect)
   for i = 1, 4 do
		for j = 1, 4 do
			table.insert(positions, reflect*patch[i][j])
		end
	end
end

local I = mat3()
local Rx = mat3(-1,0,0, 0,1,0, 0,0,1)
local Ry = mat3(1,0,0, 0,-1,0, 0,0,1)
local Rxy = mat3(-1,0,0, 0,-1,0, 0,0,1)

local function BuildPatchReflect(patch_num, positions, reflect_x, reflect_y)
	local patch, patchRevV = GetPatch(patch_num)
   BuildPatch(patch, positions, I)
   if reflect_x then BuildPatch(patchRevV, positions, Rx) end -- reflected in x
   if reflect_y then BuildPatch(patchRevV, positions, Ry) end -- reflected in y
   if reflect_x and reflect_y then BuildPatch(patch, positions, Rxy) end
end

local function GeneratePatches()
	local positions = {}

   -- Build each patch
	io.write("Building teapot patches: ")
	io.flush()
	io.write("rim, ")
	io.flush()
   BuildPatchReflect(1, positions, true, true)  -- rim
	io.write("body, ")
	io.flush()
   BuildPatchReflect(2, positions, true, true)  -- body
   BuildPatchReflect(3, positions, true, true)
	io.write("lid, ")
	io.flush()
   BuildPatchReflect(4, positions, true, true)  -- lid
   BuildPatchReflect(5, positions, true, true)
	io.write("bottom, ")
	io.flush()
   BuildPatchReflect(6, positions, true, true)  -- bottom
	io.write("handle, ")
	io.flush()
   BuildPatchReflect(7, positions, false, true) -- handle
   BuildPatchReflect(8, positions, false, true)
	io.write("spout, ")
	io.flush()
   BuildPatchReflect(9, positions, false, true) -- spout
   BuildPatchReflect(10, positions, false, true)
	print("done.")

	return gl.pack('float', positions)
end

-------------------------------------------------------------------------------
return function()
-------------------------------------------------------------------------------
   local positions = GeneratePatches()
	local vao = gl.new_vertex_array()
	local pos_vbo = gl.new_buffer('array')
	gl.buffer_data('array', positions, 'static draw')
   gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)
	gl.enable_vertex_attrib_array(0)
   gl.unbind_vertex_array()
   positions = nil

	return setmetatable({}, {
		__index = {
			----------------------------------------------------------------------
			vao = function(teapot) return vao end,
		   ----------------------------------------------------------------------
			render = function(teapot)
    			gl.patch_parameter('vertices', 16)
   	 		gl.bind_vertex_array(vao)
    			gl.draw_arrays('patches', 0, 512)
			end,
		   ----------------------------------------------------------------------
			delete = function(teapot)
   	 		gl.delete_vertex_arrays(vao)
				gl.delete_buffers(pos_vbo)
				vao, pos_vbo = nil
			end,
		   ----------------------------------------------------------------------
		},
		__gc = function(self) self:delete() end,
	})
end

