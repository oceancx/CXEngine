#!/usr/bin/env lua
-- MoonGL example: versions.lua

local gl= require("moongl")
local glfw = require("moonglfw")

print(gl._VERSION)
print(gl._GLEW_VERSION)
print(glfw._VERSION)
print(glfw._GLFW_VERSION)

-- We don't really need a window for this example, but we do need a GL context
-- to initialize OpenGL, so we create one with MoonGLFW:
glfw.window_hint('visible', false)
local window = glfw.create_window(100, 100)
glfw.make_context_current(window)
gl.init()

-- The following calls work only after gl.init() (i.e. glewInit()):
print(gl.version()) -- OpenGL version supported by MoonGL+GLEW+driver
print("Versions supported by the OpenGL implementation:")
print("version:  "..gl.get_string("version"))
print("renderer: "..gl.get_string("renderer"))
print("vendor:   "..gl.get_string("vendor"))
print("shading language version: "..gl.get_string("shading language version"))

--[[
print("Available GL versions:")
print(table.concat({gl.versions()}, '\n'))
-- Note that the available GL versions depends on both GLEW and the driver
-- (e.g., if the driver supports 4.6 but GLEW does not, then 4.6 is not available)
--]]

