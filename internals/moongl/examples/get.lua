#!/usr/bin/env lua
-- MoonGL example: get.lua
-- 
-- Attempts to retrieve all parameters using the gl.get() function, and prints
-- their values (or an error message) on stdout.
--
-- Usage:
-- $ ./get.lua
-- or
-- $ lua get.lua
-- 

gl = require("moongl")
glfw = require("moonglfw")

-- We don't really need a window for this example, but we do need a GL context to
-- initialize OpenGL, so we create one with MoonGLFW:
window = glfw.create_window(100,100)
glfw.make_context_current(window)
gl.init()


-- Retrieve the OpenGL version in use:
ver = gl.get("major version") .. "." .. gl.get("minor version")


function get(pname)
-- This is a wrapper that calls the gl.get() function in protected mode, in order
-- to check if pname is supported by the OpenGL version being used.
-- The index 0 is used for all pnames (the index is ignored if pname does not require one).
   local res = { pcall(gl.get, pname, 0) }
   -- on error, res = { false, errmsg }
   -- on success, res = { true, ... results .. }
   
   if not res[1] then
      local errmsg = res[2]
      if errmsg == 'invalid enumerant' then
         print("NOT SUPPORTED (OpenGL "..ver .. "):", pname)
      else
         print("ERROR ('".. errmsg.."'):", pname)
      end
      return
   end
   print(pname, ":", table.unpack(res, 2))
end

-- Get values (or errors) for all pnames:
get("context flags")
get("major version")
get("max server wait timeout")
get("min map buffer alignment")
get("minor version")
get("num extensions")
get("num shading language versions")
get("timestamp")
get("array buffer binding")
get("copy read buffer binding")
get("copy write buffer binding")
get("draw indirect buffer binding")
get("element array buffer binding")
get("query buffer binding")
get("texture buffer binding")
get("vertex array binding")
get("max atomic counter buffer bindings")
get("atomic counter buffer binding")
get("atomic counter buffer start")
get("atomic counter buffer size")
get("max shader storage buffer bindings")
get("shader storage buffer binding")
get("shader storage buffer start")
get("shader storage buffer size")
get("max transform feedback buffers")
get("max transform feedback buffer bindings") 
get("transform feedback buffer binding")
get("transform feedback buffer start")
get("transform feedback buffer size")
get("max uniform buffer bindings")
get("uniform buffer binding")
get("uniform buffer start")
get("uniform buffer size")
get("max transform feedback interleaved components")
get("max transform feedback separate attribs")
get("max transform feedback separate components")
get("transform feedback paused")
get("transform feedback active")
get("debug group stack depth")
get("debug logged messages")
get("debug next logged message length")
get("debug output")
get("debug output synchronous")
get("max debug logged messages")
get("max debug message length")
get("max debug group stack depth")
get("max label length")
get("color clear value")
get("color writemask")
get("depth clear value")
get("depth func")
get("depth test")
get("depth writemask")
get("doublebuffer")
get("draw buffer")
get("draw buffer 0")
get("draw buffer 1")
get("draw buffer 2")
get("draw buffer 3")
get("draw buffer 4")
get("draw buffer 5")
get("draw buffer 6")
get("draw buffer 7")
get("draw buffer 8")
get("draw buffer 9")
get("draw buffer 10")
get("draw buffer 11")
get("draw buffer 12")
get("draw buffer 13")
get("draw buffer 14")
get("draw buffer 15")
get("draw framebuffer binding")
get("max color attachments")
get("max color texture samples")
get("max depth texture samples")
get("max draw buffers")
get("max dual source draw buffers")
get("max framebuffer height")
get("max framebuffer layers")
get("max framebuffer samples")
get("max framebuffer width")
get("max integer samples")
get("max samples")
get("read buffer")
get("read framebuffer binding")
get("renderbuffer binding")
get("stencil back fail")
get("stencil back func")
get("stencil back pass depth fail")
get("stencil back pass depth pass")
get("stencil back ref")
get("stencil back value mask")
get("stencil back writemask")
get("stencil clear value")
get("stencil fail")
get("stencil func")
get("stencil pass depth fail")
get("stencil pass depth pass")
get("stencil ref")
get("stencil test")
get("stencil value mask")
get("stencil writemask")
get("stereo")
get("fragment shader derivative hint")
get("line smooth hint")
get("polygon smooth hint")
get("texture compression hint")
get("image binding access")
get("image binding format")
get("image binding name")
get("image binding layer")
get("image binding layered")
get("image binding level")
get("max sample mask words")
get("multisample")
get("sample alpha to coverage")
get("sample alpha to one")
get("sample buffers")
get("sample coverage")
get("sample coverage invert")
get("sample coverage value")
get("sample mask")
get("sample mask value")
get("samples")
get("blend")
get("blend color")
get("blend dst alpha")
get("blend dst rgb")
get("blend equation rgb")
get("blend equation alpha")
get("blend src alpha")
get("blend src rgb")
get("color logic op")
get("dither")
get("logic op mode")
get("scissor box")
get("scissor test")
get("clamp read color")
get("implementation color read format")
get("implementation color read type")
get("pack alignment")
get("pack compressed block depth")
get("pack compressed block height")
get("pack compressed block size")
get("pack compressed block width")
get("pack image height")
get("pack lsb first")
get("pack row length")
get("pack skip images")
get("pack skip pixels")
get("pack skip rows")
get("pack swap bytes")
get("pixel pack buffer binding")
get("pixel unpack buffer binding")
get("unpack alignment")
get("unpack compressed block depth")
get("unpack compressed block height")
get("unpack compressed block size")
get("unpack compressed block width")
get("unpack image height")
get("unpack lsb first")
get("unpack row length")
get("unpack skip images")
get("unpack skip pixels")
get("unpack skip rows")
get("unpack swap bytes")
get("current program") 
get("max atomic counter buffer size")
get("max combined shader output resources") 
get("max combined shader storage blocks") 
get("max image samples") 
get("max image units") 
get("max program texel offset") 
get("max shader storage block size") 
get("max subroutines") 
get("max subroutine uniform locations") 
get("max uniform block size") 
get("max uniform locations") 
get("max varying vectors") 
get("max vertex attrib relative offset") 
get("max vertex attrib bindings") 
get("max vertex attrib stride") 
get("min program texel offset") 
get("num program binary formats") 
get("num shader binary formats") 
get("program binary formats") 
get("program pipeline binding") 
get("shader binary formats") 
get("shader compiler")
get("shader storage buffer offset alignment") 
get("uniform buffer offset alignment") 
get("viewport index provoking vertex")
get("layer provoking vertex")
get("provoking vertex")
get("aliased line width range")
get("cull face") 
get("cull face mode") 
get("front face") 
get("line smooth") 
get("line width") 
get("point fade threshold size") 
get("point size") 
get("point size granularity") 
get("point size range") 
get("point sprite coord origin") 
get("polygon mode") 
get("polygon offset factor")
get("polygon offset fill") 
get("polygon offset line") 
get("polygon offset point") 
get("polygon offset units")
get("polygon smooth") 
get("program point size") 
get("rasterizer discard") 
get("smooth line width range") 
get("smooth line width granularity") 
get("subpixel bits") 
get("fragment interpolation offset bits")
get("max fragment interpolation offset")
get("min fragment interpolation offset")
get("max combined atomic counter buffers")
get("max compute atomic counter buffers")
get("max fragment atomic counter buffers")
get("max geometry atomic counter buffers")
get("max tess control atomic counter buffers")
get("max tess evaluation atomic counter buffers")
get("max vertex atomic counter buffers")
get("max combined atomic counters")
get("max compute atomic counters")
get("max fragment atomic counters")
get("max geometry atomic counters")
get("max tess control atomic counters")
get("max tess evaluation atomic counters")
get("max vertex atomic counters")
get("max combined compute uniform components")
get("max combined fragment uniform components")
get("max combined geometry uniform components")
get("max combined tess control uniform components")
get("max combined tess evaluation uniform components")
get("max combined vertex uniform components")
get("max combined image uniforms")
get("max compute image uniforms")
get("max fragment image uniforms")
get("max geometry image uniforms")
get("max tess control image uniforms")
get("max tess evaluation image uniforms")
get("max vertex image uniforms")
get("max compute shader storage blocks")
get("max fragment shader storage blocks")
get("max geometry shader storage blocks")
get("max tess control shader storage blocks")
get("max tess evaluation shader storage blocks")
get("max vertex shader storage blocks")
get("max compute uniform components")
get("max fragment uniform components")
get("max geometry uniform components")
get("max tess control uniform components")
get("max tess evaluation uniform components")
get("max vertex uniform components")
get("max texture image units")
get("max combined texture image units")
get("max compute texture image units")
get("max geometry texture image units")
get("max tess control texture image units")
get("max tess evaluation texture image units")
get("max vertex texture image units")
get("max combined uniform blocks")
get("max compute uniform blocks")
get("max fragment uniform blocks")
get("max geometry uniform blocks")
get("max tess control uniform blocks")
get("max tess evaluation uniform blocks")
get("max vertex uniform blocks")
get("dispatch indirect buffer binding")
get("max compute shared memory size")
get("max compute work group invocations")
get("max compute work group count")
get("max compute work group size")
get("max fragment input components")
get("max fragment uniform vectors")
get("max program texture gather offset")
get("min program texture gather offset")
get("max geometry input components")
get("max geometry output components")
get("max geometry output vertices")
get("max geometry shader invocations")
get("max geometry total output components")
get("max vertex streams")
get("max patch vertices")
get("max tess control input components")
get("max tess control output components")
get("max tess control total output components")
get("max tess gen level")
get("max tess patch components")
get("patch default inner level")
get("patch default outer level")
get("patch vertices")
get("max tess evaluation input components")
get("max tess evaluation output components")
get("max vertex attribs")
get("max vertex output components")
get("max vertex uniform vectors")
get("active texture")
get("compressed texture formats")
get("max 3d texture size")
get("max array texture layers")
get("max cube map texture size")
get("max rectangle texture size")
get("max renderbuffer size")
get("max texture buffer size")
get("max texture lod bias")
get("max texture size")
get("num compressed texture formats")
get("sampler binding")
get("texture binding 1d")
get("texture binding 1d array")
get("texture binding 2d")
get("texture binding 2d array")
get("texture binding 2d multisample")
get("texture binding 2d multisample array")
get("texture binding 3d")
get("texture binding buffer")
get("texture binding cube map")
get("texture binding rectangle")
get("texture buffer offset alignment")
get("texture cube map seamless")
get("clip distance 0")
get("clip distance 1")
get("clip distance 2")
get("clip distance 3")
get("clip distance 4")
get("clip distance 5")
get("clip distance 6")
get("clip distance 7")
get("depth clamp")
get("depth range")
get("max clip distances")
get("max viewport dims")
get("max viewports")
get("transform feedback binding")
get("viewport")
get("viewport bounds range")
get("viewport subpixel bits")
get("max element index")
get("max elements indices")
get("max elements vertices")
get("primitive restart")
get("primitive restart fixed index")
get("primitive restart for patches supported")
get("primitive restart index")
get("vertex binding divisor")
get("vertex binding offset")
get("vertex binding stride")

