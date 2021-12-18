local gl = require("moongl")
local mi = require("moonimage")

local face_target = {
   'cube map positive x', -- right face
   'cube map negative x', -- left face
   'cube map positive y', -- top face
   'cube map negative y', -- bottom face
   'cube map positive z', -- front face
   'cube map negative z', -- back face
}

return function(faces) 
-- Utility function for loading a cubemap texture from 6 individual files containing.
-- The faces table is assumed to contain 6 file paths for the cubemap faces, in the
-- same order as the above face_target table.
   local texid = gl.new_texture('cube map')
   for i=1, 6 do
      local data, w, h, channels = mi.load(faces[i])
      assert(channels=='rgb')
      gl.texture_image(face_target[i], 0, 'rgb', 'rgb', 'ubyte', data, w, h)
   end
   gl.texture_parameter('cube map', 'min filter', 'linear')
   gl.texture_parameter('cube map', 'mag filter', 'linear')
   gl.texture_parameter('cube map', 'wrap s', 'clamp to edge')
   gl.texture_parameter('cube map', 'wrap t', 'clamp to edge')
   gl.texture_parameter('cube map', 'wrap r', 'clamp to edge')
   return texid
end

