## MoonGL: Lua bindings for OpenGL

MoonGL is a Lua binding library for [OpenGL](https://www.opengl.org/).

It runs on GNU/Linux, MacOS, and on Windows (MSYS2/MinGW) and requires
[Lua](http://www.lua.org/) (>=5.3), 
[OpenGL](https://www.opengl.org/) (>=3.3), and
[GLEW](http://glew.sourceforge.net/).


_Authored by:_ _[Stefano Trettel](https://www.linkedin.com/in/stetre)_

[![Lua logo](./doc/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT/X11 license (same as Lua). See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://stetre.github.io/moongl/doc/index.html).

#### Getting and installing

Setup the build environment as described [here](https://github.com/stetre/moonlibs), then:

```sh
$ git clone https://github.com/stetre/moongl
$ cd moongl
moongl$ make
moongl$ make install # or 'sudo make install' (Ubuntu and MacOS)
```

#### Example

Below is an "Hello, World!" example, using [MoonGLFW](https://github.com/stetre/moonglfw) 
as [windowing library](#see-also).

Other examples can be found in the **examples/** directory contained in the release package.

```lua
-- Script: hello.lua

gl = require("moongl")
glfw = require("moonglfw")

glfw.window_hint('context version major', 3)
glfw.window_hint('context version minor', 3)
glfw.window_hint('opengl profile', 'core')

window = glfw.create_window(600, 400, "Hello, World!")
glfw.make_context_current(window)
gl.init() -- this is actually glewInit()

function reshape(_, w, h) 
   print("window reshaped to "..w.."x"..h)
   gl.viewport(0, 0, w, h)
end

glfw.set_window_size_callback(window, reshape)

while not glfw.window_should_close(window) do
   glfw.poll_events()
   -- ... rendering code goes here ...
   gl.clear_color(1.0, 0.5, 0.2, 1.0) -- GLFW orange
   gl.clear("color", "depth")
   glfw.swap_buffers(window)
end
```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua hello.lua
```

#### See also

* [MoonLibs - Graphics and Audio Lua Libraries](https://github.com/stetre/moonlibs).
