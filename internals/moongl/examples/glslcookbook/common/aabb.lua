-- Axis-Aligned Bounding Box (AABB)

local glmath = require("moonglmath")
local inf = math.huge
local vec3 = glmath.vec3

local Aabb = {}
local Mt = {
   __index = Aabb,
   __tostring = function(aabb) return "AABB "..aabb.min.." "..aabb.max end,
   __concat = function(a, b) return a.type=="aabb" and tostring(a)..b or a..tostring(b) end,
}

local function new(min, max)
-- min, max = optional points (vec3) for delimiting the box
   local aabb = setmetatable({}, Mt)
   aabb.min = min and vec3(min) or vec3(inf, inf, inf)
   aabb.max = max and vec3(max) or -vec3(inf, inf, inf)
   aabb.type = "aabb"
   return aabb
end

function Aabb.diagonal(aabb)
   return aabb.max-aabb.min
end

function Aabb.reset(aabb)
-- Resets aabb to a box with no volume
   aabb.min = vec3(inf, inf, inf)
   aabb.max = -vec3(inf, inf, inf)
end

function Aabb.add(aabb, other)
-- Expands aabb so that it contains 'other', which may
-- be either a point (vec3) or another box (aabb).
   local othermin, othermax
   if other.type == "aabb" then
      othermin, othermax = other.min, other.max
   else
      othermin, othermax = other, other 
   end
   local min, max = aabb.min, aabb.max
   min.x = math.min(min.x, othermin.x)
   min.y = math.min(min.y, othermin.y)
   min.z = math.min(min.z, othermin.z)
   max.x = math.max(max.x, othermax.x)
   max.y = math.max(max.y, othermax.y)
   max.z = math.max(max.z, othermax.z)
end

return new

