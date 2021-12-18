#!/usr/bin/env lua
-- MoonGL example: hello-glfw.lua

gl = require("moongl")
glfw = require("moonglfw")

function reshape(_, w, h) 
   print("window reshaped to "..w.."x"..h)
   gl.viewport(0, 0, w, h)
end

-- glfwInit() is automatically called by MoonGLFW
glfw.window_hint('context version major', 3)
glfw.window_hint('context version minor', 3)
glfw.window_hint('opengl profile', 'core')

window = glfw.create_window(600, 400, "Hello, World!")
glfw.make_context_current(window)
gl.init() -- this is actually glewInit()

glfw.set_framebuffer_size_callback(window, reshape)

while not glfw.window_should_close(window) do
   glfw.poll_events()
   -- ... rendering code goes here ...
   gl.clear_color(1.0, 0.5, 0.2, 1.0) -- GLFW orange
   gl.clear("color", "depth")
   glfw.swap_buffers(window)
end
   
-- glfwTerminate() is automatically called by MoonGLFW at exit
