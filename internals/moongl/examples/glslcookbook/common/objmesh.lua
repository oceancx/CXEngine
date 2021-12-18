-- Drawable mesh, loaded from a Wavefront OBJ file.
-- Rfr: http://www.martinreddy.net/gfx/3d/OBJ.spec
local gl = require("moongl")
local glmath = require("moonglmath")
local new_aabb = require("common.aabb")
local cache = require("common.cache")

local huge = math.huge
local vec2, vec3, vec4 = glmath.vec2, glmath.vec3, glmath.vec4
local fmt = string.format
local function printf(...) io.write(string.format(...)) end

local function GenerateNormals(positions, faces)
   -- faces = {{p_index, tc_index, n_index}}, 1-based
   local normals = {}
   for i = 1, #positions do normals[i] = vec3() end
   for i = 1, #faces, 3 do
      local i1, i2, i3 = faces[i][1], faces[i+1][1], faces[i+2][1] -- p_index
      local p1, p2, p3 = positions[i1], positions[i2], positions[i3]
      local n = (p2-p1)%(p3-p1):normalize()
      normals[i1], normals[i2], normals[i3] = normals[i1]+n, normals[i2]+n, normals[i3]+n
      -- Set n_index to be the same as p_index
      faces[i][3], faces[i+1][3], faces[i+2][3] = i1, i2, i3
   end
   for i, n in ipairs(normals) do normals[i] = n:normalize() end
   return normals
end

local function GenerateTangents(positions, normals, faces, texcoords)
   -- faces = {{p_index, tc_index, n_index}}, 1-based
   local accum1, accum2 = {}, {}
   for i = 1, #positions do
      accum1[i], accum2[i] = vec3(), vec3()
   end
   -- Compute the tangent vector
   for i = 1, #faces, 3 do
      local i1, i2, i3 = faces[i][1], faces[i+1][1], faces[i+2][1] -- p_index
      local j1, j2, j3 = faces[i][2], faces[i+1][2], faces[i+2][2] -- tc_index
      local p1, p2, p3 = positions[i1], positions[i2], positions[i3]
      local tc1, tc2, tc3 = texcoords[j1], texcoords[j2], texcoords[j3]
      local q1, q2 = p2 - p1, p3 - p1
      local s1, s2 = tc2.x - tc1.x, tc3.x - tc1.x
      local t1, t2 = tc2.y - tc1.y, tc3.y - tc1.y
      local r = 1/(s1*t2 - s2*t1)
      local tan1 = vec3(t2*q1.x - t1*q2.x, t2*q1.y - t1*q2.y, t2*q1.z - t1*q2.z)*r
      local tan2 = vec3(s1*q2.x - s2*q1.x, s1*q2.y - s2*q1.y, s1*q2.z - s2*q1.z)*r
      accum1[i1] = accum1[i1] + tan1
      accum1[i2] = accum1[i2] + tan1
      accum1[i3] = accum1[i3] + tan1
      accum2[i1] = accum2[i1] + tan2
      accum2[i2] = accum2[i2] + tan2
      accum2[i3] = accum2[i3] + tan2
   end
   local tangents = {}
   for i = 1, #positions do 
      local n, t1, t2 = normals[i], accum1[i], accum2[i]
      -- Gram-Schmidt orthogonalize
      tangents[i] = vec4((t1 - (n*t1)*n):normalize())
      -- Store handedness in w
      tangents[i].w = (n % t1)*t2 < 0 and -1 or 1
   end
   return tangents
end

local function Center(positions, bbox)
   if #positions == 0 then return positions end
   local center = 0.5*(bbox.max + bbox.min) -- center of the AABB
   -- Translate center of the AABB to the origin
   local new_positions = {}
   for i, pos in ipairs(positions) do
      new_positions[i] = pos - center
   end
   -- Update bbox
   bbox.max = bbox.max - center
   bbox.min = bbox.min - center
   return new_positions
end

local function ToGLMesh(objpositions, objnormals, objfaces, objtexcoords, objtangents)
-- Converts the data loaded from the OBJ file to a format suitable for OpenGL.
-- Removes duplicated vertex data, and stores 0-based positions indices in the 'faces'
-- table, as expected by OpenGL.
   local positions, normals, faces, texcoords, tangents = {}, {}, {}, {}, {}
   local has_texcoords, has_tangents = #objtexcoords>0, #objtangents>0
   local map = {} -- map["p_index/tc_index/n_index"] = 0-based p_index
   for _, v in ipairs(objfaces) do
      local key = fmt("%d/%d/%d", v[1], v[2], v[3]) -- p_index/tc_index/n_index (1-based)
      local p_index = map[key]
      if not p_index then -- first time we encounter this vertex
         p_index = #positions -- 0-based position index
         table.insert(positions, objpositions[v[1]])
         table.insert(normals, objnormals[v[3]])
         if has_texcoords then table.insert(texcoords, objtexcoords[v[2]]) end
         -- We use the position index for tangents
         if has_tangents then table.insert(tangents, objtangents[v[1]]) end
         map[key] = p_index
      end
      table.insert(faces, p_index)
   end
   return positions, normals, faces, texcoords, tangents
end

local function ConvertToAdjancencyFormatOLD(faces)
-- This implementation is too slow, especially with Lua 
   local f = {}
   -- Copy and make room for adjacency info
   for i = 0, #faces-1, 3 do
      f[i*2+1] = faces[i+1]
      f[i*2+2] = huge
      f[i*2+3] = faces[i+2]
      f[i*2+4] = huge
      f[i*2+5] = faces[i+3]
      f[i*2+6] = huge
   end
   -- Find matching edges
   for i = 1, #f, 6 do
      local a1, b1, c1 = f[i], f[i+2], f[i+4] -- a triangle
         print(string.format("%d %%", math.floor(i/#f*100)))
      -- Scan subsequent triangles
      for j = i+6, #f, 6 do
         local a2, b2, c2 = f[j], f[j+2], f[j+4]
         if (a1==a2 and b1==b2) or (a1==b2 and b1==a2) then f[i+1], f[j+1] = c2, c1 end -- Edge1==Edge1
         if (a1==b2 and b1==c2) or (a1==c2 and b1==b2) then f[i+1], f[j+3] = a2, c1 end -- Edge1==Edge2
         if (a1==c2 and b1==a2) or (a1==a2 and b1==c2) then f[i+1], f[j+5] = b2, c1 end -- Edge1==Edge3
         if (b1==a2 and c1==b2) or (b1==b2 and c1==a2) then f[i+3], f[j+1] = c2, a1 end -- Edge2==Edge1
         if (b1==b2 and c1==c2) or (b1==c2 and c1==b2) then f[i+3], f[j+3] = a2, a1 end -- Edge2==Edge2
         if (b1==c2 and c1==a2) or (b1==a2 and c1==c2) then f[i+3], f[j+5] = b2, a1 end -- Edge2==Edge3
         if (c1==a2 and a1==b2) or (c1==b2 and a1==a2) then f[i+5], f[j+1] = c2, b1 end -- Edge3==Edge1
         if (c1==b2 and a1==c2) or (c1==c2 and a1==b2) then f[i+5], f[j+3] = a2, b1 end -- Edge3==Edge2
         if (c1==c2 and a1==a2) or (c1==a2 and a1==c2) then f[i+5], f[j+5] = b2, b1 end -- Edge3==Edge3
      end
   end
    -- Look for any outside edges
   for i = 1, #f, 6 do
      if f[i+1] == huge then f[i+1] = f[i+4] end
      if f[i+3] == huge then f[i+3] = f[i] end
      if f[i+5] == huge then f[i+5] = f[i+2] end
   end
   return f
end

local function ConvertToAdjancencyFormat(faces)
   local adj = {}
   for i = 1, #faces, 3 do
      local a, b, c = faces[i], faces[i+1], faces[i+2] -- a triangle
      adj[fmt("%d.%d", a, b)] = c
      adj[fmt("%d.%d", b, c)] = a
      adj[fmt("%d.%d", c, a)] = b
   end
   local f = {}
   for i = 1, #faces, 3 do
      local a, b, c = faces[i], faces[i+1], faces[i+2] -- a triangle
      local k = #f
      f[#f+1] = a
      f[#f+1] = adj[fmt("%d.%d", b, a)] or c
      f[#f+1] = b
      f[#f+1] = adj[fmt("%d.%d", c, b)] or a
      f[#f+1] = c
      f[#f+1] = adj[fmt("%d.%d", a, c)] or b
   end
   return f
end
      
-- matching patterns
local FLOAT="%-?%d*%.?%d*[Ee]?%-?%d*"
local V_PATTERN = "v%s+("..FLOAT..")%s+("..FLOAT..")%s+("..FLOAT..")"
local VN_PATTERN = "vn%s+("..FLOAT..")%s+("..FLOAT..")%s+("..FLOAT..")"
local VT_PATTERN = "vt%s+("..FLOAT..")%s+("..FLOAT..")"

local function Load(filename, center_mesh, add_tangents, adjacency)
   local f = assert(io.open(filename, 'r'))
   local positions, normals, faces, texcoords, tangents = {}, {}, {}, {}, {}
   local bbox = new_aabb()

   printf("Loading mesh from: '%s' ...", filename)
   io.flush()

   -- Load the vertex data from the OBJ file:
   while true do
      local line = f:read("l")
      if not line then break end -- eof
      line = line:gsub("^%s*(.-)%s*$", "%1") -- trim leading and trailing whitespace
      if #line == 0 or line:sub(1,1) == '#' then -- empty line or comment
         goto continue
      end
      local token = string.match(line, "%w+")
      if token == 'v' then
         local x, y, z = string.match(line, V_PATTERN)
         local p =  vec3(tonumber(x), tonumber(y), tonumber(z))
         table.insert(positions, p)
         bbox:add(p)
      elseif token == 'vn' then
         local x, y, z = string.match(line, VN_PATTERN)
         table.insert(normals, vec3(tonumber(x), tonumber(y), tonumber(z)))
      elseif token == 'vt' then
         local s, t = string.match(line, VT_PATTERN)
         table.insert(texcoords, vec2(tonumber(s), tonumber(t)))
      elseif token == 'f' then
         local verts = {} -- vertices defining the face
         if string.match(line, "%d+/%d+/%d+") then
            local iter = string.gmatch(line, "%d+/%d+/%d+")
            while true do
               local xxx = iter()
               if not xxx then break end
               local p_index, tc_index, n_index = string.match(xxx, "(%d+)/(%d+)/(%d+)")
               verts[#verts+1] = { tonumber(p_index), tonumber(tc_index), tonumber(n_index) }
            end
         elseif string.match(line, "%d+/%d+") then
            local iter = string.gmatch(line, "%d+/%d+")
            while true do
               local xxx = iter()
               if not xxx then break end
               local p_index, tc_index = string.match(xxx, "(%d+)/(%d+)")
               verts[#verts+1] = { tonumber(p_index), tonumber(tc_index), #normals }
            end
         else
            local iter = string.gmatch(line, "%d+")
            while true do
               local p_index = iter()
               if not p_index then break end
               verts[#verts+1] = { tonumber(p_index), #texcoords, #normals }
            end
         end
         -- Triangulate as a triangle fan
         if #verts > 2 then
            for i = 3, #verts do
               table.insert(faces, verts[1])
               table.insert(faces, verts[i-1])
               table.insert(faces, verts[i])
            end
         else
            -- malformed file: face with less than 2 vertices
         end
      else
         -- print(line, " unhandled OBJ token="..token)
      end
      ::continue::
   end
   f:close()

   if #normals == 0 then normals = GenerateNormals(positions, faces) end

   if add_tangents and #texcoords > 0 then
      tangents = GenerateTangents(positions, normals, faces, texcoords)
   end

   -- Convert to GL format:
   positions, normals, faces, texcoords, tangents = 
      ToGLMesh(positions, normals, faces, texcoords, tangents)

   if center_mesh then positions = Center(positions, bbox) end

   if adjacency then faces = ConvertToAdjancencyFormat(faces) end

   print("done")
   printf(" %d points\n", #positions)
   printf(" %d triangles\n", #faces)
   printf(" %d normals\n", #normals)
   printf(" %d tangents\n", tangents and #tangents or 0)
   printf(" %d texture coordinates\n", texcoords and #texcoords or 0)
   printf(" aabb %s %s\n", bbox.min, bbox.max)

   -- serialize data and return:
   positions = gl.pack('float', positions)
   normals = #normals > 0 and gl.pack('float', normals) or ""
   local indices = #faces > 0 and gl.pack('uint', faces) or ""
   texcoords = #texcoords > 0 and gl.pack('float', texcoords) or ""
   tangents = #tangents > 0 and gl.pack('float', tangents) or ""
   local bboxmin = gl.pack('float', bbox.min)
   local bboxmax = gl.pack('float', bbox.max)

   return positions, normals, indices, texcoords, tangents, bboxmin, bboxmax
end


local function new(filename, options)
   local options = options or {}
   local center_mesh = options.center or false
   local add_tangents = options.tangents or false
   local adjacency = options.adjacency or false

   local cachename = "../cache/"..filename:match("^.+/(.+)$").."-"..
            (center_mesh and 1 or 0)..(add_tangents and 1 or 0)..(adjacency and 1 or 0)
   local positions, normals, indices, texcoords, tangents, bboxmin, bboxmax = cache.read(cachename)
   if not positions then -- generate patches and cache data for next time
      positions, normals, indices, texcoords, tangents, bboxmin, bboxmax =
                  Load(filename, center_mesh, add_tangents, adjacency)
      cache.write(cachename, positions, normals, indices, texcoords, tangents, bboxmin, bboxmax)
   end

   -- Create and populate the buffer objects
   local vao = gl.new_vertex_array()
   local p_vbo, n_vbo, tc_vbo, tg_vbo, ebo

   p_vbo = gl.new_buffer('array')
   gl.buffer_data('array', positions, 'static draw')
   gl.enable_vertex_attrib_array(0)  -- positions
   gl.vertex_attrib_pointer(0, 3, 'float', false, 0, 0)

   n_vbo = gl.new_buffer('array')
   gl.buffer_data('array', normals, 'static draw')
   gl.enable_vertex_attrib_array(1)  -- normals
   gl.vertex_attrib_pointer(1, 3, 'float', false, 0, 0)

   if #texcoords > 0 then
      tc_vbo = gl.new_buffer('array')
      gl.buffer_data('array', texcoords, 'static draw')
      gl.enable_vertex_attrib_array(2)  -- texcoords
      gl.vertex_attrib_pointer(2, 2, 'float', false, 0, 0)
   end

   if add_tangents and #tangents > 0 then
      tg_vbo = gl.new_buffer('array')
      gl.buffer_data('array', tangents, 'static draw')
      gl.vertex_attrib_pointer(3, 4, 'float', false, 0, 0)
      gl.enable_vertex_attrib_array(3)  -- tangents
   end

   ebo = gl.new_buffer('element array')
   gl.buffer_data('element array', indices, 'static draw')

   gl.unbind_vertex_array()

   local count = #indices/gl.sizeof('uint')
   local mode = adjacency and 'triangles adjacency' or 'triangles'

   bboxmin = vec3(table.unpack(gl.unpack('float', bboxmin)))
   bboxmax = vec3(table.unpack(gl.unpack('float', bboxmax)))
   local bbox = new_aabb(bboxmin, bboxmax)

   positions, tangents, normals, texcoords, indices = nil

   return setmetatable({}, {
      __index = {
         ----------------------------------------------------------------------
         vao = function(mesh) return vao end,
         bbox = function(mesh) return bbox end,
         ----------------------------------------------------------------------
         render = function(mesh)
            if vao then
               gl.bind_vertex_array(vao)
               gl.draw_elements(mode, count, 'uint', 0)
               gl.unbind_vertex_array()
            end
         end,
         ----------------------------------------------------------------------
         delete = function(mesh)
            if not vao then return end
            gl.delete_vertex_arrays(vao)
            gl.delete_buffers(p_vbo, n_vbo, ebo)
            if tc_vbo then gl.delete_buffers(tc_vbo) end
            if tg_vbo then gl.delete_buffers(tg_vbo) end
            vao, p_vbo, n_vbo, tc_vbo, tg_vbo, ebo = nil
         end,
         ----------------------------------------------------------------------
      },
   })
end

return new

