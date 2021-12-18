#!/usr/bin/env lua
-- MoonGL example: hello-glut.lua

gl= require("moongl")
glut = require("moonglut") -- Note that MoonGLUT is now discontinued (use MoonGLFW instead).

function Reshape(_, w, h) 
   print("window reshaped to "..w.."x"..h)
   gl.viewport(0, 0, w, h)
end

function Render()
   -- ... rendering code goes here ...
   gl.clear_color(1.0, 1.0, 1.0, 1.0)
   gl.clear( "color", "depth" )
   glut.swap_buffers()
end

glut.init(arg)
glut.init_context(3, 3, "core")
glut.action_on_window_close("return")
glut.init_window_position(0, 0)
glut.init_window_size(600, 400)
glut.init_display_mode("rgba", "double", "depth")
glut.create_window("Hello, World!")

glut.reshape_func(Reshape)
glut.display_func(Render)

gl.clear_color(0, 0, 0, 1)
glut.main_loop()

