-- Texture utilities
local gl = require("moongl")
local mi = require("moonimage")

----------------------------------------------------------
local function load_texture(filename)
----------------------------------------------------------
   mi.flip_vertically_on_load(true)
	local data, width, height = mi.load(filename, 'rgba')
	--print(#data, width, height, width*height*4, filename)
	local texid = gl.new_texture('2d')
	gl.texture_storage('2d', 1, 'rgba8', width, height)
	gl.texture_sub_image('2d', 0, 'rgba', 'ubyte', data, 0, 0, width, height)
	gl.texture_parameter('2d', 'mag filter', 'linear')
	gl.texture_parameter('2d', 'min filter', 'nearest')
	gl.unbind_texture('2d')
	return texid, width, height
end

local SUFFIX = { "posx", "negx", "posy", "negy", "posz", "negz" }
local TARGET = { 'cube map positive x', 'cube map negative x',
	              'cube map positive y', 'cube map negative y',
					  'cube map positive z', 'cube map negative z' }


----------------------------------------------------------
local function load_cube_map(basename, extension)
----------------------------------------------------------
	local extension = estension or "png"
	local texid = gl.new_texture('cube map')
   mi.flip_vertically_on_load(false)
	local data, width, height
   for i=1,6 do
   	local filename = basename .."_"..SUFFIX[i].."."..extension
		data, width, height = mi.load(filename, 'rgba')
		if i == 1 then -- Allocate immutable storage for the whole cube map texture
			gl.texture_storage('cube map', 1, 'rgba8', width, height)
		end
		gl.texture_sub_image(TARGET[i], 0, 'rgba', 'ubyte', data, 0, 0, width, height)
	end
	gl.texture_parameter('cube map', 'mag filter', 'linear')
	gl.texture_parameter('cube map', 'min filter', 'nearest')
	gl.texture_parameter('cube map', 'wrap s', 'clamp to edge')
	gl.texture_parameter('cube map', 'wrap t', 'clamp to edge')
	gl.texture_parameter('cube map', 'wrap r', 'clamp to edge')
	gl.unbind_texture('cube map')
	return texid, width, height
end


----------------------------------------------------------
local function load_hdr_cube_map(basename)
----------------------------------------------------------
	local texid = gl.new_texture('cube map')
-- mi.flip_vertically_on_load(true)
	local data, width, height
   for i=1,6 do
   	local filename = basename .."_"..SUFFIX[i]..".hdr"
		data, width, height = mi.load(filename, 'rgb', 'f')
		if i == 1 then -- Allocate immutable storage for the whole cube map texture
			gl.texture_storage('cube map', 1, 'rgb32f', width, height)
		end
		gl.texture_sub_image(TARGET[i], 0, 'rgb', 'float', data, 0, 0, width, height)
	end
	gl.texture_parameter('cube map', 'mag filter', 'linear')
	gl.texture_parameter('cube map', 'min filter', 'nearest')
	gl.texture_parameter('cube map', 'wrap s', 'clamp to edge')
	gl.texture_parameter('cube map', 'wrap t', 'clamp to edge')
	gl.texture_parameter('cube map', 'wrap r', 'clamp to edge')
	gl.unbind_texture('cube map')
	return texid, width, height
end

return {
	load_texture = load_texture,
	load_hdr_cube_map = load_hdr_cube_map,
	load_cube_map = load_cube_map,
}

