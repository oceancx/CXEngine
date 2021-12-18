-- Drawable teapot
local gl = require("moongl")
local glmath = require("moonglmath")
local cache = require("common.cache")
local teapotdata = require("common.teapotdata")

local gl = require("moongl")
local glmath = require("moonglmath")

local vec2, vec3, vec4 = glmath.vec2, glmath.vec3, glmath.vec4
local mat3 = glmath.mat3
local sin, cos = math.sin, math.cos

local function get_patch(patch_num)
   local patchdata = teapotdata.patchdata[patch_num]
   local positions = teapotdata.positions
   local patch1 = {{}, {}, {}, {}}
   local patch2 = {{}, {}, {}, {}} -- reverse v
   for u = 1, 4 do
      for v = 1, 4 do
         patch1[u][v] = positions[patchdata[4*u+v-4]+1]
         patch2[u][v] = positions[patchdata[4*u-v+1]+1]
      end
   end
   return patch1, patch2
end

local function compute_basis_functions(grid)
   local B, dB = {}, {}
   local inc = 1/grid
   for i = 0, grid do
      local t = i*inc
      B[i*4 + 1] = (1-t)^3
      B[i*4 + 2] = 3*(1-t)^2 *t
      B[i*4 + 3] = 3*(1-t)*t^2
      B[i*4 + 4] = t^3
      dB[i*4 + 1] = -3*(1-t)^2
      dB[i*4 + 2] = -6*t*(1-t) + 3*(1-t)^2
      dB[i*4 + 3] = -3*t^2 + 6*t*(1-t)
      dB[i*4 + 4] = 3*t^2
   end
   return B, dB
end


local EPSILON = 1e-8
local function evaluate(u, v, B, dB, patch)
   local p, du, dv = vec3(), vec3(), vec3()
   for i = 1, 4 do
      for j = 1, 4 do
         p = p + patch[i][j]*B[4*u+i]*B[4*v+j]
         du = du + patch[i][j]*dB[4*u+i]*B[4*v+j]
         dv = dv + patch[i][j]*B[4*u+i]*dB[4*v+j]
      end
   end
   local n = du % dv
   local norm_n = n:norm()
   if norm_n > EPSILON then n = n/norm_n else n = vec3() end
   return p, n
end

local function build_patch(patch, B, dB, positions, normals, texcoords, indices, grid, reflect, invert_normal)
   local start_index = #positions -- 0-based
   local norm_reflect = invert_normal and -reflect or reflect
   local p, n, u, v

   for i = 0, grid do
      for j = 0, grid do
         p, n = evaluate(i, j, B, dB, patch)
         table.insert(positions, reflect*p)
         table.insert(normals, norm_reflect*n)
         table.insert(texcoords, vec2(i, j)/grid)
      end
   end

   for i = 0, grid-1 do
      local start = i*(grid+1) + start_index
      local next_start = (i+1)*(grid+1) + start_index
      for j = 0, grid-1 do
         table.insert(indices, 
            {start+j, next_start+j+1, next_start+j, start+j, start+j+1, next_start+j+1})
      end
   end
end

local I = mat3()
local Rx = mat3(-1, 0, 0, 0, 1, 0, 0, 0, 1)
local Ry = mat3(1, 0, 0, 0, -1, 0, 0, 0, 1)
local Rxy = mat3(-1, 0, 0, 0, -1, 0, 0, 0, 1)

local function build_patch_reflect(patch_num, B, dB, positions, normals, texcoords, indices, grid, reflect_x, reflect_y)
   local patch, patch_rev = get_patch(patch_num)
   -- Patch without modification
   build_patch(patch, B, dB, positions, normals, texcoords, indices, grid, I, true)
   -- Patch reflected in x
   if reflect_x then 
      build_patch(patch_rev, B, dB, positions, normals, texcoords, indices, grid, Rx, false)
   end
   -- Patch reflected in y
   if reflect_y then 
      build_patch(patch_rev, B, dB, positions, normals, texcoords, indices, grid, Ry, false)
   end
   -- Patch reflected in x and y
   if reflect_x and reflect_y then 
      build_patch(patch, B, dB, positions, normals, texcoords, indices, grid, Rxy, true)
   end
end

local function generate_patches(grid, lid_transform)
   local positions, normals, texcoords, indices = {}, {}, {}, {}

   -- Pre-compute the basis functions  (Bernstein polynomials) and their derivatives
   local B, dB = compute_basis_functions(grid)

   -- Build each patch
   -- The rim
   build_patch_reflect(1, B, dB, positions, normals, texcoords, indices, grid, true, true)
   -- The body
   build_patch_reflect(2, B, dB, positions, normals, texcoords, indices, grid, true, true)
   build_patch_reflect(3, B, dB, positions, normals, texcoords, indices, grid, true, true)
   -- The lid
   local lid_first = #positions + 1
   build_patch_reflect(4, B, dB, positions, normals, texcoords, indices, grid, true, true)
   build_patch_reflect(5, B, dB, positions, normals, texcoords, indices, grid, true, true)
   local lid_last = #positions
   -- The bottom
   build_patch_reflect(6, B, dB, positions, normals, texcoords, indices, grid, true, true)
   -- The handle
   build_patch_reflect(7, B, dB, positions, normals, texcoords, indices, grid, false, true)
   build_patch_reflect(8, B, dB, positions, normals, texcoords, indices, grid, false, true)
   -- The spout
   build_patch_reflect(9, B, dB, positions, normals, texcoords, indices, grid, false, true)
   build_patch_reflect(10, B, dB, positions, normals, texcoords, indices, grid, false, true)

   if lid_transform then -- apply transform to lid vertices
      for i = lid_first, lid_last do
         local p = vec4(positions[i])
         p.w = 1
         positions[i] = vec3(lid_transform*p)
      end
   end

   -- Convert data to binary strings:
   return gl.pack('float', positions), gl.pack('float', normals),
          gl.pack('float', texcoords), gl.pack('uint', indices)
end

-------------------------------------------------------------------------------
local function new(grid, lid_transform)
   local positions, normals, texcoords, indices
   
   local filename
   if lid_transform then 
      filename = string.format("../cache/teapot-%d-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f-%f",
         grid, gl.flatten(lid_transform))
   else
      filename = string.format("../cache/teapot-%d", grid)
   end

   local positions, normals, texcoords, indices = cache.read(filename)
   if not positions then -- generate patches and cache data for next time
      print("Generating vertex data...")
      positions, normals, texcoords, indices = generate_patches(grid, lid_transform)
      cache.write(filename, positions, normals, texcoords, indices)
   end

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
