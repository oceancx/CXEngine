

local gl = require("moongl")
local glmath = require("moonglmath")

return function(vertices, indices, textures)
-- vertices = {vertex}, where vertex=14 floats (position, normal, texcoords, tangent, bitangent)
-- indices = {integer}
-- textures {texture}, where texture = { id=integer, name=string }
   local vertices = gl.pack('float', vertices)
   local indices = gl.pack('uint', indices)
   local count = #indices/gl.sizeof('uint') -- no. of vertices to draw
   local textures = textures
   -- now that we have all the required data, set the vertex buffers and its attribute pointers.
   -- create buffers/arrays
   local vao = gl.new_vertex_array()
   local vbo = gl.new_buffer('array')
   gl.buffer_data('array', vertices, 'static draw')
   local ebo = gl.new_buffer('element array')
   gl.buffer_data('element array', indices, 'static draw')
   -- set the vertex attribute pointers
   local float = gl.sizeof('float')
   gl.enable_vertex_attrib_array(0) -- position
   gl.vertex_attrib_pointer(0, 3, 'float', false, 14*float, 0)
   gl.enable_vertex_attrib_array(1) -- normal
   gl.vertex_attrib_pointer(1, 3, 'float', false, 14*float, 3*float)
   gl.enable_vertex_attrib_array(2) -- texcoords
   gl.vertex_attrib_pointer(2, 2, 'float', false, 14*float, 6*float)
   gl.enable_vertex_attrib_array(3) -- tangent
   gl.vertex_attrib_pointer(3, 3, 'float', false, 14*float, 8*float)
   gl.enable_vertex_attrib_array(4) -- bitangent
   gl.vertex_attrib_pointer(4, 3, 'float', false, 14*float, 11*float)
   gl.unbind_vertex_array()

   local diffuseNr, specularNr, normalNr, heightNr = 1, 1, 1, 1
   for i, texture in ipairs(textures) do
      texture.unit = i-1
      -- adjust the sampler's name by adding the trailing N (e.g. diffuse_textureN)
      local name = texture.name
      local n
      if name=="texture_diffuse"      then n=diffuseNr  diffuseNr=diffuseNr+1
      elseif name=="texture_specular" then n=specularNr specularNr=specularNr+1
      elseif name=="texture_normal"   then n=normalNr   normalNr=normalNr+1
      elseif name=="texture_height"   then n=heightNr   heightNr=heightNr+1
      else error("invalid Texture.name")
      end
      texture.name = name..tostring(n) -- name of the GLSL texture sampler 
   end

   vertices, indices = nil -- we don't need these any more

   return setmetatable({}, {
      __index = {

         draw = function(mesh, prog)
         -- render the mesh with the given shader program prog
            for _, texture in ipairs(textures) do
               local ok, loc = pcall(gl.get_uniform_location, prog, texture.name)
               if ok then -- the sampler is being used
                  -- print(loc, texture.unit, texture.id, texture.name, texture.path)
                  gl.uniformi(loc, texture.unit)
                  gl.active_texture(texture.unit)
                  gl.bind_texture('2d', texture.id)
               end
            end
            -- draw mesh
            gl.bind_vertex_array(vao)
            gl.draw_elements('triangles', count, 'uint', 0)
            gl.unbind_vertex_array()
            gl.active_texture(0)
         end,

         vao = function(mesh) return vao end,
         textures = function(mesh) return textures end,
         count = function(mesh) return count end,

         delete = function(mesh)
            if not vao then return end
            gl.delete_vertex_arrays(vao)
            gl.delete_buffers(vbo, ebo)
            textures, count, vba, vbo, ebo = nil
         end
         },

      __gc = function(mesh) mesh:delete() end,

      })
end

