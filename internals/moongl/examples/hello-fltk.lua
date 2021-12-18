#!/usr/bin/env lua
-- MoonGL example: hello-fltk.lua

fl = require("moonfltk")
gl = require("moongl")

function init(win)
	-- Called only the first time the window is drawn.
	-- Initializes the GL context and sets the draw() callback to the
	-- actual rendering function, which will be called the next times
	-- the window will be drawn.
	assert(win == window)	
	gl.init() -- glewInit()
	print("OpenGL " .. gl.get("major version") .. "." .. gl.get("minor version"))
	print("mode", win:mode())
	print("can_do_overlay", win:can_do_overlay())
	win:override_draw(render)
	win:flush()
end

function render(win) -- Rendering function
	-- print("draw()", win) 
   -- ... rendering code goes here ...
   --gl.clear_color(0.85, 0.85, 1.0, 1.0)
   gl.clear_color(0.67, 0.67, 0.87, 1.0)
   gl.clear("color", "depth")
	win:redraw()
end

window = fl.gl_window_sub(0, 0, 600, 400, "Hello, World!")
window:override_draw(init)

-- Optional overloading of other fl.gl_window_sub methods:

window:override_handle(function (win, ev) -- Handles an event
	assert(win == window)	
	-- print("handle()", win, ev) 
	return win:super_handle(ev) 
end)

window:override_hide(function(win) -- Called when the wintow is hidden.
	assert(win == window)	
	-- print("hide()", win) 
	win:super_hide()
end)

window:override_resize(function (win, x, y, w, h) -- Called when the window is resized.
	assert(win == window)	
	-- print("resize()", win, x, y, w, h) 
   print("window reshaped to "..w.."x"..h)
	win:super_resize(x, y, w, h) 
end)

-- main FLTK loop
window:resizable(window)
window:show()
return fl.run()

