-- Random utilities
local glmath = require("moonglmath")

local random = math.random
local randomseed = math.randomseed
local pi = math.pi
local sqrt = math.sqrt
local sin, cos = math.sin, math.cos
local vec3 = glmath.vec3

local function uniform_hemisphere()
   local x1, x2 = random(), random()
   local s = sqrt(1.0- x1*x1)
   local pi2x2 = 2*pi*x2
   return vec3(cos(pi2x2)*s, sin(pi2x2)*s, x1)
end

local function uniform_circle()
   local x = random()
   local pi2x = 2*pi*x
   return vec3(cos(pi2x), sin(pi2x), 0)
end

local function shuffle(t) 
   --@@ shuffle the elements of table t so that each ordering has the same probability
   -- std::shuffle()
   error("TODO")
end

return {
   uniform_hemisphere = uniform_hemisphere,
   uniform_circle = uniform_circle,
   shuffle = shuffle,
}

