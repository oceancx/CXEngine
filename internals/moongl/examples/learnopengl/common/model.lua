-- ASSIMP-based model loader
package.path = package.path..";../?.lua"
local gl = require("moongl")
local ai = require("moonassimp")
local new_mesh = require("common.mesh")
local new_texture = require("common.texture")

local directory = "???"
local textures_loaded = {}
local meshes = {}

local function load_material_textures(textures, material, texturetype, name)
-- Checks all material textures of a given type and loads the textures if they're not loaded yet.
-- Note: We adopt the following convention for sampler names in the shaders:
-- each diffuse texture should be named as 'texture_diffuseN' where N is a sequential number
-- ranging from 1 to MAX_SAMPLER_NUMBER. The same applies to the other texture types:
-- diffuse  -> texture_diffuseN
-- specular -> texture_specularN
-- normal   -> texture_normalN
-- height   -> texture_heightN
   for i = 1, material:texture_count(texturetype) do
      local path = material:texture_path(texturetype, i)
      local tex = textures_loaded[path]   
      if not tex then -- not already loaded
         tex = {}
         tex.name = name
         tex.id = new_texture(directory..path)
         textures_loaded[path] = tex
      end
      table.insert(textures, {name=tex.name, id=tex.id, path=directory..path})
      -- print("material texture", tex.id, tex.name, tex.path)
   end
end

local function process_mesh(aimesh)
   local vertices, indices, textures = {}, {}, {} -- data to fill

   -- We assume that the models we use have only one set of texture coordinates:
   local has_texcoords = aimesh:has_texture_coords(1)
   
   for i=1, aimesh:num_vertices() do
      local position = {aimesh:position(i)}
      local normal = {aimesh:normal(i)}
      local tangent = {aimesh:tangent(i)}
      local bitangent = {aimesh:bitangent(i)}
      local u, v
      if has_texcoords then u, v = aimesh:texture_coords(1, i) else u, v = 0.0, 0.0 end
      local texcoords = {u, v}
      table.insert(vertices, {position, normal, texcoords, tangent, bitangent})
   end

   indices = aimesh:all_indices(true) -- zero-based indices

   local material = aimesh:material()
   load_material_textures(textures, material, 'diffuse', 'texture_diffuse')
   load_material_textures(textures, material, 'specular', 'texture_specular')
   load_material_textures(textures, material, 'height', 'texture_normal')
   load_material_textures(textures, material, 'ambient', 'texture_height')
   -- print("vertices:" .. aimesh:num_vertices() .. " faces:".. #indices .. " textures:" ..#textures) 
   return new_mesh(vertices, indices, textures)
end

local function process_node(node)
-- Processes each individual mesh located at the node and repeats rcursively
-- on its children nodes (if any).
   for _, mesh in ipairs(node:meshes()) do 
      table.insert(meshes, process_mesh(mesh))
   end
   for _, child in ipairs(node:children()) do
      process_node(child)
   end
end


return function(path)
-- Constructor, expects a filepath to a 3D model.
   local scene = assert(ai.import_file(path, 'triangulate', 'flip uvs', 'calc tangent space'))
   assert((scene:flags()&ai.SCENE_FLAGS_INCOMPLETE)==0, "scene is incomplete")
   local root = assert(scene:root_node(), "missing root node")
   directory = string.match(path, '(.*/)')
   textures_loaded = {} -- contains already loaded textures (to avoid loading them twice or more)
   meshes = {} -- the meshes that compose the model
   process_node(root)
   local meshes = meshes
   return setmetatable({}, {
      __index = {
         draw = function(model, prog)
            for _, mesh in ipairs(meshes) do mesh:draw(prog) end
         end,
         meshes = function(model) return meshes end,
         delete = function(model)   
            if not meshes then return end
            for _, mesh in ipairs(meshes) do mesh:delete(prog) end
            meshes = nil
         end,
         },
      __gc = function(model) model:delete() end
   })
end

