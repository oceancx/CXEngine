#!/usr/bin/env lua
local gl = require("moongl")
local glfw = require("moonglfw")

-- settings
local SCR_WIDTH, SCR_HEIGHT = 800, 600

-- process all input: query GLFW whether relevant keys are pressed/released
-- this frame and react accordingly
local function process_input(window)
   if glfw.get_key(window, 'escape') == 'press' then
       glfw.set_window_should_close(window, true)
   end
end

-- glfw: whenever the window size changed (by OS or user resize) this callback function executes
local function framebuffer_size_callback(window, width, height)
    -- make sure the viewport matches the new window dimensions; note that width and 
    -- height will be significantly larger than specified on retina displays.
    gl.viewport(0, 0, width, height)
end

-- glfw: initialize and configure
glfw.window_hint('context version major', 3)
glfw.window_hint('context version minor', 3)
glfw.window_hint('opengl profile', 'core')

-- glfw window creation
local window = glfw.create_window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL")
glfw.make_context_current(window)
gl.init() -- this loads all OpenGL function pointers
glfw.set_framebuffer_size_callback(window, framebuffer_size_callback)

-- render loop
while not glfw.window_should_close(window) do
   -- input
   process_input(window)
   -- render
   gl.clear_color(0.2, 0.3, 0.3, 1.0)
   gl.clear('color')
   -- glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
   glfw.swap_buffers(window)
   glfw.poll_events()
end

