-- An abstract camera object that processes input and calculates the corresponding Euler angles,
-- vectors and matrices for use in OpenGL
local glmath = require("moonglmath")

local vec3 = glmath.vec3
local clamp = glmath.clamp
local look_at = glmath.look_at
local sin, cos, rad = math.sin, math.cos, math.rad

-- Default camera values
local YAW = -90.0
local PITCH = 0.0
local SPEED = 2.5
local SENSITIVITY =  0.1
local ZOOM =  45.0
local POSITION = vec3(0.0, 0.0, 0.0)
local UP = vec3(0.0, 1.0, 0.0)
local FRONT = vec3(0.0, 0.0, -1.0)

local Camera = {}

local function update_vectors(camera)
-- Recomputes the camera vectors after an update of the Euler angles (yaw and pitch)
   -- Calculate the new Front vector
   local yaw, pitch = rad(camera.yaw), rad(camera.pitch)
   camera.front = vec3(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch)):normalize()
   -- Normalize the vectors, because their length gets closer to 0 the more you look
   -- up or down which results in slower movement.
   camera.right = (camera.front % camera.world_up):normalize()
   camera.up = (camera.right % camera.front):normalize()
end

function Camera.view(camera)
-- Returns the view matrix calculated using Euler Angles and the LookAt Matrix
   return look_at(camera.position, camera.position+camera.front, camera.up)
end

function Camera.process_keyboard(camera, dir, dt)
-- Processes input received from any keyboard-like input system.
-- dir = 'forward' | 'backward' | 'left' | 'right'
-- dt = delta time, in seconds
   local velocity = camera.speed * dt
   if dir=='forward' then camera.position = camera.position + camera.front*velocity
   elseif dir=='backward' then camera.position = camera.position - camera.front*velocity
   elseif dir=='left' then camera.position = camera.position - camera.right*velocity
   elseif dir=='right' then camera.position = camera.position + camera.right*velocity
   end
end

function Camera.process_mouse(camera, xoffset, yoffset, constrain_pitch)
-- Processes input received from a mouse input system.
-- Beware that constraint_pitch defaults to nil (=false) unlike in the original.
   camera.yaw = camera.yaw + xoffset*camera.sensitivity
   camera.pitch = camera.pitch + yoffset*camera.sensitivity
   if constraint_pitch then 
      -- make sure that when pitch is out of bounds, screen doesn't get flipped
      camera.pitch = clamp(camera.pitch, -89.0, 89.0)
   end
   -- Update Front, Right and Up Vectors using the updated Euler angles
   update_vectors(camera)
end

function Camera.process_scroll(camera, yoffset)
-- Processes input received from a mouse scroll-wheel event.
-- Only requires input on the vertical wheel-axis
   local zoom = camera.zoom
   if zoom >= 1.0 and zoom <= 45.0 then zoom = zoom - yoffset end
   camera.zoom = clamp(zoom, 1.0, 45.0)
end

return function(position, up, yaw, pitch) 
-- Camera constructor.
-- position and up are optional vec3
-- yaw and pitch are optional numbers (Euler angles, in degrees)
   local camera = setmetatable({}, {__index = Camera})
   camera.position = vec3(position or POSITION)
   camera.front = vec3(FRONT)
   camera.up = vec3(up or UP)
   camera.world_up = vec3(camera.up)
   camera.right = vec3()
   camera.yaw = yaw or YAW
   camera.pitch = pitch or PITCH
   camera.speed = SPEED
   camera.sensitivity = SENSITIVITY -- mouse sensitivity
   camera.zoom = ZOOM
   update_vectors(camera)
   return camera
end

