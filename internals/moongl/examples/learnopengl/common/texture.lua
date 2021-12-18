-- Utility function for loading a 2D texture from file
local gl = require("moongl")
local mi = require("moonimage")

return function(path) 
   local data, w, h, channels = mi.load(path)
   -- print(path, w, h, channels, #data, w*h*#channels)
   local intformat, format
   if channels=='rgba' then intformat, format = 'rgba', 'rgba'
   elseif channels=='rgb' then intformat, format = 'rgb', 'rgb'
   elseif channels=='y' then intformat, format = 'red', 'red'
   elseif channels=='ya' then intformat, format = 'rg', 'rg' -- is this correct?
   end
   local texid = gl.new_texture('2d')
   gl.texture_image('2d', 0, intformat, format, 'ubyte', data, w, h)
   gl.generate_mipmap('2d')
   gl.texture_parameter('2d', 'wrap s', 'repeat')
   gl.texture_parameter('2d', 'wrap t', 'repeat')
   gl.texture_parameter('2d', 'min filter', 'linear mipmap linear')
   gl.texture_parameter('2d', 'mag filter', 'linear')
   return texid
end

