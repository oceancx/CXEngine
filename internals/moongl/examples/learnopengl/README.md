## Learn OpenGL

This is a Lua port of the C++ examples from Joey de Vries' 
[**Learn OpenGL**](https://learnopengl.com) tutorial.

All shaders, textures, and models used in these examples are taken from the original,
and the respective original licenses apply (see the above link for details).

Resources such as texture images and object models are actually not included in this repository,
so to successfully run the examples you'll have to copy the contents of the `resources/`
directory of the [original](https://github.com/JoeyDeVries/LearnOpenGL) into the `resources/`
directory of this one.

The following libraries from the [MoonLibs](https://github.com/stetre/moonlibs) collection are used here:

* [MoonGL](https://github.com/stetre/moongl): bindings to the OpenGL API.
* [MoonGLFW](https://github.com/stetre/moonglfw): bindings to GLFW, for window/surface creation and input handling.
* [MoonGLMATH](https://github.com/stetre/moonglmath): graphics math library, for vertex transformations.
* [MoonImage](https://github.com/stetre/moonimage): bindings to stb_image, for image loading.
* [MoonAssimp](https://github.com/stetre/moonassimp): bindings to Assimp, for model loading.

